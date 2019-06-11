#ifndef ROBOT_H
#define ROBOT_H
#include "position.h"
#include <iostream>

class Robot
{
    Position position;
    int heading;

public:
    Robot();
    Robot(unsigned int heading, const Position& position) : heading(heading), position(position) {}

    int getHeading() const;
    void setHeading(int value);


    const Position& getPosition() const;
    void setPosition(const Position &value);

    friend std::ostream& operator<<(std::ostream& os, const Robot& robot){
        os << "Robot : " << "Position : " << robot.position << " Heading : " << robot.heading;
        return os;
    }

};



#endif // ROBOT_H
