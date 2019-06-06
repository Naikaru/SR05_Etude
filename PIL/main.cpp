#include "pil.h"

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
    //Pil pil(argc, argv);
    //pil.show();

    Map m;
    m.show();
    for(int i = 0; i<20;i++)
        m.initRobot(i,i,0,0);

    m.move(0,5);
    m.turn(0,270);
    m.move(0,5);
    m.turn(0,90);
    m.move(0,5);

    return app.exec();
}
