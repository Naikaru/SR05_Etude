#include "tcpServerManager.h"
int socket::getRobotId() const
{
    return robotId;
}

socket::socket(QHostAddress address , int port):server()
{
    robotId = address.toString().left(1).toInt();

    if (server.listen(address, port)){
        qDebug() << QString("Server listening on ")+address.toString()+":"+QString(std::to_string(port).c_str());
    } else {
        qDebug() << QString("listening failed");
        throw std::runtime_error("Server creation failed");
    }
    QObject::connect(&server, SIGNAL(newConnection()), this, SLOT(handleConnect()));
}

void socket::send(QString message){
    if (sock == nullptr || !sock->isOpen()){
        throw std::runtime_error("No socket to send the message on");
    } else {
        sock->write(message.toStdString().c_str());
    }
}

void socket::send(Message message){
    send(message.getCompleteMessage());
}

// Slots definition:
void socket::handleConnect(){
    qDebug() << QString("Got new connection");
    if (sock != nullptr){
        sock->close();
        delete sock;
    }
    // update the socket
    sock = server.nextPendingConnection();
    QObject::connect(sock, SIGNAL(readyRead()), this, SLOT(handleMessage()));
    handshakeState = 0;
    send(handshakeMessages[handshakeState++]);
}

void socket::handleMessage()
{
    while (sock->bytesAvailable()>0){
        QString message = QString(sock->readLine(2048));
        qDebug() << message << QString(std::to_string(handshakeState).c_str());
        if (handshakeState < 4){
            if (handshakeState == 3 && message == handshakeMessages[handshakeState++]){
                qDebug() << "handshake success";
            } else if (message == handshakeMessages[handshakeState++]){
                send(handshakeMessages[handshakeState++]);
            } else {
                sock->abort();
                qDebug() << "handshake failed, aborting connexion";
                delete sock;
                sock = nullptr;
                break;
            }
        } else {
            qDebug() << "Envoie au manager";
            emit recievedMessage(Message(message));
        }
    }
}
