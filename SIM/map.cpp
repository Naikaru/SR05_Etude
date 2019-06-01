#include "map.h"



std::map<int,Robot> Map::getRobots() const
{
    return robots;
}

bool Map::addRobot(int id, const Robot &newRobot)
{
    if(robots.size() < nbRobotMax)
    {
        if(robots.find(id) == robots.end())
        {
            robots[id] = newRobot;
            return true;
        }
    }

    return false;
}



unsigned int Map::getNbRows() const
{
    return nbRows;
}

void Map::setNbRows(unsigned int value)
{
    nbRows = value;
}

unsigned int Map::getNbCols() const
{
    return nbCols;
}

void Map::setNbCols(unsigned int value)
{
    nbCols = value;
}

CellState **Map::getGrid() const
{
    return grid;
}

void Map::setGrid(CellState **value)
{
    grid = value;
}

unsigned int Map::getNbRobotMax() const
{
    return nbRobotMax;
}

void Map::setNbRobotMax(unsigned int value)
{
    nbRobotMax = value;
}

void Map::changeState(unsigned int i, unsigned int j, CellState newState){
    if( i >= nbRows || j >= nbCols){
        throw new QString("Out of Bounds");
    }
    grid[i][j] = newState;
}

CellState Map::getState(unsigned int i, unsigned int j)
{
    if( i >= nbRows || j >= nbCols){
            throw new QString("Out of Bounds");
    }
    return grid[i][j];
}

Position Map::getCoordinatesFromPosition(const Position& position)
{

    int x = position.getX() / 2;
    int y = position.getY() / 2;
    if(nbRows%2 == 0 && position.getX() < 0)
        x -= 1;
    if(nbCols%2 == 0 && position.getY() < 0)
        y -=1;

    return Position(x, y);

}

Map::Map()
{

}

Map::Map(unsigned int nbRobotMax, unsigned int nbRows, unsigned int nbCols) : nbRobotMax(nbRobotMax), nbRows(nbRows), nbCols(nbCols)
{
    grid = new CellState*[nbRows];
    for(unsigned int i = 0; i < nbRows; ++i){
        grid[i] = new CellState[nbCols];
    }
}

