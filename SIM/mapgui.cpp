#include "mapgui.h"

QColor MapGui::cellFullColor = Qt::black;
QColor MapGui::cellEmptyColor = Qt::white;

MapGui::MapGui(QWidget * parent) : QWidget(parent)
{

    map = Map(5, dimX, dimY);
    map.initMap();
    //general layout
    l_mapGui = new QVBoxLayout;

    /*
    grid = new QTableWidget(dimY, dimX, this);
    grid->setColumnCount(dimX);
    grid->setRowCount(dimY);
    grid->setShowGrid(true);
    grid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(unsigned int i=0 ; i<dimY; ++i ){
        for (unsigned int j = 0 ; j<dimX;++j){
            QTableWidgetItem* cellItem = new QTableWidgetItem ("");
            cellItem->setFlags(Qt::NoItemFlags);
            cellItem->setFlags(Qt::ItemIsEnabled);
            grid->setItem(i,j,cellItem);
            grid->item(i,j)->setBackgroundColor("white");
            if(i==0)
                grid->setColumnWidth(j,(int)(dimX));
        }
        grid->setRowHeight(i,dimY);
    }
    grid->setFixedSize(40 * dimY,40 * dimX);
    grid->horizontalHeader()->setVisible(false);
    grid->verticalHeader()->setVisible(false);
    grid->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grid->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grid->horizontalHeader()->hide();
    grid->verticalHeader()->hide();
    */
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
    //connection for cellModification
    connect(grid, SIGNAL(clicked(const QModelIndex&)), this, SLOT(cellActivation(const QModelIndex&)));
    connect(grid, SIGNAL(itemSelectionChanged()), this, SLOT(cellSelection()));

    l_mapGui->addWidget(grid);
    l_mapModifier = new QHBoxLayout;

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

/* SLOTS */

void MapGui::cellActivation(const QModelIndex &index){
    if(isRunning)
    {
        qDebug() << "Modification case : " << index.row() << index.column();
        grid->item(index.row(),index.column())->setSelected(false);
        if(grid->item(index.row(),index.column())->text() == "")
        {
            map.changeState(index.column(), index.row(), CellState::full);

            grid->item(index.row(),index.column())->setText("_");
            grid->item(index.row(),index.column())->setBackgroundColor(cellFullColor);
            grid->item(index.row(),index.column())->setTextColor(cellFullColor);

        }
        else
        {            
            map.changeState(index.column(), index.row(), CellState::empty);
            grid->item(index.row(),index.column())->setBackgroundColor(cellEmptyColor);
            grid->item(index.row(),index.column())->setTextColor(cellEmptyColor);
            grid->item(index.row(),index.column())->setText("");


        }
        map.printMap();
    }

}

void MapGui::cellSelection()
{
    QList<QTableWidgetItem *> itemsSelected = grid->selectedItems();
    for(QTableWidgetItem *cell : itemsSelected){
        if(cell->backgroundColor() == cellEmptyColor){
            cell->setBackgroundColor(cellFullColor);
        }else{
            cell->setBackgroundColor(cellEmptyColor);
        }
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
}

void MapGui::synchronizeMaxW(int newMax)
{
    maxW = newMax;
}

void MapGui::synchronizeMaxH(int newMax)
{
    maxH = newMax;
}

