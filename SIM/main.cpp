#include "map.h"
#include "message.h"
#include "tcpServerManager.h"
#include "position.h"
#include "robot.h"
#include <iostream>
#include <QDebug>
#include <ctime>
#include "simgui.h"
#include "mapgui.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

//    Pil pil(argc, argv);
//    pil.show();

//    Position origine(0, 0);
//    Position milieu(5, 5);
//    Map m(1, 10, 10);
//    Robot robot(0, milieu);

//    QString message("WHEROBLCH/mnemo~value3/mnemo1~value2");
//    Message manager(message);
//    qDebug() << manager.getAPP() << manager.getWHE() << manager.getWHO();
//    qDebug() << manager.getValue("mnemo") << manager.getValue("mnemo1");
//    qDebug() << manager.getCompleteMessage();
//    socket sockServ(QHostAddress("127.0.0.1"), 4646);
//    socket sockServ2(QHostAddress("127.0.0.2"), 4646);

//    std::cout << "Coordonées : " << robot.getPosition().getX() << "," << robot.getPosition().getY() << std::endl;

//    robot.setHeading(230);

//    std::cout << "Heading : " << robot.getHeading() << std::endl;

//    m.addRobot(1, robot);

//    m.changeState(3,3,full);

//    //std::cout << "X : " << robot.getPosition().getX() << std::endl;
//    //std::cout << "Y : " << robot.getPosition().getY() << std::endl;


//    m.move(1, 4);

//    std::cout << "Coordonées : " << m.getRobots()[1].getPosition().getX() << "," << m.getRobots()[1].getPosition().getY() << std::endl;
/*
    std::srand(std::time(nullptr));
    Position origine(0, 0);
    Position droite(5, 0);
    Position basGauche(-20, -20);
    Map m(5, 10, 10);
   // MapGui mapGui;
    Robot robot(0, origine);
    initMap(m);

    robot.setHeading(0);

    //mapGui.move(1,2);
    m.addRobot(1, robot);
    printMap(m);

    Position testPos = Position(0,0);

    //std::cout << "Position : " << testGrid.getX() << " " << testGrid.getY();
    m.move(1,5);
    printMap(m);

    std::cout << " 0 5 : " << m.getRobots().at(1) << std::endl;
    m.move(1,-5);
    printMap(m);
    m.turn(1,180);
    m.move(1,2);
    printMap(m);

    m.join(1,2,3);
    printMap(m);

    std::cout << "Robot en 2 3 : " << m.getRobots().at(1) << std::endl;

    m.join(1, -2, -3);
    printMap(m);
    std::cout << "Robot en -2 -3 : " << m.getRobots().at(1) << std::endl;
    Position collision = m.getCoordinatesFromPosition(Position( 3,3));
    m.changeState(collision.getX(), collision.getY(),full);
    m.join(1,2,2);
    printMap(m);
    std::cout << "Robot en 2 2 : " << m.getRobots().at(1) << std::endl;
    m.join(1,4,4);
    std::cout << "colision en 3 3: " << m.getRobots().at(1) << std::endl;
    printMap(m);

    */


    MapGui mapGui(nullptr);
    mapGui.show();
    return app.exec();
}
