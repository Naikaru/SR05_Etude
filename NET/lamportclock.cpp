#include "lamportclock.h"

// c and i = 0 by default
LamportClock::LamportClock(int i,unsigned int c):  id(i),clock(c)
{}

unsigned int LamportClock::incrAndGet() {
    return ++clock;
}


unsigned int LamportClock::updateAndGet(unsigned int foreignClock) {
    clock = (clock<foreignClock)?foreignClock:clock;
    return ++clock;
}

bool LamportClock::operator<(LamportClock const& b)
{
    if ( clock < b.clock)
        return true;
    else if ((clock == b.clock) && (id<b.id))
        return true;

    return false;
}

LamportClock& LamportClock::operator=(LamportClock const& b){
    clock = b.clock;
    id = b.id;
    return *this;
}
