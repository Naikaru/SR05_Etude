#include "algo.h"


void Algo::findFrontier()
{
    frontiers.clear();
    for(int i =0; i<map->get_nbL();i++){
        for(int j =0; j<map->get_nbC();j++){
            if(map->get_cell(i,j) == FRONT){
                QPair<Pos,float> p(Pos(i,j),euclidDist( Pos( map->getRobotPosition(i).x,map->getRobotPosition(i).y ),Pos(i,j)));
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
            Cellule* c1 = new Cellule(map->getRobotPosition(i).x,map->getRobotPosition(i).y);
            //cellule d'arrivée = une des frontières
            Cellule* c2 = new Cellule(frontiers[j].first.x,frontiers[j].first.y);
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


QString Algo::runMinPos()
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

float Algo::euclidDist(Pos a, Pos b){
    return sqrt( pow((b.x - a.x),2)+pow((b.y - a.y),2) );
}

void Algo::sortFrontier(){
    std::sort(frontiers.begin(),frontiers.end(), Algo::cmp);
}

/*
 * Calcul des coûts et de la frontière en même temps :
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
        Cellule* c1 = new Cellule(map->getRobotPosition(id).x,map->getRobotPosition(id).y);
        //cellule d'arrivée = une des frontières
        Cellule* c2 = new Cellule(frontiers[j].first.x,frontiers[j].first.y);
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
                Cellule* c1 = new Cellule(map->getRobotPosition(i).x,map->getRobotPosition(i).y);
                //cellule d'arrivée = une des frontières
                Cellule* c2 = new Cellule(frontiers[j].first.x,frontiers[j].first.y);
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


QString Algo::runMinPosOpti()
{
    findFrontier();
    sortFrontier(); //premier trie par la distance euclidienne

    int front = getFrontierMinPosOpti();
    return cost[id][front].get_path();
}
