#ifndef CELLULE_H
#define CELLULE_H

#include "map.h"

#include <math.h>
#include <limits>
#include <list>

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

int estimation_heuristique(int x1, int y1, int x2, int y2);

class AStar
{
private:
    std::list<Cellule*> openList;
    std::list<Cellule*> closedList;
    Cellule* begin;
    Cellule* end;
    unsigned int ident;
    Map* map;
    QStringList pathList;
    unsigned int m_heuristique;

    void clear();

public:
    AStar(){
        begin=nullptr;
        end=nullptr;
        map=nullptr;
        ident=0;
        m_heuristique=std::numeric_limits<int>::max();
    }

    AStar(Cellule* b, Cellule* e, unsigned int id, Map* m);
    ~AStar();

    void add_neighbours(Cellule* cell);
    void add_cell(Cellule* cell);
    Cellule* lookfor_cell(std::list<Cellule*> alist, unsigned int x, unsigned int y);

    void astar(); // launch A* algorithm
    void compute_pathList();
    QStringList get_path() { return pathList; }

    unsigned int get_heuristique(){ return m_heuristique; }
    Cellule* get_begin() { return begin; }
    Cellule* get_end() { return end; }
    std::list<Cellule*> get_closedList(){ return closedList ; }

    void init(Cellule* b, Cellule* e, unsigned int id, Map* m){
        begin=b;
        end=e;
        map=m;
        ident=id;
    }

};

int compute_angle(int heading, unsigned int x, unsigned int y, unsigned int xp, unsigned int yp);

#endif // CELLULE_H
