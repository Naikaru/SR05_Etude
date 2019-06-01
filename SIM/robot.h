#ifndef ROBOT_H
#define ROBOT_H
#include "position.h"

class Robot
{
    Position position;
    int heading;

public:
    Robot();

        int getHeading() const;
        void setHeading(int value);


        Position getPosition() const;
        void setPosition(const Position &value);
};

#endif // ROBOT_H
