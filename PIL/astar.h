#ifndef CELLULE_H
#define CELLULE_H

#include <math.h>
#include <list>
#include "map.h"

class Cellule
{
    friend class Pil;

private:
    int m_x, m_y;           // coords of current cellule
    int m_xp, m_yp;         // coords of precedent cell in the list
    int m_cost;             // distance from initial cell
    int m_heuristique;      // total cost : cost + estimation of minimal number of cell to reach the end
public:
    Cellule(int x, int y);
    Cellule(int x, int y, int cost, Cellule* pred, Cellule* last);
    ~Cellule() {}
};

int estimation_heuristique(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void add_cell(std::list<Cellule*>& alist, Cellule* cell);
void remove_cell(std::list<Cellule*>& alist, Cellule* cell);
void add_neighbours(std::list<Cellule*>& openList, std::list<Cellule*>& closedList, Cellule* cell, Cellule* last,
                    Map* map);
std::list<Cellule*> aStar(std::list<Cellule*> closedList, Map* map, Cellule* begin, Cellule* frontier);

#endif // CELLULE_H
