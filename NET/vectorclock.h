#ifndef VECTORCLOCK_H
#define VECTORCLOCK_H

#include <iostream>
#include <string>
#include<map>

#include <QString>
#include <QStringList>

class VectorClock {
private:
    int ident;
    std::map<int, unsigned int> clock;
public:
    VectorClock(int id);
    std::map<int, unsigned int> getClock() { return clock; }
    void setClock(std::map<int, unsigned int> c) { clock = c; }
    std::map<int, unsigned int> incrAndGet(); // increment value associated to ident
    std::map<int, unsigned int> updateAndGet(std::map<int, unsigned int> foreignClock); // update clock using another clock
};

std::map<int, unsigned int> qstringToVector(QString vector);
QString vectorToQString(std::map<int, unsigned int> vector);

#endif // VECTORCLOCK_H
