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
        int move(unsigned int id, int d ){

            unsigned int distanceTraveled = 0;
            Robot robot = robots[id];
            Position position = robot.getPosition();

            for(unsigned int i = 1; i <=d; ++i){

            }

            return distanceTraveled ;
        }

        int turn(unsigned int id, int d){
            robots[id].setPosition(robots[id].getPosition + d);
            return d;
        }

        void init(unsigned int id, int x, int y, int heading){
            Robot& robotToChange = robots[id];
            robotToChange.setPosition(Position(x,y));
            robotToChange.setHeading(heading);

        }

        Robot curr(unsigned int id){
            return robots[id];
        }

        void join(unsigned int id, int x, int y ){
            Robot& robotToMove = robots[id];
            Position currentPosition = robotToMove.getPosition();

            float teta;
            float diffX = (float)x - currentPosition.getX();
            float diffY = (float) y - currentPosition.getY();
            if(diffX == 0 && diffY == 0)
                return; //on retourne la position actuelle, on a pas bougé

            if(diffX > 0 && diffY > 0) //1er cadrant
                teta = teta;
            if(diffX < 0 && diffY > 0) //2eme cadrant
                teta = 180 - teta;
            if(diffX < 0 && diffY < 0) //3eme cadrant
                teta = 180 + teta;
            if(diffX > 0 && diffY < 0)//4eme cadrant
                teta = 360 - teta;
            if(diffX == 0)
                teta = 90;
            if(diffY == 0)
                teta = 0;

            teta = abs(teta - robotToMove.getHeading());


};

#endif // MAP_H
