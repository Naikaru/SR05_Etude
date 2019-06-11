#include "pil.h"
#include "tcpclientmanager.h"
int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    Pil pil(argc, argv);
    pil.show();


    Map m;
    m.show();
    for(int i = 0; i<20;i++)
        m.initRobot(i,i,i,0);

     m.move(0,5);
     m.turn(0,90);
     m.move(0,4);
     m.turn(0,-90);
     m.move(0,5);

    TcpClientManager client;
    client.connect(&client, SIGNAL(receivedMessage(Message)), &pil, SLOT(rmtMessage(Message)));
    client.connectToRobot(QHostAddress("127.0.0.1"), 4646);

    return app.exec();
}
