#ifndef MAPGUI_H
#define MAPGUI_H

#include <QWidget>
#include "map.h"
#include "messagemanager.h"
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
#include <QLineEdit>
#include <QFile>
class MapGui : QWidget
{

    Q_OBJECT

    Map map;

    //widgets
    QVBoxLayout * l_mapGui;

    QTableWidget* grid;

    unsigned int dimX = 10; //nb de lignes
    unsigned int dimY = 10; //nb de colonne
    unsigned int maxW = 300;
    unsigned int maxH = 300;
    unsigned int width = 300;     //largeur
    unsigned int height = 300;    //hauteur
    unsigned int dCell() {return std::min(width/dimX,height/dimY);}     //hauteur et largeur d'une cellule
    bool isRunning = false;
    static QColor cellEmptyColor;
    static QColor cellFullColor;

    Qt::GlobalColor colorList[6] {Qt::blue, Qt::red, Qt::green, Qt::yellow, Qt::magenta, Qt::black};
    std::map<int, Qt::GlobalColor> robotColors;

    QHBoxLayout * l_run_coord;
    QPushButton * bt_run;
    QLabel * lb_coord;

    QHBoxLayout * l_mapModifier;
    QSpinBox *sb_selectX;
    QSpinBox *sb_selectY;
    QSpinBox *sb_selectMaxW;
    QSpinBox *sb_selectMaxH;
    QCheckBox * cb_selection;

    QHBoxLayout * l_addRobot;
    QTableWidget * listRobotColor;
    QPushButton * bt_addRobot;
    QLabel * lb_adress;
    QLineEdit * t_adress;

    //Error
    QLabel * lb_error;
    QTextEdit * t_display;

    //MessageManager
    QLabel * lb_message;
    MessageManager  * messageManager;

    //Config
    QPushButton * bt_configSave;
    QPushButton * bt_configLoad;
    QHBoxLayout * l_config;

    QPushButton * bt_test;

public:
    MapGui(QWidget *parent);
    void show()
    {
        QWidget::show();
    }


    void setWalls();

    void addMessageInDisplay(const QString& msg, bool warning = false);


    void updateRobotOnGrid(const Position& formerPosition, const Position& newPosition);

    Robot init(int id, int x, int y, int heading);
    int move(int id, int d);
    int turn(unsigned int id, int angle);
    Robot curr(unsigned int id);
    Robot join(unsigned int id, int x, int y );
    void addRobot(int x, int y, unsigned int heading, unsigned int id);


private :
    unsigned int convert(unsigned int coord, unsigned int dim);
    void convertBis(int* x, int* y);

public slots:
    void handleMessageFromRobot(const std::pair<int, Message> &msg);
    void handleMessageFromRobotTest();
private slots:

    void saveConfig();
    void loadConfig();
    void run();

    void cellActivation(const QModelIndex& index);
    void cellSelection();

    void synchronizeDimX(int newDim);
    void synchronizeDimY(int newDim);
    void synchronizeMaxW(int newMax);
    void synchronizeMaxH(int newMax);
    void initRobot();


};

#endif // MAPGUI_H
