#include "algo.h"

float euclidean_dist(QPair<int, int> a, QPair<int, int> b){
    return sqrt(pow((b.first - a.first), 2) + pow((b.second - a.second),2));
}

void Algo::findFrontier()
{
    frontiers.clear();
    for(int i =0; i<map->get_nbL();i++){
        for(int j =0; j<map->get_nbC();j++){
            if(map->get_cell(i,j) == FRONT){
                QPair<QPair<int,int>,float> p(QPair<int,int>(i,j), euclidean_dist(QPair<int, int>(map->robots[id].x, map->robots[id].y),
                                                                                  QPair<int, int>(i,j)));
                frontiers.push_back(p);
            }
        }
    }
}

/*
 * fonctionne si les indices des robots vont de 0 à nbRobot-1
 * Complexité : O(nbRobot * nbFrontière * (nbCase)^2 )
 */
void Algo::computeCost()
{
    cost.clear();
    cost.resize(nbRobot); // une ligne par robot
    int nbFrontier(frontiers.size());
    for(int i =0; i<nbRobot;i++)
    {
        cost[i].resize(nbFrontier);
        for(int j =0; j<nbFrontier;j++)
        {
            //cellule de départ = position du robot d'indice i
            Cellule* c1 = new Cellule(map->robots[i].x,map->robots[i].y);
            //cellule d'arrivée = une des frontières
            Cellule* c2 = new Cellule(frontiers[j].first.first,frontiers[j].first.second);
            //on initialise le AStar
            cost[i][j].init(c1,c2,i,map);
            //on calcule le plus court chemin pour y arriver ( = cost)
            cost[i][j].astar();
        }
    }
}


int Algo::getFrontierMinPos()
{
    QVector<int> classment(frontiers.size(),int(1));

    //pour chaque frontière on enregistre
    //le classement du robot par rapport aux autres
    // si = 1 -> il est le robot le + proche de la frontière
    // si = nbRobot -> il est le plus loin

    for(int j =0; j<frontiers.size();j++)
    {
        int costRobot(cost[id][j].get_heuristique()); //coût de notre robot sur cette frontière

        for(int i =0; i<nbRobot;i++)
        {     
            if(i != id && cost[i][j].get_heuristique()<costRobot ){ //si un autre robot à un coût inferieur on se classe plus loin
                classment[j]++;
            }
        }
    }
    int min = *std::min_element(classment.constBegin(), classment.constEnd());  //recherche du score minimum
    int front = classment.indexOf(min);     //regarde pour quelle frontier on a obtenu ce score minimum

    return front;
}


QStringList Algo::runMinPos()
{
    findFrontier();
    computeCost();
    int front = getFrontierMinPos();
    return cost[id][front].get_path();
}


// ################### Version optimisée ###################

/*
 * Toujours la même complexité mais plus de chance de s'arrêter avant !
 */


void Algo::sortFrontier(){
    std::sort(frontiers.begin(),frontiers.end(), Algo::cmp_frontiers);
}

/*
 * Calcul des coûts et de la frontière à choisir en même temps :
 *  comme ça dès qu'on trouve une frontière pour laquelle
 *  on est le robot le plus proche on la choisit
 */
int Algo::getFrontierMinPosOpti()
{
    cost.clear();
    cost.resize(nbRobot); // une ligne par robot
    int nbFrontier(frontiers.size());
    QVector<int> classment(frontiers.size(),int(1));


    //initialisation
    for(int i =0; i<nbRobot;i++)
        cost[i].resize(nbFrontier);

    //boucle sur les frontières
    for(int j =0; j<nbFrontier;j++)
    {
        //calcul d'abord de la frontière de robot courant
        Cellule* c1 = new Cellule(map->robots[id].x,map->robots[id].y);
        //cellule d'arrivée = une des frontières
        Cellule* c2 = new Cellule(frontiers[j].first.first,frontiers[j].first.second);
        //on initialise le AStar
        cost[id][j].init(c1,c2,id,map);
        //on calcule le plus court chemin pour y arriver ( = cost)
        cost[id][j].astar();

        int costRobot(cost[id][j].get_heuristique());

        //pour chaque frontière on calcul de AStar à chaque frontièreS
        for(int i =0; i<nbRobot;i++)
        {
            if( i != id){
                //cellule de départ = position du robot d'indice i
                Cellule* c1 = new Cellule(map->robots[i].x,map->robots[i].y);
                //cellule d'arrivée = une des frontières
                Cellule* c2 = new Cellule(frontiers[j].first.first,frontiers[j].first.second);
                //on initialise le AStar
                cost[i][j].init(c1,c2,i,map);
                //on calcule le plus court chemin pour y arriver ( = cost)
                cost[i][j].astar();

                if(i != id && cost[i][j].get_heuristique()<costRobot ){ //si un autre robot à un coût inferieur on se classe plus loin
                    classment[j]++;
                }
            }
        }
        if( classment[j] == 1)
            return j;
    }
    int min = *std::min_element(classment.constBegin(), classment.constEnd());  //recherche du score minimum
    int front = classment.indexOf(min);     //regarde pour quelle frontier on a obtenu ce score minimum

    return front;
}


QStringList Algo::runMinPosOpti()
{
    findFrontier();
    sortFrontier(); //premier trie par la distance euclidienne

    int front = getFrontierMinPosOpti();
    return cost[id][front].get_path();
}

QStringList Algo::runMinRobots() {
    QVector<QPair<int, float>> robots_dist(nbRobot);
    for(int i=0; i<nbRobot; ++i) {
        if (i != id)
            robots_dist[i] = qMakePair(i, euclidean_dist(QPair<int, int>(map->robots[id].x, map->robots[id].y),
                                                         QPair<int, int>(map->robots[i].x, map->robots[i].y)));
        else
            robots_dist[i] = qMakePair(id, std::numeric_limits<int>::max());
    }
    std::sort(robots_dist.begin(),robots_dist.end(), Algo::cmp_robots);
    //premier tri par la distance euclidienne

    // Position du robot courant
    Cellule* c1 = new Cellule(map->robots[id].x,map->robots[id].y);
    // Position du robot à atteindre
    Cellule* c2 = new Cellule(robots_dist.first().first,robots_dist.first().second);
    //on initialise le AStar
    AStar a(c1, c2, id, map);
    return a.get_path();
}
