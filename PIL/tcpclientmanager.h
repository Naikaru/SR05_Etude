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
    void send(QString message);
    void send(Message message);


signals:
    void receivedMessage(Message message);
private slots:
    void handleErrors(QAbstractSocket::SocketError);
    void handleMessage();
};

#endif // TCPCLIENTMANAGER_H
