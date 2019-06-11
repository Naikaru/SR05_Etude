#ifndef ALGO_H
#define ALGO_H

#include "map.h"
#include "astar.h"

class Algo
{
private:
    QVector<QPair<Pos, float>> frontiers; // pos = frontier et unsigned int = distance euclidienne entre robot et la frontière
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

    // ######### version opti #########

    /*
     * Tri des frontières
     */
    void sortFrontier();

    float euclidDist(Pos a, Pos b);

    static bool cmp(const QPair<Pos, float>& a,const QPair<Pos, float>& b){
        return a.second < b.second;
    }
    
    int getFrontierMinPosOpti();

public:

    Algo(Map* m,unsigned int ident,unsigned int r):map(m),id(ident),nbRobot(r) {}

    /*
     *  Permet de savoir quelle frontière choisir
     */
    QStringList runMinPos();

    QStringList runMinPosOpti();



};




#endif // ALGO_H
