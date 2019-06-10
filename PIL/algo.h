#ifndef ALGO_H
#define ALGO_H

#include "map.h"
#include "astar.h"

class Algo
{
private:
    QVector<QPair<unsigned int, unsigned int>> frontiers;
    QVector<QVector<AStar>> cost;
    Map* map;
    unsigned int id;
    unsigned int nbRobot;


    /*
     * Récupère toutes les frontières d'une map
     */
    void findFrontier();

    /*
     * Calcul la map des coûts en fonction de l'algorithme A*
     * Le coût = au nombre de case que comporte le chemin le + court pour accéder à la frontière
     */
    void computeCost();

    /*
     * Implémentation de l'algorithme minPos:
     * Une frontère est affectée à un robot si il est le robot le plus proche de la frontière
     */
    int getFrontierMinPos();

public:

    Algo(Map* m,unsigned int ident,unsigned int r):map(m),id(ident),nbRobot(r) {

    }


    /*
     *  Permet de savoir quelle frontière choisir
     */
    QString runMinPos();



};




#endif // ALGO_H
