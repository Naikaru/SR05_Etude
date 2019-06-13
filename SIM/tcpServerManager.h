#ifndef SOCKET_H
#define SOCKET_H
#include <QSocketNotifier>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QDebug>
#include <QObject>
#include <QTest>
#include "message.h"
class socket : public QObject
{
    Q_OBJECT
private:
QTcpServer server;
QTcpSocket * sock = nullptr;
int handshakeState = 0;
const QString handshakeMessages[4] = {QString("^ ~ app beglch begair endlch endair rmtend"),
                                      QString("^app~PIL"),
                                      QString("ROB"),
                                      QString("^begair~PIL")};

int robotId;
public:
    socket(QHostAddress address, int port = 4646);
    ~socket();
    void send(QString message) const;
    void send(Message message) const;
    int getRobotId() const;

private slots:
    void handleConnect();
    void handleMessage();
signals :
    void recievedMessage(const Message& message);


};

#endif // SOCKET_H
