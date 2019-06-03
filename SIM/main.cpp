#include "map.h"
#include "messagemanager.h"
#include "position.h"
#include "robot.h"
#include <iostream>
#include <QDebug>
#include <ctime>

void printMap(const Map& m){
    std::cout << "Map : \n";
    for(unsigned int i = 0; i< m.getNbRows(); ++i){
        for(unsigned int j=0; j < m.getNbCols(); ++j){
           std::cout << ((m.getState(i,j) == CellState::empty) ? "_" : "X");
        }
        std::cout << std::endl;
    }
}

void initMap(Map& m){
    for(unsigned int i = 0; i< m.getNbRows(); ++i){
        for(unsigned int j=0; j < m.getNbCols(); ++j){
            int obstacle = std::rand()%10;
            //if(obstacle == 5 || i == 0 || j == 0 || j == (m.getNbCols() - 1) ||i == (m.getNbRows() - 1)){
              //  m.changeState(i,j,CellState::full);
            //}
            //else{
                m.changeState(i,j,CellState::empty);
            //}
        }
    }
}

int main(int argc, char* argv[]) {

//    QApplication app(argc, argv);
//    Pil pil(argc, argv);
//    pil.show();

    std::srand(std::time(nullptr));
    Position origine(0, 0);
    Position droite(5, 0);
    Position basGauche(-20, -20);
    Map m(5, 10, 10);
    Robot robot(0, origine);
    initMap(m);
    printMap(m);

    robot.setHeading(0);


    m.addRobot(1, robot);
    Position testPos = Position(0,0);

    //std::cout << "Position : " << testGrid.getX() << " " << testGrid.getY();
    m.move(1,5);

    std::cout << " 0 5 : " << m.getRobots().at(1) << std::endl;
    m.move(1,-5);
    m.turn(1,180);
    m.move(1,2);

    m.join(1,2,3);

    std::cout << "Robot en 2 3 : " << m.getRobots().at(1) << std::endl;

    m.join(1, -2, -3);
    std::cout << "Robot en -2 -3 : " << m.getRobots().at(1) << std::endl;
    Position collision = m.getCoordinatesFromPosition(Position( 3,3));
    m.changeState(collision.getX(), collision.getY(),full);
    m.join(1,2,2);
    std::cout << "Robot en 2 2 : " << m.getRobots().at(1) << std::endl;
    m.join(1,4,4);
    std::cout << "colision en 3 3: " << m.getRobots().at(1) << std::endl;

}
