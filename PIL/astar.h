#ifndef CELLULE_H
#define CELLULE_H

#include <math.h>
#include <list>
#include "map.h"

class Cellule
{
    friend class Pil;

private:
    unsigned int m_x, m_y;           // coords of current cellule
    int m_xp, m_yp;         // coords of precedent cell in the list
    unsigned int m_cost;             // distance from initial cell
    unsigned int m_heuristique;      // total cost : cost + estimation of minimal number of cell to reach the end
public:
    Cellule(unsigned int x, unsigned int y);
    Cellule(unsigned int x, unsigned int y, unsigned int cost, Cellule* pred, Cellule* last);
    ~Cellule() {}

    unsigned int get_x() { return m_x; }
    unsigned int get_y() { return m_y; }
    int get_xp() { return m_xp; }
    int get_yp() { return m_yp; }
    unsigned int get_cost() { return m_cost; }
    unsigned int get_heuristique() { return m_heuristique; }

    void set_xp(unsigned int x) { m_xp = x; }
    void set_yp(unsigned int y) { m_yp = y; }
    void set_cost(unsigned int c) { m_cost = c; }
    void set_heuristique(unsigned int h) { m_heuristique = h; }
};

int estimation_heuristique(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

class AStar
{
private:
    std::list<Cellule*> openList;
    std::list<Cellule*> closedList;
    Cellule* begin;
    Cellule* end;
    Map* map;
    unsigned int ident;


public:
    AStar(){
        begin=nullptr;
        end=nullptr;
        map=nullptr;
        ident =0;
    }
    AStar(Cellule* b, Cellule* e, unsigned int id, Map* m);
    ~AStar();

    void add_neighbours(Cellule* cell);
    void add_cell(Cellule* cell);
    Cellule* lookfor_cell(std::list<Cellule*> alist, unsigned int x, unsigned int y);
    void astar();
    unsigned int get_heuristique(){ return begin->get_heuristique(); }
    //QString get_path() {}

    void init(Cellule* b, Cellule* e, unsigned int id, Map* m){
        begin=b;
        end=e;
        map=m;
        ident =id;
    }

};

#endif // CELLULE_H
