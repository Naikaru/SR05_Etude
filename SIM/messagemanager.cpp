#include "messagemanager.h"

MessageManager::MessageManager(QString app, QString who, QString whe, QWidget *parent) : QWidget(parent), APP(app), WHO(who), WHE(whe)
{


    t_displayMessage = new QTextEdit(this);
    t_displayMessage ->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    t_displayMessage ->setFocusPolicy(Qt::NoFocus);
    t_displayMessage ->setFixedSize(500, 50);

    signalMapper = new QSignalMapper(this);

}

MessageManager::~MessageManager()
{
    for(auto s : sockets){
        delete s.second;
    }
    sockets.clear();

}

void MessageManager::handleMessage(const Message &msg)
{
    socket *socketSender = (socket * ) sender();
    socketSender->getRobotId();

    std::pair<int,Message> message(socketSender->getRobotId(), msg);
    addMessageInDisplay(msg);
    emit receivedMessageFromRobot(message);

}

bool MessageManager::addRobotSocket(QString adress)
{

    int id = adress.right(1).toInt();
    if(sockets.find(id) != sockets.end())
        return false;

    //QString address = QString("127.0.0.") + QString::number(id);

    sockets[id] = new socket(QHostAddress (adress));

    QObject::connect(sockets[id], SIGNAL(recievedMessage(const Message&)), this, SLOT(handleMessage(const Message&)));

    return true;
}

void MessageManager::sendMessage(int id, const Message &msg)
{
    qDebug() << "sending " <<msg.getCompleteMessage();
    sockets[id]->send(msg);
    t_displayMessage->setTextColor(Qt::blue);
    t_displayMessage->append(QDateTime::currentDateTime().time().toString() + QString(" >>  " +  msg.getCompleteMessage()));
}

void MessageManager::addMessageInDisplay(const Message &msg)
{
    t_displayMessage->setTextColor(Qt::green);
    t_displayMessage->append(QDateTime::currentDateTime().time().toString() + QString(" << " + msg.getCompleteMessage()));
}



