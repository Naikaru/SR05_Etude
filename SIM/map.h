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
#include "mathhelper.h"



enum CellState{
    empty,
    full
};

class Map
{

protected :
    std::map<int, Robot> robots;
    unsigned int nbRobotMax;


private:
    CellState **grid;
    unsigned int nbRows;
    unsigned int nbCols;


    public:
    Position getCoordinatesFromPosition(const Position &position) const;
    Position getPositionFromCoordinates(const Position &coordinates) const;

    unsigned int convert(unsigned int coord, unsigned int dim);

        //ctor
        Map();
        Map(unsigned int nbRobotMax, unsigned int nbCols, unsigned int nbRows);
        //getter and setter
        const std::map<int, Robot>& getRobots() const;
        bool addRobot(int id, const Robot& newRobot);
        bool deleteRobot(int id);

        unsigned int getNbRows() const;
        void setNbRows(unsigned int value);
        unsigned int getNbCols() const;
        void setNbCols(unsigned int value);
        CellState **getGrid() const;
        void setGrid(CellState **value);
        unsigned int getNbRobotMax() const;
        void setNbRobotMax(unsigned int value);

        void initMap();
        void printMap();


        //getter & setter map
        void changeState(unsigned int i, unsigned int j, CellState newState);
        CellState getState(unsigned int i, unsigned int j) const;




        //Robot movements
        int move(unsigned int id, int d );

        int turn(unsigned int id, int angle);

        void init(unsigned int id, int x, int y, int heading);

        const Robot& curr(unsigned int id) const;

        const Robot &join(unsigned int id, int x, int y );

};

#endif // MAP_H
