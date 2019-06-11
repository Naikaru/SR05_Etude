#include "pil.h"
#include "tcpclientmanager.h"
int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
    Pil pil(argc, argv);
    pil.addMovementInBuffer(1, "init", "0,0", "10,20,0");
    pil.addMovementInBuffer(2, "move", "10,20", "0,0,0");
    pil.addMovementInBuffer(3, "turn", "90,90", "10,20,0");
    QStringList list = pil.getBuffer().first();
    qDebug() << list[0] << ":" << list[1] << ":" << list[2] << ":" << list[3];
    pil.sendBufferToNet();
    pil.applyBufferFromMessage(QString::fromStdString("/who~1/payload~@buffer|1:init:0,0:10,20,0|2:move:10,20:0,0,0|3:turn:90,90:10,20,0/nseq~1/dest~-1"));
    pil.show();


    TcpClientManager client;
    client.connect(&client, SIGNAL(receivedMessage(Message)), &pil, SLOT(rmtMessage(Message)));
    client.connectToRobot(QHostAddress("127.0.0.1"), 4646);

    return app.exec();
}
