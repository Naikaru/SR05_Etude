#include "map.h"


Map::Map(unsigned int nbR, unsigned int w,unsigned int h,unsigned int c,unsigned int l, QWidget* parent ):
    QWidget(parent),width(w),height(h),nbC(c),nbL(l), nbRobots(nbR)
{
    for (unsigned int i = 0; i < nbRobots; i++) {
        nbActionsRobot[i] = 0;
    }
    map = new QTableWidget();
    init();

    legend = new QGridLayout();
    //QString col[4]={"background-color: white;","background-color: green;","background-color: gray;","background-color: black;"};
    QString col[4]={"","","",""};
    col[0] = QString("background-color: ") + QString(EXPLO)+QString(";");
    col[1] = QString("background-color: ") + QString(FRONT)+QString(";");
    col[2] = QString("background-color: ") + QString(UNEXP)+QString(";");
    col[3] = QString("background-color: ") + QString(OBSTA)+QString(";");

    QString labelLegend[4]={"exploré","frontière","inexploré","obstacle"};

    for(int i = 0;i<4;i++){
        QPushButton* b = new QPushButton();
        b->setFixedSize(10,10);
        b->setStyleSheet(col[i]);
        b->setCheckable(false);
        legend->addWidget(b,0,i*2);
        legend->addWidget(new QLabel(labelLegend[i]),0,i*2+1);
    }

    mainLayout = new QVBoxLayout();
    setLayout(mainLayout);
    mainLayout->addWidget(map);
    mainLayout->addLayout(legend);
}



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

    map->setColumnCount(nbC);
    map->setRowCount(nbL);
    map->setShowGrid(true);
    map->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    map->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    map->verticalHeader()->hide();
    map->horizontalHeader()->hide();

    for(unsigned int i=0 ; i<nbL; ++i ){
        for (unsigned int j = 0 ; j<nbC;++j){
            QTableWidgetItem* a = new QTableWidgetItem ("");
            a->setFlags(Qt::NoItemFlags);
            a->setFlags(Qt::ItemIsEnabled);
            map->setItem(i,j,a);
            map->item(i,j)->setBackgroundColor(UNEXP);
            if(i==0)
                map->setColumnWidth(j,(int)(dCell));
        }
        map->setRowHeight(i,dCell);
    }
    map->setFixedSize(width,height);
}


void Map::initRobot(int id,int x, int y,int heading)
{

    convert(&x,&y);
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
            map->item(robots[id].x,robots[id].y)->setBackgroundColor(robots[id].color);
            setFrontier(robots[id].x,robots[id].y);
        }
    }
}

//déplacement uniquement suivant une grille
void Map::move(int id,int d)
{
    if(robots.find(id) != robots.end())
    {

        switch (robots[id].heading){
            case 0:         //droite
                moveRight(id,d);
                break;
            case 90:        //haut
                moveTop(id,d);
                break;
            case 180:       //gauche
                moveLeft(id,d);
                break;
            case 270:       //bas
                moveBottom(id,d);
                break;
            default:
                return;
        }

        map->item(robots[id].x,robots[id].y)->setBackgroundColor(robots[id].color);
        setFrontier(robots[id].x,robots[id].y);
    }
    nbActionsRobot[id]++;

    return;
}


void Map::moveTop(int id,int d)
{
    for(int i = std::min(robots[id].x-d,robots[id].x); i<=std::max(robots[id].x-d,robots[id].x); ++i){
        map->item(i,robots[id].y)->setBackgroundColor(EXPLO);
        setFrontier(i,robots[id].y);
    }
    robots[id].x -= d;
}

void Map::moveBottom(int id,int d)
{
    for(int i = std::min(robots[id].x+d,robots[id].x); i<=std::max(robots[id].x+d,robots[id].x); ++i){
        map->item(i,robots[id].y)->setBackgroundColor(EXPLO);
        setFrontier(i,robots[id].y);
    }
    robots[id].x += d;
}

