#include "map.h"


Map::Map(unsigned int w,unsigned int h,unsigned int c,unsigned int l, QWidget* parent ):
    QWidget(parent), width(w),height(h),nbC(c),nbL(l)
{
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
            std::cerr << "Initialisation robot " << id << std::endl;
            robots[id].x = x; robots[id].y = y;
            robots[id].heading = heading;
            robots[id].color = colors.at(colors.size()-1);
            colors.pop_back();
            map->item(robots[id].x,robots[id].y)->setBackgroundColor(robots[id].color);
            setFrontier(robots[id].x,robots[id].y);
        }
    }
    map->update();
    map->repaint();
}

//déplacement uniquement suivant une grille
void Map::move(int id,int d, bool obstacle)
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
        if (obstacle) {
            setObstacle(id);
        }
    }
    map->update();
    map->repaint();
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

void Map::setObstacle(int id) {
    int x = robots[id].x;
    int y = robots[id].y;
    switch (robots[id].heading) {
        case 0:         //droite
            y += 1;
            break;
        case 90:        //haut
            x += 1;
            break;
        case 180:       //gauche
            y -= 1;
            break;
        case 270:       //bas
            x -= 1;
            break;
        default:
            break;

    }
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
