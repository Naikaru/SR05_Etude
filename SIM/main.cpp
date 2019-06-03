#include "map.h"
#include "messagemanager.h"
#include "position.h"
#include "robot.h"
#include <iostream>

int main(int argc, char* argv[]) {

//    QApplication app(argc, argv);
//    Pil pil(argc, argv);
//    pil.show();


    Position origine(0, 0);
    Position milieu(5, 5);
    Map m(1, 10, 10);
    Robot robot(0, milieu);

    std::cout << "Coordonées : " << robot.getPosition().getX() << "," << robot.getPosition().getY() << std::endl;

    robot.setHeading(230);

    std::cout << "Heading : " << robot.getHeading() << std::endl;

    m.addRobot(1, robot);

    m.changeState(3,3,full);

    //std::cout << "X : " << robot.getPosition().getX() << std::endl;
    //std::cout << "Y : " << robot.getPosition().getY() << std::endl;


    m.move(1, 4);

    std::cout << "Coordonées : " << m.getRobots()[1].getPosition().getX() << "," << m.getRobots()[1].getPosition().getY() << std::endl;


}
