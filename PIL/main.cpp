#include "pil.h"

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
    Pil pil(argc, argv);
    pil.addMovementInBuffer(1, "init", "0,0", "10,20,0");
    pil.addMovementInBuffer(2, "move", "10,20", "0,0,0");
    pil.addMovementInBuffer(3, "turn", "90,90", "10,20,0");
    QStringList list = pil.getBuffer().first();
    qDebug() << list[0] << ":" << list[1] << ":" << list[2] << ":" << list[3];
    pil.sendBufferToNet();
    //Map m;
    pil.applyBufferFromMessage(QString::fromStdString("/who~1/payload~@buffer|1:init:0,0:10,20,0|2:move:10,20:0,0,0|3:turn:90,90:10,20,0/nseq~1/dest~-1"));
    pil.show();
//    for(int i = 0; i<20;i++)
    //m.initRobot(0,25,25,0);
    //m.turn(0,180);
    //m.move(0,-15);

    //m.move(0,5);
    //m.turn(0,90);
   // m.move(0,5);
    //m.show();



    return app.exec();
}
