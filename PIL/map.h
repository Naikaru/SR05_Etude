#ifndef MAP_H
#define MAP_H

#include <QtWidgets>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <regex>
#include <fstream>



typedef struct Pos{
    int x;
    int y;

    Pos(){
        x=0;
        y=0;
    }

    Pos(int x,int y){
        this->x=x;
        this->x=y;
    }
}Pos;

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


class Map : protected QTableWidget
{
    friend class pil;

    unsigned int nbC;       //nb de colonne
    unsigned int nbL;       //nb de ligne
    unsigned int width;     //largeur
    unsigned int height;    //hauteur
    unsigned int dCell;     //hauteur et largeur d'une cellule

    std::vector<QString> colors;    //défini les couleurs que peuvent prendre les robots sur la map
    std::map<int, Robot> robots;    // défini pour chaque robot sa position (x, y) et son angle heading, dans cet ordre

public :

    Map(unsigned int w=300,unsigned int h=300,unsigned int c=20,unsigned int l=20,QWidget* parent = NULL):
            width(w),height(h),nbC(c),nbL(l),QTableWidget(parent)
    {
        init();
    }

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
    virtual Pos move(int id,int d);

    /*
     * Surcharge de show pour afficher la map pour des tests
     * Necessaire car héritage privé
     */
    void show()
    {
        QTableWidget::show();
    }

    /*
     * Permet de tourner le robot d'un certain angle
     */
    void turn(int id,int angle)
    {
        if(angle >0)
            robots[id].heading = (robots[id].heading +angle)%360;
    }
};



/*
 * La map Simu est une map particulière.
 * ELle sera contenue dans la classe simulation et elle vérifiera si les
 * mouvement comme les move sont possibles.
 * Elle retournera la position réellement atteinte par le robot
 */



class MapSimu : public Map
{
    //rest à définir la class simu
    //friend class Simu;

public :


    MapSimu(unsigned int w=300,unsigned int h=300,unsigned int c=20,unsigned int l=20,QWidget* parent = NULL): Map(w,h,c,l,parent)
    {
        generateWorld();
    }

    /*
     * La méthode move doit être surchargé
     */
    virtual Pos move(int id,int d);


    /*
     * Cette méthode crée le monde dans
     */
    void generateWorld();
};






#endif // MAP_H
