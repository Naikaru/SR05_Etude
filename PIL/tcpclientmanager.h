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
    explicit TcpClientManager(QObject *parent = 0);
//    TcpClientManager();
    void connectToRobot(QHostAddress addr, int port);
    void send(QString message);
    void useMessage();
//    void send()

signals:
    void receivedMessage(Message message);
private slots:
    void handleErrors(QAbstractSocket::SocketError);
public slots:
    void handleMessage();
};

#endif // TCPCLIENTMANAGER_H
