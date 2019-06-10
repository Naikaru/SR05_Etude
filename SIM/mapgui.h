#ifndef MAPGUI_H
#define MAPGUI_H

#include <QWidget>
#include "map.h"
#include <QColor>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QTextEdit>
#include <QDateTime>
class MapGui : QWidget
{

    Q_OBJECT

    Map map;

    //widgets
    QVBoxLayout * l_mapGui;

    QTableWidget* grid;

    unsigned int dimX = 50; //nb de lignes
    unsigned int dimY = 50; //nb de colonne
    unsigned int maxW = 500;
    unsigned int maxH = 500;
    unsigned int width = 400;     //largeur
    unsigned int height = 400;    //hauteur
    unsigned int dCell() {return std::min(width/dimX,height/dimY);}     //hauteur et largeur d'une cellule
    bool isRunning = false;
    static QColor cellEmptyColor;
    static QColor cellFullColor;



    Qt::GlobalColor colorList[6] {Qt::black, Qt::blue, Qt::red, Qt::green, Qt::yellow,Qt::magenta};
    std::map<int, QColor> robotColors;

    QPushButton * bt_run;

    QHBoxLayout * l_mapModifier;
    QSpinBox *sb_selectX;
    QSpinBox *sb_selectY;
    QSpinBox *sb_selectMaxW;
    QSpinBox *sb_selectMaxH;
    QCheckBox * cb_selection;

    QHBoxLayout * l_addRobot;
    QTableWidget * listRobotColor;
    QPushButton * b_addRobot;

    QTextEdit * t_display;


public:
    MapGui(QWidget *parent);
    void show()
    {
        QWidget::show();
    }


    void setWalls();

    void addMessageInDisplay(const QString& msg);
    void updateRobotOnGrid(const Position& formerPosition, const Position& newPosition);

    int move(int id, int d);
    int turn(unsigned int id, int angle);
    const Robot& curr(unsigned int id);
    const Robot&join(unsigned int id, int x, int y );


private slots:

    void run();

    void addRobot(){

        listRobotColor->setColumnCount(listRobotColor->columnCount() + 1);
        listRobotColor->setFixedSize(listRobotColor->columnCount() * 40 ,40);



    }
    void cellActivation(const QModelIndex& index);
    void cellSelection();

    void synchronizeDimX(int newDim);
    void synchronizeDimY(int newDim);
    void synchronizeMaxW(int newMax);
    void synchronizeMaxH(int newMax);
    void initRobot();
};

#endif // MAPGUI_H
