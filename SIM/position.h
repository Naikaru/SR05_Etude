#ifndef POSITION_H
#define POSITION_H

#include <ostream>

class Position
{
    int x;
    int y;
public:
    Position() : x(0), y(0) {}
    Position(int x, int y) : x(x), y(y) {}

    int getY() const;
    void setY(int value);

    int getX() const;
    void setX(int value);
    friend std::ostream& operator<<(std::ostream& os, const Position& position){
        os << "X : " << position.getX() << " Y : " << position.getY();
        return os;
    }

};

#endif // POSITION_H
