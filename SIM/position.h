#ifndef POSITION_H
#define POSITION_H


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
};

#endif // POSITION_H
