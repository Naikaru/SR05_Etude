#include "map.h"



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

int Map::move(unsigned int id, int d){

    unsigned int distanceTraveled = 0;
    //Position obstacle;
    Robot& robot = robots[id];
    Position position = robot.getPosition();
    std::vector<Position> cellTraveled;
    cellTraveled.push_back(position);
    for(unsigned int i = 1; i <=d; ++i){
        Position destination;
        int x = position.getX() + floor(i*cos(robot.getHeading()) + 0.5);
        int y = position.getY() + floor(i*sin(robot.getHeading()) + 0.5);
        destination.setX(x);
        destination.setY(y);
        std::cout << "(" << destination.getX() << "," << destination.getY() << ")" << std::endl;

        if(this->getState(destination.getX(), destination.getY()) == empty)
        {
            cellTraveled.push_back(destination);
        } else {
            //obstacle = destination;
            distanceTraveled = i - 1;
            break;
        }
    }
    robot.setPosition(cellTraveled.back());
    std::cout << "last case (" << robot.getPosition().getX() << "," << robot.getPosition().getY() << ")" << std::endl;
    return distanceTraveled;
}

int Map::turn(unsigned int id, int d){
    robots[id].setHeading(robots[id].getHeading() + d);
    return d;
}

void Map::init(unsigned int id, int x, int y, int heading){
    Robot& robotToChange = robots[id];
    robotToChange.setPosition(Position(x,y));
    robotToChange.setHeading(heading);

}

Robot Map::curr(unsigned int id){
    return robots[id];
}

void Map::join(unsigned int id, int x, int y){
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

std::map<int, Robot> Map::getRobots() const
{
    return this->robots;
}

