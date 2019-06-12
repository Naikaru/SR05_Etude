#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QDateTime>
#include <QVBoxLayout>
#include "message.h"
#include "map.h"
#include "tcpServerManager.h"

class MessageManager : public QWidget
{
    Q_OBJECT


    QTextEdit * t_displayMessage;

    QString APP;
    QString WHO;
    QString WHE;

    const QString mnemoAckMove = "moved";
    const QString menmoAckTurn = "turned";
    const QString mnemoAckError = "order";


    std::map<int, socket *> sockets;
    QSignalMapper * signalMapper;

    void addMessageInDisplay(const Message& msg);

public:
    explicit MessageManager(QString app, QString who, QString whe, QWidget *parent = 0);

    ~MessageManager();
    bool addRobotSocket(QString adress);
    bool removeRobotSocket(int id);
    Message createMessage() const {return Message(APP, WHO, WHE);}
    void sendMessage(int id, const Message& msg);

    void show()
    {
        QWidget::show();
    }

signals:

    void receivedMessageFromRobot(std::pair<int,Message> msg);

private slots:
    void handleMessage(const Message& msg);

};

#endif // MESSAGEMANAGER_H
