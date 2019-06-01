#ifndef MAP_H
#define MAP_H

#include <QtWidgets>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <regex>
#include <fstream>
#include "robot.h"
#include "position.h"



enum CellState{
    empty,
    full
};

class Map
{

    std::map<int, Robot> robots;
    unsigned int nbRobotMax;

    unsigned int nbRows;
    unsigned int nbCols;

    CellState **grid;

    Position getCoordinatesFromPosition(const Position &position);

    public:

        //ctor
        Map();
        Map(unsigned int nbRobotMax, unsigned int nbRows, unsigned int nbCols);
        //getter and setter
        std::map<int, Robot> getRobots() const;
        bool addRobot(int id, const Robot& newRobot);

        unsigned int getNbRows() const;
        void setNbRows(unsigned int value);
        unsigned int getNbCols() const;
        void setNbCols(unsigned int value);
        CellState **getGrid() const;
        void setGrid(CellState **value);
        unsigned int getNbRobotMax() const;
        void setNbRobotMax(unsigned int value);


        //getter & setter map
        void changeState(unsigned int i, unsigned int j, CellState newState);
        CellState getState(unsigned int i, unsigned int j);




        //Robot movements
        int move(unsigned int id, int d ){

            unsigned int distanceTraveled = 0;
            Robot robot = robots[id];
            Position position = robot.getPosition();

            for(unsigned int i = 1; i <=d; ++i){

            }

            return distanceTraveled ;
        }

        int turn(unsigned int id, int d){

        }

        void init(unsigned int id, int x, int y, int heading){

        }

        void curr(unsigned int id){

        }

        void join(unsigned int id, int x, int y ){

        }




};

#endif // MAP_H
