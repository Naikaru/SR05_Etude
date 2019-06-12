#include "tcpclientmanager.h"

TcpClientManager::TcpClientManager(QObject *parent, QString app_name) : QObject(parent),
    sock(this), APP(app_name)
{
    connect(&sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleErrors(QAbstractSocket::SocketError)));
    connect(&sock, SIGNAL(readyRead()), this, SLOT(handleMessage()));
}

void TcpClientManager::connectToRobot(QHostAddress addr, int port){
    handshakeState = 1;
    sock.connectToHost(addr, port);

}

bool TcpClientManager::send(QString message){
    if (sock.isOpen() && handshakeState > 1){
        if (sock.write(message.toStdString().c_str()) >= 0){
            return true;
        }
    }
    return false;
}
bool TcpClientManager::send(Message message){
    return this->send(message.getCompleteMessage());
}


// Slots
void TcpClientManager::handleMessage(){
    while (sock.bytesAvailable()>0){
        QString message = QString(sock.readLine(2048)).simplified();
        //qDebug() << message;
        if (message == "") {
            return;
        } else {
            switch (handshakeState) {
            case 1: {
                // get the parameters from the message
                QStringList params = message.split(" ");
                if (params.length() < 8){
                    sock.close();
                    //qDebug() << QString("Error : wrong number of parameters");
                    handshakeState = 0;
                    return;
                }
                delim = params[0];
                equal = params[1];
                mnemoapp = params[2];
                mnemobeglch = params[3];
                mnemobegair = params[4];
                mnemoendlch = params[5];
                mnemoendair = params[6];
                mnemodisco = params[7];

                //Beginning phase 2
                handshakeState = 2;
                send(delim+mnemoapp+equal+APP);
                break;
            }
            case 2: {
                filtre = message.split(" ");
                //qDebug()<<filtre;
                send(delim+mnemobegair+equal+APP);
                handshakeState = 3;
                emit handshakeFinished();
                break;
            }
            case 3:
                emit receivedMessage(Message(message));
                break;
            default:
                //qDebug()<<QString("Error ! unknown state");
                break;
            }
        }
    }
}

void TcpClientManager::handleErrors(QAbstractSocket::SocketError){
    //qDebug() << "error : " + sock.errorString();
}


