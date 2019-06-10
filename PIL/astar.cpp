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


void add_cell(std::list<Cellule*>& alist, Cellule* cell){
    // Insertion triée (croissante) de cell dans list selon l'heuristique
    // On fait ici le choix de placer, à heuristique égale, la nouvelle cellule en première

    if (alist.empty() == true){ // cell devient la tete de liste comme seul element
        alist.push_back(cell);
    }
    else if (cell->get_heuristique() <= alist.front()->get_heuristique()){ // insertion en tete
        alist.push_front(cell);
    }
    else{
        std::list<Cellule*>::iterator it = alist.begin();
        while(it != alist.end() && cell->get_heuristique() <= (*it)->get_heuristique()) {
            ++it;
        }
        alist.insert(it, cell);
    }
}

void remove_cell(std::list<Cellule*>& alist, Cellule* cell){
    for(std::list<Cellule*>::iterator it=alist.begin(); it!=alist.end(); ++it){
        if((*it)->get_x() == cell->get_x() && (*it)->get_y() == cell->get_y()){
            alist.erase(it);
        }
    }
    delete cell;
}

Cellule* lookfor_cell(std::list<Cellule*> alist, unsigned int x, unsigned int y){
    for(std::list<Cellule*>::iterator it=alist.begin(); it!=alist.end(); ++it){
        if((*it)->get_x() == x && (*it)->get_y() == y){
            return *it;
        }
    }
    return NULL;
}

std::list<Cellule*>& add_neighbours(std::list<Cellule*>& openList, std::list<Cellule*>& closedList, Cellule* cell, Cellule* last,
                    Map* map){
    int x = cell->get_x();
    int y = cell->get_y();
    // Access to element (x,y)

    // right neighbour:
    if(x+1 < map->get_nbL() && map->get_cell(x+1,y) != OBSTA
            && (lookfor_cell(closedList, x+1, y) == NULL)
            && (lookfor_cell(openList, x+1, y) == NULL)){
        //printf("\nAjout droit");
        Cellule newCell(x+1, y, 1, cell, last);
        add_cell(openList, &newCell);
    }

    // voisin gauche :
    if(x-1 >= 0 && map->get_cell(x-1,y) != OBSTA
            && (lookfor_cell(closedList, x-1, y) == NULL)
            && (lookfor_cell(openList, x-1, y) == NULL)){
        Cellule newCell(x-1, y, 1, cell, last);
        add_cell(openList, &newCell);
    }

    // voisin haut :
    if(y+1 < map->get_nbC() && map->get_cell(x,y+1) != OBSTA
            && (lookfor_cell(closedList, x, y+1) == NULL)
            && (lookfor_cell(openList, x, y+1) == NULL)){
        Cellule newCell(x, y+1, 1, cell, last);
        add_cell(openList, &newCell);
    }

    // voisin bas :
    if(y-1 >= 0 && map->get_cell(x,y-1) != OBSTA
            && (lookfor_cell(closedList, x, y-1) == NULL)
            && (lookfor_cell(openList, x, y-1) == NULL)){
        Cellule newCell(x, y-1, 1, cell, last);
        add_cell(openList, &newCell);
    }
    return openList;
}


Cellule* aStar(std::list<Cellule*>& closedList, Map* map, Cellule* begin, Cellule* frontier){
    // A* starts from the end to reach the beginning
    // then you read the path from the beginning to reach the end

    std::list<Cellule*> openList;
    openList.push_front(frontier);

    while(openList.empty() == false){
        // Récupération du plus petit element (donc la tete)
        Cellule* cell = openList.front();
        closedList.push_front(cell);
        openList.pop_front();
        // Suppression de ce premier element, dit 'exploré'
        if((cell->get_x() == begin->get_x()) && (cell->get_y() == begin->get_y())){
            return cell;
            // Pareil que begin mais avec xp et yp mis à jour
        }
        else{
          // ajout des voisins
          openList = add_neighbours(openList, closedList, cell, openList.back()/*end*/, map); //attention, modif du end par openList.back()??
        }
    }
    // End of loop : no path to reach end
    std::cout << std::endl << "*** Pas de chemin possible ***" << std::endl;
    return NULL;
}

/* LEFT TO DO
 * Surcharger l'opérateur d'affectation pour initialiser une cellule ?
 * Cellule* cell = openList.front();
 */
