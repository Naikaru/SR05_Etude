#include "robot.h"

const Position& Robot::getPosition() const
{
    return position;
}

void Robot::setPosition(const Position &value)
{
    position = value;
}

Robot::Robot() : heading(0), position(Position())
{

}


int Robot::getHeading() const
{
    return heading;
}

void Robot::setHeading(int value)
{
    heading = value;
}

