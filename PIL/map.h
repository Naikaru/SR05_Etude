#ifndef MAP_H
#define MAP_H

#include <QtWidgets>
#include <QDebug>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <regex>
#include <fstream>

#define FRONT "green"
#define EXPLO "white"
#define UNEXP "gray"
#define OBSTA "black"


class Pos
{
public:

    int x;
    int y;

    Pos(){
        x=0;
        y=0;
    }

    Pos(const Pos& p): x(p.x), y(p.y)
    {}

    Pos& operator=(const Pos& p) { x = p.x; y = p.y; return *this; }

    Pos(int x,int y){
        this->x=x;
        this->x=y;
    }
};

typedef struct Robot
{
    int x;
    int y;
    int heading;
    QString color;

    Robot(){
        x=0;
        y=0;
        heading = 0;
        color ="";
    }

}Robot;


/*
 * La map est une variable partagée par tous les robots
 * Elle contiendra les infos de positions et d'orientions de tous les robots
 *     + les zones découvertes.
 */


class Map : public QWidget {
    Q_OBJECT

    friend class Pil;
    friend class Algo;

    unsigned int nbC;       //nb de colonne
    unsigned int nbL;       //nb de ligne
    unsigned int width;     //largeur
    unsigned int height;    //hauteur
    unsigned int dCell;     //hauteur et largeur d'une cellule

    std::vector<QString> colors;    //défini les couleurs que peuvent prendre les robots sur la map
    std::map<int, Robot> robots;    // défini pour chaque robot sa position (x, y) et son angle heading, dans cet ordre

    QTableWidget* map;
    QGridLayout* legend;
    QVBoxLayout* mainLayout;
public :

    Map(unsigned int w=500,unsigned int h=500,unsigned int c=50,unsigned int l=50, QWidget* parent = NULL);

    //initialise la map avec les dimensions du constructeur.
    void init();

    /*
     * initialise un robot avec un identifiant, une position et un angle
     * une couleur unique est affecté au robot
     * le nombre de couleur défini le nombre de robot possible
     */
    void initRobot(int id,int x, int y,int heading);

    /*
     * Permet le déplacement du robot d'une distance d case pour la simu
     * équivalent à la mnémonique move
     * le robot avance dans sa la direction de sa tête, si d est < 0 il recule
     * déplacement uniquement suivant une grille, pas de déplacement diagonaux
     */
    void move(int id,int d, bool setObstacle = false);

    Pos getRobotPosition(int id) { return Pos(robots[id].x, robots[id].y); }

    /*
     * Permet de tourner le robot d'un certain angle
     */
    void turn(int id,int angle)
    {
        if(robots.find(id) != robots.end())
        {
            if(angle <0)
                angle = 360 + angle;
            robots[id].heading = (robots[id].heading +angle)%360;
        }
    }

    unsigned int get_nbL() { return nbL; }

    unsigned int get_nbC() { return nbC; }

    QColor get_cell(unsigned int x, unsigned int y) { return map->item(x, y)->backgroundColor(); }
    QColor get_color(unsigned int id) { return robots[id].color; }
    int get_heading(unsigned int id) { return robots[id].heading; }

private:

    /*
     * x, y représente une case par laquelle le robot est passé
     * il faut donc mettre autour les frontières à jour
     */
    void setFrontier(int x,int y);
    /*
     * x, y représente une case qui doit être définie comme obstacle
     */
    void setObstacle(int id);
    /*
     *                                                               y ^
     * Converti les coordonnées pour un affichage comme dans un repère |__> x
     */
    void convert(int* x, int* y);

    void moveTop(int id,int d);
    void moveBottom(int id,int d);
    void moveRight(int id,int d);
    void moveLeft(int id,int d);

};





#endif // MAP_H
