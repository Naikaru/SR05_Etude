#ifndef TCPCLIENTMANAGER_H
#define TCPCLIENTMANAGER_H

#include <QTcpSocket>
#include <QHostAddress>
#include "message.h"

class TcpClientManager : public QObject
{
    Q_OBJECT
private:
    QTcpSocket sock;
    int handshakeState;
    QString APP;
    QString delim;
    QString equal;
    QString mnemoapp;
    QString mnemobeglch;
    QString mnemobegair;
    QString mnemoendlch;
    QString mnemoendair;
    QString mnemodisco;
    QStringList filtre;
public:
    QString getDelim(){
        return delim;
    }
    QString getEqual() {
        return equal;
    }
    QString getMnemoapp() {
        return mnemoapp;
    }
    QString getMnemobeglch() {
        return mnemobeglch;
    }
    QString getMnemobegair() {
        return mnemobegair;
    }
    QString getMnemoendlch() {
        return mnemoendlch;
    }
    QString getMnemoendair() {
        return mnemoendair;
    }
    QString getMnemodisco() {
        return mnemodisco;
    }

    explicit TcpClientManager(QObject *parent = 0, QString app_name = "PIL");
    void connectToRobot(QHostAddress addr, int port);
    bool send(QString message);
    bool send(Message message);
    bool isHandshakeFinished(){ return (handshakeState == 3); }


signals:
    void receivedMessage(Message message);
    void handshakeFinished();
private slots:
    void handleErrors(QAbstractSocket::SocketError);
    void handleMessage();
};

#endif // TCPCLIENTMANAGER_H
