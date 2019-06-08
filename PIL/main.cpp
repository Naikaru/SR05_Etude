#include "pil.h"

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
//    Pil pil(argc, argv);
//    pil.show();
//    Position p;
//    p.x = 10;
//    p.y = 0;
//    State etat = free_cell;
//    pil.addPositionInBuffer(p, etat);
//    pil.addPositionInBuffer(p, etat);
//    QPair<Position, State> pair = pil.getBuffer().first();
//    std::cout << pair.first.x << "|" << pair.first.y << "|" << pair.second << std::endl;
//    pil.sendBufferToNet();

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
