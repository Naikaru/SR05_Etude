#include "map.h"


//return true if a robot was added
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

bool Map::deleteRobot(int id)
{
    Robot robotToDelete = robots.at(id);
    this->changeState(robotToDelete.getPosition().getX(), robotToDelete.getPosition().getY(), CellState::empty);
    robots.erase(id);
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
    if( i >= nbCols || j >= nbRows){
        throw new QString("Out of Bounds");
    }
    grid[i][j] = newState;
}

CellState Map::getState(unsigned int i, unsigned int j) const
{
    if( i >= nbCols || j >= nbRows){
        throw new QString("Out of Bounds");
    }
    return grid[i][j];
}

int Map::move(unsigned int id, int d){

    qDebug() << "Move : going forward : " << d;
    unsigned int distanceTraveled = 0;

    Robot& robot = robots[id];
    Position position = robot.getPosition();
    std::vector<Position> cellTraveled;
    cellTraveled.push_back(position);

    //manage negative distances :
    if(d < 0)
    {
        turn(id, 180);

        //manage if did not turn
        d = - d;
    }
    double angleInRad = MathHelper::fromDegToRad(robot.getHeading());
    for(unsigned int i = 1; i <= d; ++i){
        Position destination;

        int x = position.getX() + i * cos(angleInRad);
        int y = position.getY() + i * sin(angleInRad);
        qDebug() << "distance " << i << "x : " << x << "y : " << y;
        destination.setX(x);
        destination.setY(y);


        Position gridCoordinates = destination;
        if(this->getState(gridCoordinates.getX(), gridCoordinates.getY()) == CellState::empty)
        {
            distanceTraveled++;
            cellTraveled.push_back(destination);
        } else {
            //obstacle = destination;

            distanceTraveled = i - 1;
            qDebug() << "obstacle rencontré en " << i;
            break;
        }
    }
    qDebug() << "Postition " << cellTraveled.back().getX() << cellTraveled.back().getY();

    robot.setPosition(cellTraveled.back());
    return distanceTraveled;
}

/*
 * This method allows the user to turn the robot, adding an angle to it original heading
 * return the updated robot's heading
 */

int Map::turn(unsigned int id, int angle){
    if(abs(angle > 360))
    {
        throw QString("Angle value is greater than 360");
    }
    int newAngle = robots[id].getHeading() + angle;
    newAngle = (newAngle < 0) ? 360 + newAngle : newAngle%360;

    robots[id].setHeading(newAngle);
    return angle;
}

/*
 *This method initialize the position and heading of a robot
 *
 */

bool Map::init(unsigned int id, int x, int y, int heading){

    Robot& robotToChange = robots[id];
    if(getState(x, y) != CellState::empty)
        return false;
    robotToChange.setPosition(Position(x,y));
    robotToChange.setHeading(heading);
    return true;
}

/*
 * Return the current position and heading of a robot
 */
const Robot& Map::curr(unsigned int id) const{
    return robots.at(id);
}

/*
 * Make the robot travel to the given position
 */

const Robot& Map::join(unsigned int id, int x, int y){
    Robot& robotToMove = robots[id];
    Position currentPosition = robotToMove.getPosition();
    qDebug() << "Trying to get to " << x << "," << y << " from " << currentPosition.getX() << "," << currentPosition.getY();
    float angleToJoin;
    float diffX = (float)x - currentPosition.getX();
    float diffY = (float) y - currentPosition.getY();
    if(diffX == 0 && diffY == 0)
        return robotToMove; //on retourne la position actuelle, on a pas bougé

    angleToJoin = MathHelper::fromRadToDeg((float)atan(abs(diffY)/(float)abs(diffX)));
    //if(diffX > 0 && diffY > 0) //1er cadrant
    //angleToJoin = angleToJoin; nothing to do

    if(diffX < 0 && diffY > 0) //2eme cadrant
        angleToJoin = 180 - angleToJoin;
    if(diffX < 0 && diffY < 0) //3eme cadrant
        angleToJoin = 180 + angleToJoin;
    if(diffX > 0 && diffY < 0) //4eme cadrant
        angleToJoin = 360 - angleToJoin;
    if(diffX == 0)
        angleToJoin = 90;
    if(diffY == 0)
        angleToJoin = 0;

    //we now have to make the robot turn to the given angle
    qDebug() << "Trying to get to angle : " << angleToJoin << "from angle " << robotToMove.getHeading();
    float turningAngle = angleToJoin - robotToMove.getHeading();
    if(abs(turningAngle) > 180)
        turningAngle = (turningAngle > 0) ? turningAngle - 360 : turningAngle + 360;
    turningAngle = MathHelper::roundToNearestInt(turningAngle);
    qDebug() << "Calling turn with angle = " << turningAngle;

    int updatedHeading = turn(id, turningAngle);
    qDebug() << "New angle is : " << robotToMove.getHeading();
    if(updatedHeading != turningAngle){ //the angle could not be reached
        return robotToMove;
    }

    int distanceToTravel = ceil(sqrt(pow(diffX,2) + pow(diffY,2))); //théorème de pythagore round to highest
    qDebug() << "exact distance : " << sqrt(pow(diffX,2) + pow(diffY,2));
    qDebug() << "Calling move with : " << distanceToTravel;
    move(id, distanceToTravel);

    return robotToMove;

}

Position Map::getCoordinatesFromPosition(const Position& position) const
{

    int x = position.getX() + nbCols / 2;
    int y = position.getY() + nbRows / 2;
    if(nbCols%2 == 0)
        x -= 1;
    if(nbRows%2 == 0)
        y -=1;

    return Position(x, y);

}

Position Map::getPositionFromCoordinates(const Position& coordinates) const
{

    int x = coordinates.getX() - nbCols / 2;
    int y = coordinates.getY() -nbRows / 2;
    if(nbCols%2 == 0)
        x += 1;
    if(nbRows%2 == 0)
        y +=1;

    return Position(x, y);

}

unsigned int Map::convert(unsigned int coord, unsigned int dim)
{
    return dim - coord -1;

}

Map::Map()
{

}

Map::Map(unsigned int nbRobotMax, unsigned int nbCols, unsigned int nbRows) : nbRobotMax(nbRobotMax), nbCols(nbCols), nbRows(nbRows)
{
    grid = new CellState*[nbCols];
    for(unsigned int i = 0; i < nbCols; ++i){
        grid[i] = new CellState[nbRows];
    }
}

const std::map<int, Robot> &Map::getRobots() const
{
    return this->robots;
}

void Map::initMap(){
    for(unsigned int x = 0; x< this->getNbCols(); ++x){
        for(unsigned int y=0; y < this->getNbRows(); ++y){

            this->changeState(x,y,CellState::empty);
        }
    }
}

void Map::printMap(){

    bool isRobot;
    Position robotPosition;
    std::map<int, Robot> robots = this->getRobots();
    std::cout << "Map : \n";
    for(unsigned int y = 0; y< this->getNbRows(); ++y){
        for(unsigned int x=0; x < this->getNbCols(); ++x){
            isRobot = false;

            for (std::map<int,Robot>::iterator it=robots.begin(); it!=robots.end(); ++it)
            {
                robotPosition = it->second.getPosition();
                if(robotPosition.getX() == x && robotPosition.getY() == y)
                {
                    isRobot = true;
                    std::cout << "R";
                }
            }
            if(isRobot == false)
            {

                std::cout << ((this->getState(x, convert(y, this->getNbRows())) == CellState::empty) ? "_" : "X");
            }
        }
        std::cout << std::endl;
    }
}
