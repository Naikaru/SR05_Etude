#include "mapgui.h"

MapGui::MapGui(QWidget * parent) : QWidget(parent)
{
    //general layout
    l_mapGui = new QVBoxLayout;

    grid = new QTableWidget(dimX, dimY, this);
    grid->setColumnCount(dimY);
    grid->setRowCount(dimX);
    grid->setShowGrid(true);
    grid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(unsigned int i=0 ; i<dimX; ++i ){
        for (unsigned int j = 0 ; j<dimY;++j){
            QTableWidgetItem* cellItem = new QTableWidgetItem ("");
            cellItem->setFlags(Qt::NoItemFlags);
            cellItem->setFlags(Qt::ItemIsEnabled);
            grid->setItem(i,j,cellItem);
            grid->item(i,j)->setBackgroundColor("grey");
            if(i==0)
                grid->setColumnWidth(j,(int)(dCell));
        }
        grid->setRowHeight(i,dCell);
    }
    grid->setFixedSize(width,height);
    grid->horizontalHeader()->setVisible(false);
    grid->verticalHeader()->setVisible(false);
    grid->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grid->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grid->horizontalHeader()->hide();
    grid->verticalHeader()->hide();
    //connection for cellModification
    connect(grid, SIGNAL(clicked(const QModelIndex&)), this, SLOT(cellActivation(const QModelIndex&)));

    l_mapGui->addWidget(grid);
    l_mapModifier = new QHBoxLayout;

    sb_selectX = new QSpinBox(this);
    sb_selectX->setRange(0,500);
    sb_selectX->setValue(10);
    l_mapModifier->addWidget(sb_selectX);

    sb_selectY = new QSpinBox(this);
    sb_selectY->setRange(0,500);
    sb_selectY->setValue(10);
    l_mapModifier->addWidget(sb_selectY);
    l_mapGui->addLayout(l_mapModifier);


    listRobotColor = new QTableWidget(1, 1, this);
    l_mapGui->addWidget(listRobotColor);
    listRobotColor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listRobotColor->setFixedSize(1*40, 40);
    listRobotColor->horizontalHeader()->setVisible(false);
    listRobotColor->verticalHeader()->setVisible(false);
    listRobotColor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listRobotColor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listRobotColor->horizontalHeader()->setDefaultSectionSize(40);
    listRobotColor->verticalHeader()->setDefaultSectionSize(40);

    listRobotId = new QTableWidget(1, 1, this);
    l_mapGui->addWidget(listRobotId);
    listRobotId->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listRobotId->setFixedSize(1*40, 40);
    listRobotId->horizontalHeader()->setVisible(false);
    listRobotId->verticalHeader()->setVisible(false);
    listRobotId->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listRobotId->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listRobotId->horizontalHeader()->setDefaultSectionSize(40);
    listRobotId->verticalHeader()->setDefaultSectionSize(40);

    b_addRobot = new QPushButton("AddNewRobot", this);

    QObject::connect(b_addRobot, SIGNAL(clicked()), this, SLOT(addRobot()));

    l_mapGui->addWidget(b_addRobot);

    this->setLayout(l_mapGui);


}

void MapGui::cellActivation(const QModelIndex &index){
    if(isRunning)
    {

        grid->item(index.row(),index.column())->setSelected(false);
        if(grid->item(index.row(),index.column())->text() == "")
        {
            //map.changeState(index.row(), index.column(), full);
            qDebug() << "case : " << index.row() << index.column();
            grid->item(index.row(),index.column())->setText("");
            grid->item(index.row(),index.column())->setBackgroundColor("black");
            grid->item(index.row(),index.column())->setTextColor("white");

        }
        else
        {
            //map.changeState(index.row(), index.column(), empty);
            qDebug() << "case : " << index.row() << index.column();
            grid->item(index.row(),index.column())->setBackgroundColor("gray");
            grid->item(index.row(),index.column())->setTextColor("gray");
            grid->item(index.row(),index.column())->setText("");

        }
    }

}

