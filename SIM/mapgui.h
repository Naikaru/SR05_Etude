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
class MapGui : QWidget
{

    Q_OBJECT

    Map map;

    //widgets
    QVBoxLayout * l_mapGui;

    QTableWidget* grid;

    unsigned int dimX = 10; //nb de lignes
    unsigned int dimY = 10; //nb de colonne
    unsigned int width = 300;     //largeur
    unsigned int height = 300;    //hauteur
    unsigned int dCell = std::min(width/dimY,height/dimX);     //hauteur et largeur d'une cellule
    bool isRunning = true;


    QColor colorList[3] {Qt::black, Qt::blue, Qt::red};
    std::map<int, QColor> robotColors;

    QHBoxLayout * l_mapModifier;
    QSpinBox *sb_selectX;
    QSpinBox *sb_selectY;

    QHBoxLayout * l_addRobot;
    QTableWidget * listRobotColor;
    QTableWidget* listRobotId;
    QPushButton * b_addRobot;


public:
    MapGui(QWidget *parent);
    void show()
    {
        QWidget::show();
    }
private slots:

    void addRobot(){

        listRobotColor->setColumnCount(listRobotColor->columnCount() + 1);
        listRobotColor->setFixedSize(listRobotColor->columnCount() * 40 ,40);


        listRobotId->setColumnCount(listRobotId->columnCount() + 1);
        listRobotId->setFixedSize(listRobotId->columnCount() * 40, 40);

    }

    void cellActivation(const QModelIndex& index);


};

#endif // MAPGUI_H
