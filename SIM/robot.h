#ifndef ROBOT_H
#define ROBOT_H
#include "position.h"

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
};

#endif // ROBOT_H