void Map::moveRight(int id,int d)
{
    for(int i = std::min(robots[id].y+d,robots[id].y); i<=std::max(robots[id].y+d,robots[id].y); ++i)
    {
        map->item(robots[id].x,i)->setBackgroundColor(EXPLO);
        setFrontier(robots[id].x,i);
    }
     robots[id].y += d;
}

void Map::moveLeft(int id,int d)
{
    for(int i = std::min(robots[id].y-d,robots[id].y); i<=std::max(robots[id].y-d,robots[id].y); ++i)
    {
        map->item(robots[id].x,i)->setBackgroundColor(EXPLO);
        setFrontier(robots[id].x,i);
    }
     robots[id].y -= d;
}


//cette méthode met met à jour les frontières autour d'une position
// par laquelle le robot est censée être passé
void Map::setFrontier(int x,int y)
{
    if(x >0){
        if(map->item(x-1,y)->backgroundColor() == QColor(UNEXP))
            map->item(x-1,y)->setBackgroundColor(FRONT);
    }
    if(x < nbL-1){
        if(map->item(x+1,y)->backgroundColor() == QColor(UNEXP))
            map->item(x+1,y)->setBackgroundColor(FRONT);
    }
    if(y > 0){
        if(map->item(x,y-1)->backgroundColor() == QColor(UNEXP))
            map->item(x,y-1)->setBackgroundColor(FRONT);
    }
    if(y < nbC-1){
        if(map->item(x,y+1)->backgroundColor() == QColor(UNEXP))
            map->item(x,y+1)->setBackgroundColor(FRONT);
    }
}

void Map::setObstacle(int x, int y) {
    x = x % nbL;
    y = y % nbC;
    if (x < 0)
        x += nbL;
    if (y < 0)
        y += nbC;
    // Mise à jour de la carte avec les bons indices
    // qDebug() << "setobstacle, x=" << x << "y=" << y;
    map->item(x,y)->setBackgroundColor(OBSTA);
}

void Map::convert(int* x, int* y)
{
    int tmp =*y;
    *y = *x;
    *x = (nbL-1-tmp);
}


// Buffer functions

void Map::applyAction(int r, QStringList action){
//    qDebug() << "applyAction";
    unsigned int numAction = action[0].toUInt();
    QString movement = action[1];
    QString destination = action[2];
    int realDestination = destination.split(",").first().toInt();
    int expectedDestination = destination.split(",").last().toInt();
    if (movement == "move") {
        // qDebug() << "move, realDestination=" << realDestination << "expected=" << expectedDestination;
        move(r, realDestination);
        if (realDestination != expectedDestination) {
            // qDebug() << "robot: x=" << robots[r].x << "y=" << robots[r].y;
            // ajout d'un obstacle car on a pas atteint la destination souhaitée
            if(robots[r].heading == 180 ) {
                // obstacle vers la gauche
                setObstacle(robots[r].x, robots[r].y - 1);
            } else if(robots[r].heading == 0) {
                // obstacle vers la droite
                setObstacle(robots[r].x, robots[r].y + 1);
            } else if (robots[r].heading == 90) {
                // obstacle au dessus
                setObstacle(robots[r].x + 1, robots[r].y);
            } else if (robots[r].heading == 270) {
                // obstacle en dessous
                setObstacle(robots[r].x - 1, robots[r].y);
            }
        }
    } else if (movement == "turn") {
        // qDebug() << "turn";
        turn(r, realDestination);
    }
    nbActionsRobot[r] = numAction;
    // qDebug() << "maj action" << nbActionsRobot[r];
}


void Map::applyBufferForRobot(unsigned int r, QVector<QStringList> buffer) {
    unsigned int nbActions = nbActionsRobot[r];
    // qDebug() << "robot number" << r;
    // qDebug() << "action number" << buffer.first()[0];

    QVector<QStringList>::iterator it = buffer.begin();
    while ((*it)[0].toUInt() < nbActions && it != buffer.end()) {
        // qDebug() << "action number" << (*it)[0];
        it++;
    }

    // qDebug() << "fin 1er while";

    // On est arrivé aux actions à appliquer pour le robot
    while (it != buffer.end()) {
        applyAction(r, (*it));
        it++;
    }
}

