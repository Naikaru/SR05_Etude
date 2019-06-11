#include "astar.h"
#include "map.h"

int estimation_heuristique(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    return (fabs(x1 - x2) + fabs(y1 - y2));
}

Cellule::Cellule(unsigned int x, unsigned int y): m_x(x), m_y(y) {}

Cellule::Cellule(unsigned int x, unsigned int y, unsigned int cost, Cellule *pred, Cellule *last):
    m_x(x),
    m_y(y),
    m_xp(pred->m_x),
    m_yp(pred->m_y)
{
  m_cost = pred->m_cost + cost;
  m_heuristique = m_cost + estimation_heuristique(x, y, last->m_x, last->m_y);
}


AStar::AStar(Cellule* b, Cellule* e, unsigned int id, Map* m):
    begin(b),
    end(e),
    ident(id),
    map(m)
{}

AStar::~AStar() {
    for (std::list<Cellule*>::iterator it=openList.begin(); it!=openList.end(); ++it)
        delete (*it);
    for (std::list<Cellule*>::iterator it=closedList.begin(); it!=openList.end(); ++it)
        delete (*it);
}

void AStar::add_cell(Cellule* cell){
    // Insertion triée (croissante) de cell dans list selon l'heuristique
    // On fait ici le choix de placer, à heuristique égale, la nouvelle cellule en première

    if (openList.empty() == true){ // cell devient la tete de liste comme seul element
        openList.push_back(cell);
    }
    else if (cell->get_heuristique() <= openList.front()->get_heuristique()){ // insertion en tete
        openList.push_front(cell);
    }
    else{
        std::list<Cellule*>::iterator it = openList.begin();
        while(it != openList.end() && cell->get_heuristique() <= (*it)->get_heuristique()) {
            ++it;
        }
        openList.insert(it, cell);
    }
}


Cellule* AStar::lookfor_cell(std::list<Cellule*> alist, unsigned int x, unsigned int y){
    for(std::list<Cellule*>::iterator it=alist.begin(); it!=alist.end(); ++it){
        if((*it)->get_x() == x && (*it)->get_y() == y){
            return *it;
        }
    }
    return nullptr;
}

void AStar::add_neighbours(Cellule* cell){
    int x = cell->get_x();
    int y = cell->get_y();
    // Access to element (x,y)
    std::cout << "Recherche des voisins autour de (x,y) = (" << x << "," << y << ")" << std::endl;

    // right neighbour:
    if(x+1 < map->get_nbL() && (map->get_cell(x+1,y) == EXPLO ||
                                map->get_cell(x+1,y) == FRONT ||
                                map->get_cell(x+1,y) == map->get_color(ident))
            && (lookfor_cell(closedList, x+1, y) == NULL)
            && (lookfor_cell(openList, x+1, y) == NULL)){
        //printf("\nAjout droit");
        add_cell(new Cellule(x+1, y, 1, cell, begin));
    }

    // voisin gauche :
    if(x > 0 && (map->get_cell(x-1,y) == EXPLO ||
                 map->get_cell(x-1,y) == FRONT ||
                 map->get_cell(x-1,y) == map->get_color(ident))
            && (lookfor_cell(closedList, x-1, y) == NULL)
            && (lookfor_cell(openList, x-1, y) == NULL)){
        add_cell(new Cellule(x-1, y, 1, cell, begin));
    }

    // voisin haut :
    if(y+1 < map->get_nbC() && (map->get_cell(x,y+1) == EXPLO ||
                                map->get_cell(x,y+1) == FRONT ||
                                map->get_cell(x,y+1) == map->get_color(ident))
            && (lookfor_cell(closedList, x, y+1) == NULL)
            && (lookfor_cell(openList, x, y+1) == NULL)){
        add_cell(new Cellule(x, y+1, 1, cell, begin));
    }

    // voisin bas :
    if(y > 0 && (map->get_cell(x,y-1) == EXPLO ||
                 map->get_cell(x,y-1) == FRONT ||
                 map->get_cell(x,y-1) == map->get_color(ident))
            && (lookfor_cell(closedList, x, y-1) == NULL)
            && (lookfor_cell(openList, x, y-1) == NULL)){
        add_cell(new Cellule(x, y-1, 1, cell, begin));
    }
    return;
}


void AStar::astar(){
    // A* starts from the end to reach the beginning
    // then you read the path from the beginning to reach the end

    std::list<Cellule*> openList;
    openList.push_front(end);
    Cellule* cell;

    while(openList.empty() == false && begin->get_xp() == -1 && begin->get_yp() == -1){
        // Récupération du plus petit element (donc la tete)
        cell = openList.front();
        closedList.push_front(cell);
        openList.pop_front();
        // Suppression de ce premier element, dit 'exploré'
        if((cell->get_x() == begin->get_x()) && (cell->get_y() == begin->get_y())){
            for (std::list<Cellule*>::iterator it=openList.begin(); it!=openList.end(); ++it)
                delete (*it);
            begin->set_cost(cell->get_cost());
            begin->set_heuristique(cell->get_heuristique());
            begin->set_xp(cell->get_xp());
            begin->set_yp(cell->get_yp());
            return;
            // Pareil que begin mais avec xp et yp mis à jour
        }
        else{
          // ajout des voisins
          add_neighbours(cell); //attention, modif du end par openList.back()??
        }
    }
    // End of loop : no path to reach end
    // std::cout << std::endl << "*** Pas de chemin possible ***" << std::endl;

    for (std::list<Cellule*>::iterator it=openList.begin(); it!=openList.end(); ++it)
        delete (*it);

    return;
}

int compute_angle(int heading, unsigned int x, unsigned int y, unsigned int xp, unsigned int yp) {
    // Next coord is another cell, so compulsory having (x != xp) OR (y != yp)
    unsigned int angle;
    if (yp - y == 1)
        angle = 0;
    if (yp - y == -1)
        angle = 180;
    if (xp - x == 1)
        angle = 270;
    if (xp - x == -1)
        angle = 90;
    return heading - angle;
}


QStringList AStar::get_path() {
    unsigned int x, y;
    int heading = map->get_heading(ident);
    unsigned int dist(0);
    QStringList path("");
    Cellule* cell = begin;

    while(cell->get_xp() != -1 && cell->get_yp() != -1) {
        x = cell->get_x();
        y = cell->get_y();
        cell = lookfor_cell(closedList, cell->get_xp(), cell->get_yp());
        dist += 1;
        int angle = compute_angle(heading, x, y, cell->get_x(), cell->get_y());
        if (angle != 0) {
            path << QString("turn:"+QString::number(angle)+"|");
            path << QString("move:"+QString::number(dist)+"|");
            dist = 0;
        }
        else if ((cell->get_xp() == -1 && cell->get_yp() == -1) ||
                 (compute_angle(heading, x, y, cell->get_x(), cell->get_y()) != 0))
                path << QString("move:"+QString::number(dist)+"|");
    }
    return path;
}
