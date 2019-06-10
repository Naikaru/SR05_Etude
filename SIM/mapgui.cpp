#include "mapgui.h"

QColor MapGui::cellFullColor = Qt::black;
QColor MapGui::cellEmptyColor = Qt::white;

MapGui::MapGui(QWidget * parent) : QWidget(parent)
{

    //general layout
    l_mapGui = new QVBoxLayout;

    grid = new QTableWidget(dimY, dimX, this);
    grid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    grid->setFixedSize(width, height);
    grid->horizontalHeader()->setVisible(false);
    grid->verticalHeader()->setVisible(false);
    grid->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grid->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grid->horizontalHeader()->setDefaultSectionSize(width/dimX);
    grid->verticalHeader()->setDefaultSectionSize(width/dimY);
    for(unsigned int i = 0; i < dimY; i++){
        for(unsigned int j = 0; j<dimX;j++){
            grid->setItem(i,j, new QTableWidgetItem(""));
            grid->item(i,j)->setBackgroundColor(cellEmptyColor);
            grid->item(i,j)->setTextColor(cellEmptyColor);
        }
    }
    setWalls();
    //connection for cellModification
    //connect(grid, SIGNAL(clicked(const QModelIndex&)), this, SLOT(cellActivation(const QModelIndex&)));
    connect(grid, SIGNAL(itemSelectionChanged()), this, SLOT(cellSelection()));

    l_mapGui->addWidget(grid);
    l_mapModifier = new QHBoxLayout;

    bt_run = new QPushButton("Run",this);
    l_mapGui->addWidget(bt_run);

    connect(bt_run, SIGNAL(clicked()), this, SLOT(run()));

    sb_selectX = new QSpinBox(this);
    sb_selectX->setRange(1,500);
    sb_selectX->setValue(10);
    l_mapModifier->addWidget(sb_selectX);

    QObject::connect(sb_selectX, SIGNAL(valueChanged(int)), this, SLOT(synchronizeDimX(int)));

    sb_selectY = new QSpinBox(this);
    sb_selectY->setRange(1,500);
    sb_selectY->setValue(10);
    l_mapModifier->addWidget(sb_selectY);

    QObject::connect(sb_selectY, SIGNAL(valueChanged(int)), this, SLOT(synchronizeDimY(int)));

    sb_selectMaxW = new QSpinBox(this);
    sb_selectMaxW->setRange(100,1000);
    sb_selectMaxW->setValue(500);
    l_mapModifier->addWidget(sb_selectMaxW);

    QObject::connect(sb_selectMaxW, SIGNAL(valueChanged(int)), this, SLOT(synchronizeMaxW(int)));

    sb_selectMaxH = new QSpinBox(this);
    sb_selectMaxH->setRange(100,1000);
    sb_selectMaxH->setValue(500);
    l_mapModifier->addWidget(sb_selectMaxH);

    QObject::connect(sb_selectMaxW, SIGNAL(valueChanged(int)), this, SLOT(synchronizeMaxH(int)));

    cb_selection = new QCheckBox("Ajout obstacles");
    l_mapModifier->addWidget(cb_selection);

    l_mapGui->addLayout(l_mapModifier);

    listRobotColor = new QTableWidget(1, 1, this);
    l_mapGui->addWidget(listRobotColor);
    listRobotColor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listRobotColor->setFixedSize(1*40, 40);
    listRobotColor->horizontalHeader()->setVisible(false);
    listRobotColor->verticalHeader()->setVisible(false);
    listRobotColor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listRobotColor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listRobotColor->horizontalHeader()->setDefaultSectionSize(dCell());
    listRobotColor->verticalHeader()->setDefaultSectionSize(dCell());

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

void MapGui::setWalls()
{
    for(unsigned int x = 0; x< this->dimX; ++x){
        for(unsigned int y=0; y < this->dimY; ++y){
            if(x == 0 || x == this->dimX-1 || y == 0 || y == this->dimY-1)
            {
                grid->item(y,x)->setText("_");
                grid->item(y,x)->setBackgroundColor(cellFullColor);
                grid->item(y,x)->setTextColor(cellFullColor);
            }
        }

    }
}
/* SLOTS */

void MapGui::run()
{
    map = Map(100, dimX, dimY);
    map.initMap();

    for(unsigned int x = 0; x< this->dimX; ++x){
        for(unsigned int y=0; y < this->dimY; ++y){
            if(grid->item(y,x)->text() == "")
            {
                map.changeState(x,y, CellState::empty);
            }
            else
            {
                map.changeState(x,y, CellState::full);
            }
        }
    }

    isRunning = true;

    bt_run->setEnabled(false);
    sb_selectMaxH->setEnabled(false);
    sb_selectMaxW->setEnabled(false);
    sb_selectX->setEnabled(false);
    sb_selectY->setEnabled(false);
    map.printMap();

}

void MapGui::cellActivation(const QModelIndex &index){

    qDebug() << "Modification case : " << index.row() << index.column();
    grid->item(index.row(),index.column())->setSelected(false);
    if(grid->item(index.row(),index.column())->text() == "")
    {
        grid->item(index.row(),index.column())->setText("_");
        grid->item(index.row(),index.column())->setBackgroundColor(cellFullColor);
        grid->item(index.row(),index.column())->setTextColor(cellFullColor);

    }
    else
    {
        grid->item(index.row(),index.column())->setBackgroundColor(cellEmptyColor);
        grid->item(index.row(),index.column())->setTextColor(cellEmptyColor);
        grid->item(index.row(),index.column())->setText("");

    }
}

void MapGui::cellSelection()
{
    QList<QTableWidgetItem *> itemsSelected = grid->selectedItems();
    for(QTableWidgetItem *cell : itemsSelected){
        if(cb_selection->isChecked()){
            if(isRunning)
            {
                map.changeState(cell->column(),cell->row(), CellState::full);
                map.printMap();
            }
            cell->setText("_");
            cell->setBackgroundColor(cellFullColor);
            cell->setTextColor(cellFullColor);
        }else{
            if(isRunning)
            {
                map.changeState(cell->column(),cell->row(), CellState::empty);
                map.printMap();
            }
            cell->setText("");
            cell->setBackgroundColor(cellEmptyColor);
            cell->setTextColor(cellEmptyColor);
        }
        cell->setSelected(false);
    }
}

void MapGui::synchronizeDimX(int newDim)
{
    qDebug() << "Nouvelle dimension :" << newDim;

    //on garde la même largeur
    //on diminue la hauteur
    if(dimY < newDim)
    {
        width = maxW;
        height = ((width) / ((float)newDim/(float)dimY));
        qDebug() << "w :" << width << " h:" << height;
    }else if(dimY > newDim){
        height = maxH;
        width = (float)(height) / ((float)dimY/(float)newDim);

        qDebug() << "w :" << width << " h:" << height;
    } else{
        width = maxW;
        height = maxH;
    }

    dimX = newDim;

    grid->setColumnCount(newDim);
    unsigned int rowW = height/dimY;
    unsigned int colW = width/dimX;
    grid->setFixedSize(colW * dimX , rowW * dimY);

    grid->horizontalHeader()->setDefaultSectionSize(rowW);
    grid->verticalHeader()->setDefaultSectionSize(colW);

    for(unsigned int i = 0; i < dimY; i++){
        for(unsigned int j = 0; j<dimX;j++){
            grid->setItem(i,j, new QTableWidgetItem(""));
            grid->item(i,j)->setBackgroundColor(MapGui::cellEmptyColor);
            grid->item(i,j)->setTextColor(MapGui::cellEmptyColor);
        }
    }
    setWalls();
}

void MapGui::synchronizeDimY(int newDim)
{
    qDebug() << "Nouvelle dimension " << newDim;

    //on garde la même largeur
    //on diminue la hauteur
    if(dimX > newDim)
    {
        width = maxW;
        height = ((float)(width) / ((float)dimX / (float)newDim));
        qDebug() << "w :" << width << " h:" << height;
    }else if(dimX < newDim){
        height = maxH;
        width = (float)(height) / ((float)newDim / (float)dimX);
        qDebug() << "w :" << width << " h:" << height;
    }else{
        width = maxW;
        height = maxH;
    }

    dimY = newDim;

    grid->setRowCount(newDim);
    unsigned int rowW = height/dimY;
    unsigned int colW = width/dimX;
    grid->setFixedSize(colW * dimX , rowW * dimY);

    grid->horizontalHeader()->setDefaultSectionSize(rowW);
    grid->verticalHeader()->setDefaultSectionSize(colW);


    for(unsigned int i = 0; i < dimY; i++){
        for(unsigned int j = 0; j<dimX;j++){
            grid->setItem(i,j, new QTableWidgetItem(""));
            grid->item(i,j)->setBackgroundColor(MapGui::cellEmptyColor);
            grid->item(i,j)->setTextColor(MapGui::cellEmptyColor);
        }
    }
    setWalls();

}

void MapGui::synchronizeMaxW(int newMax)
{
    maxW = newMax;
}

void MapGui::synchronizeMaxH(int newMax)
{
    maxH = newMax;
}

