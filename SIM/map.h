#ifndef MAP_H
#define MAP_H


#define _USE_MATH_DEFINES

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
#include "math.h"



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
        int move(unsigned int id, int d );

        int turn(unsigned int id, int d);

        void init(unsigned int id, int x, int y, int heading);

        Robot curr(unsigned int id);

        void join(unsigned int id, int x, int y );
};

#endif // MAP_H
