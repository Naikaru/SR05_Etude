#ifndef LAMPORTCLOCK_H
#define LAMPORTCLOCK_H

class LamportClock{
    friend class Net;
private:
    unsigned int clock;
    int id;
public:
    LamportClock(int i = 0, unsigned int c = 0);
    unsigned int getClock(){ return clock; }
    void setClock(unsigned int c) {clock = c; }
    unsigned int getId(){ return id; }
    unsigned int incrAndGet();
    unsigned int updateAndGet(unsigned int foreignClock);
    bool operator<(LamportClock const& b);
    LamportClock& operator=(LamportClock const& b);
};

#endif // LAMPORTCLOCK_H
