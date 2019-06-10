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
    unsigned int m_xp, m_yp;         // coords of precedent cell in the list
    unsigned int m_cost;             // distance from initial cell
    unsigned int m_heuristique;      // total cost : cost + estimation of minimal number of cell to reach the end
public:
    Cellule(unsigned int x, unsigned int y);
    Cellule(unsigned int x, unsigned int y, unsigned int cost, Cellule* pred, Cellule* last);
    ~Cellule() {}

    unsigned int get_x() { return m_x; }
    unsigned int get_y() { return m_y; }
    unsigned int get_xp() { return m_xp; }
    unsigned int get_yp() { return m_yp; }
    unsigned int get_cost() { return m_heuristique; }
    unsigned int get_heuristique() { return m_heuristique; }
};

int estimation_heuristique(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void add_cell(std::list<Cellule*>& alist, Cellule* cell);
void remove_cell(std::list<Cellule*>& alist, Cellule* cell);
Cellule* lookfor_cell(std::list<Cellule*> alist, unsigned int x, unsigned int y);
std::list<Cellule*>& add_neighbours(std::list<Cellule*>& openList, std::list<Cellule*>& closedList, Cellule* cell, Cellule* last,
                    Map* map);
std::list<Cellule*> aStar(std::list<Cellule*> closedList, Map* map, Cellule* begin, Cellule* frontier);

#endif // CELLULE_H
