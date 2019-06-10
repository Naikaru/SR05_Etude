#include "algo.h"


void Algo::findFrontier()
{
    frontiers.clear();
    for(int i =0; i<map->get_nbL();i++){
        for(int j =0; j<map->get_nbC();j++){
            if(map->get_cell(i,j) == FRONT){
                frontiers.push_back(QPair<unsigned int,unsigned int>(i,j));
            }
        }
    }
}

/*
 * fonctionne si les indices des robots vont de 0 à nbRobot-1
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
            Cellule* c1 = new Cellule(map->getRobotPosition(i).x,map->getRobotPosition(j).y);
            //cellule d'arrivée = une des frontières
            Cellule* c2 = new Cellule(frontiers[j].first,frontiers[j].second);
            //on initialise le AStar
            cost[i][j].init(c1,c2,id,map);
            //on calcule le plus court chemin pour y arriver ( = cost)
            cost[i][j].aStar();
        }
    }
}


Pos Algo::getFrontierMinPos()
{
    QVector<int> classment(frontiers.size(),int(0));

    //pour chaque frontière on enregistre
    //le classement du robot par rapport au autre
    // si = 1 -> il est le robot le + proche de la frontière
    // si = nbRobot -> il est le plus loin

    for(int j =0; j<frontiers.size();j++)
    {
        for(int i =0; i<nbRobot;i++)
        {
            int costRobot(cost[id][j]); //coût de notre robot sur cette frontière
            if(i != id && cost[i][j]<costRobot ){ //si un autre robot à un coût inferieur on se classe plus loin
                classment[j]++;
            }
        }
    }
    int min = *std::min_element(classment.constBegin(), classment.constEnd());
    int front = classment.indexOf(min);

    return frontiers[front];
}


Pos Algo::minPos()
{
    findFrontier();
    computeCost();

    return getFrontierMinPos();
}