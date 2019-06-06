#include "map.h"


/*
 * Couleur:
 *  - blanc = zone découverte
 *  - noir  = obstacle
 *  - gris  = non découverte
 *  - autre = robot
 */


void Map::init()
{
    QString c[12]={"darkred","darkblue","darkgreen","darkcyan","darkmagenta","darkgray","red","blue","green","cyan","magenta","yellow"};
    colors.insert(colors.begin(),c, c+12);
    //on prend le minimum pour être sur que ça entre
    dCell = std::min(width/nbC,height/nbL);

    setColumnCount(nbC);
    setRowCount(nbL);
    setShowGrid(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    verticalHeader()->hide();
    horizontalHeader()->hide();

    for(unsigned int i=0 ; i<nbL; ++i ){
        for (unsigned int j = 0 ; j<nbC;++j){
            QTableWidgetItem* a = new QTableWidgetItem ("");
            a->setFlags(Qt::NoItemFlags);
            a->setFlags(Qt::ItemIsEnabled);
            setItem(i,j,a);
            item(i,j)->setBackgroundColor("gray");
            if(i==0)
                setColumnWidth(j,(int)(dCell));
        }
        setRowHeight(i,dCell);
    }
    setFixedSize(width,height);
}


void Map::initRobot(int id,int x, int y,int heading)
{
    if(robots.find(id) == robots.end())
    {
        if(colors.size()==0){
            std::cerr<< "Il est impossible d'ajouter un nouveau Robot !\n"<< std::endl;
        }
        else{
            robots[id].x = x; robots[id].y = y;
            robots[id].heading = heading;
            robots[id].color = colors.at(colors.size()-1);
            colors.pop_back();
            item(robots[id].x,robots[id].y)->setBackgroundColor(robots[id].color);
        }

    }
}

//déplacement uniquement suivant une grille
void Map::move(int id,int d)
{
    if(robots[id].heading == 180 || robots[id].heading == 0 )   //déplacement suivant une ligne
    {
        for(int i = std::min(robots[id].y+d,robots[id].y); i<std::max(robots[id].y+d,robots[id].y); ++i)
            item(robots[id].x,i)->setBackgroundColor("white");
         robots[id].y += d;
    }
    else if(robots[id].heading == 90 || robots[id].heading == 270 ) // déplacement suivant une colonne
    {
        for(int i = std::min(robots[id].x+d,robots[id].x); i<std::max(robots[id].x+d,robots[id].x); ++i)
            item(i,robots[id].y)->setBackgroundColor("white");
        robots[id].x += d;
    }
    item(robots[id].x,robots[id].y)->setBackgroundColor(robots[id].color);

    return;
}



