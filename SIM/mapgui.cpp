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

    l_run_coord = new QHBoxLayout;
    bt_run = new QPushButton("Run",this);
    lb_coord = new QLabel("X : 0 | Y : 0 ");
    l_run_coord->addWidget(bt_run);
    l_run_coord->addWidget(lb_coord);
    l_mapGui->addLayout(l_run_coord);

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
    sb_selectMaxW->setValue(300);
    l_mapModifier->addWidget(sb_selectMaxW);

    QObject::connect(sb_selectMaxW, SIGNAL(valueChanged(int)), this, SLOT(synchronizeMaxW(int)));

    sb_selectMaxH = new QSpinBox(this);
    sb_selectMaxH->setRange(100,1000);
    sb_selectMaxH->setValue(300);
    l_mapModifier->addWidget(sb_selectMaxH);

    QObject::connect(sb_selectMaxW, SIGNAL(valueChanged(int)), this, SLOT(synchronizeMaxH(int)));

    cb_selection = new QCheckBox("Ajout obstacles");
    l_mapModifier->addWidget(cb_selection);

    l_mapGui->addLayout(l_mapModifier);

    listRobotColor = new QTableWidget(1, 0, this);
    l_mapGui->addWidget(listRobotColor);
    listRobotColor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listRobotColor->setFixedSize(0*20, 0*20);
    listRobotColor->horizontalHeader()->setVisible(false);
    listRobotColor->verticalHeader()->setVisible(false);
    listRobotColor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listRobotColor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listRobotColor->horizontalHeader()->setDefaultSectionSize(20);
    listRobotColor->verticalHeader()->setDefaultSectionSize(20);

    robotColors = std::map<int, Qt::GlobalColor>();


    //AddRobot
    l_addRobot = new QHBoxLayout();

    bt_addRobot = new QPushButton("Connexion", this);
    bt_addRobot->setEnabled(false);
    lb_adress = new QLabel("Adresse TCP : ");
    t_adress = new QLineEdit();
    t_adress->setPlaceholderText("127.0.0.1");
    t_adress->setEnabled("false");

    l_addRobot->addWidget(bt_addRobot);
    l_addRobot->addWidget(lb_adress);
    l_addRobot->addWidget(t_adress);
    l_mapGui->addLayout(l_addRobot);

    QObject::connect(bt_addRobot, SIGNAL(clicked()), this, SLOT(initRobot()));



    //Messages
    lb_message = new QLabel("Gestion des messages");
    l_mapGui->addWidget(lb_message);
    messageManager = new MessageManager("ROB","PIL", "LCH", this);
    messageManager->setFixedSize(500, 50);
    QObject::connect(messageManager, SIGNAL(receivedMessageFromRobot(const std::pair<int,Message>&)), this, SLOT(handleMessageFromRobot(const std::pair<int,Message>&)));

    l_mapGui->addWidget(messageManager);

    //Erreurs
    lb_error = new QLabel("Gestion des erreurs");
    l_mapGui->addWidget(lb_error);

    t_display = new QTextEdit(this);
    t_display->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    t_display->setFocusPolicy(Qt::NoFocus);
    t_display->setFixedSize(500, 50);
    l_mapGui->addWidget(t_display);

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
void MapGui::addMessageInDisplay(const QString &msg)
{
    t_display->append(QDateTime::currentDateTime().time().toString() + QString(" : ") + msg);
}

void MapGui::updateRobotOnGrid(const Position &formerPosition, const Position &newPosition)
{
    unsigned int formerX = formerPosition.getX();
    unsigned int formerY = convert(formerPosition.getY(), dimY);

    unsigned int newX = newPosition.getX();
    unsigned int newY = convert(newPosition.getY(), dimY);

    if(grid->item(formerY,formerX)->text() == "R")
    {
        grid->item(newY, newX)->setText("R");
        grid->item(newY, newX)->setBackgroundColor(grid->item(formerY,formerX)->backgroundColor());
        grid->item(newY, newX)->setTextColor(grid->item(formerY,formerX)->textColor());

        grid->item(formerY,formerX)->setBackgroundColor(cellEmptyColor);
        grid->item(formerY,formerX)->setTextColor(cellEmptyColor);
        grid->item(newY, newX)->setText("");
    }

}

Robot MapGui::init(int id, int x, int y, int heading){
    if(map.getRobots().find(id) == map.getRobots().end())
    {
        addMessageInDisplay(QString("Init : Le robot d'id ") + QString::number(id) + QString(" n'est pas reconnu"));
        return Robot(heading,Position(x, y));
    }
    Position robotBeforeMove = map.getRobots().at(id).getPosition();
    //on considère que ca renvoie le bon
    map.init(id, x, y, heading);
    Position newPosition = map.getRobots().at(id).getPosition();
    updateRobotOnGrid(robotBeforeMove, newPosition);
    return map.getRobots().at(id);

}


int MapGui::move(int id, int d)
{
    if(map.getRobots().find(id) == map.getRobots().end())
    {
        addMessageInDisplay(QString("Move : Le robot d'id ") + QString::number(id) + QString(" n'est pas reconnu"));
        return 0;
    }

    Position robotBeforeMove = map.getRobots().at(id).getPosition();
    int distanceTravelled = map.move(id, d);
    Position newPosition = map.getRobots().at(id).getPosition();
    updateRobotOnGrid(robotBeforeMove, newPosition);
    return distanceTravelled;
}

int MapGui::turn(unsigned int id, int angle)
{
    if(map.getRobots().find(id) == map.getRobots().end())
    {
        addMessageInDisplay(QString("Turn : Le robot d'id ") + QString::number(id) + QString(" n'est pas reconnu"));
        return 0;
    }
    return map.turn(id, angle);
}

Robot MapGui::curr(unsigned int id)
{
    if(map.getRobots().find(id) == map.getRobots().end())
    {
        addMessageInDisplay(QString("Curr : Le robot d'id ") + QString::number(id) + QString(" n'est pas reconnu"));
        throw QString("Id non reconnu");
    }
    return map.curr(id);
}

Robot MapGui::join(unsigned int id, int x, int y)
{
    if(map.getRobots().find(id) == map.getRobots().end())
    {
        addMessageInDisplay(QString("Join : Le robot d'id ") + QString::number(id) + QString(" n'est pas reconnu"));
        throw QString("Id non reconnu");
    }

    Position robotBeforeMove = map.getRobots().at(id).getPosition();
    map.join(id, x, y);
    Position newPosition = map.getRobots().at(id).getPosition();
    updateRobotOnGrid(robotBeforeMove, newPosition);
    return map.getRobots().at(id);
}


void MapGui::handleMessageFromRobot(const std::pair<int, Message> &msg)
{

    Message ackMessage = messageManager->createMessage();

    QString order = msg.second.getValue(Message::mnemoRobotOrder);
    std::vector<int> parameters = std::vector<int>();
    if(order.startsWith("move")) {
        int distanceToTravel = Message::getOrderValue(order)[0];
        int distanceTravelled = this->move(msg.first, distanceToTravel);
        parameters.push_back(distanceTravelled);
        ackMessage.setValue(Message::mnemoRobotAck, Message::parseOrderValues(Message::mnemoAckMove, parameters));
    }
    else if(order.startsWith("turn")){
        int angleToTurn = Message::getOrderValue(order)[0];
        int angleTurned = this->turn(msg.first, angleToTurn);
        parameters.push_back(angleTurned);
        ackMessage.setValue(Message::mnemoRobotAck, Message::parseOrderValues(Message::mnemoAckTurn, parameters));
    }
    else if(order.startsWith("init")){
        std::vector<int> inputParameters = Message::getOrderValue(order);
        int x = inputParameters[0];
        int y = inputParameters[1];
        int heading = inputParameters[2];
        Robot newPosition = this->init(msg.first, x, y, heading);
        parameters.push_back(newPosition.getPosition().getX());
        parameters.push_back(newPosition.getPosition().getY());
        parameters.push_back(newPosition.getHeading());
        ackMessage.setValue(Message::mnemoRobotAck, Message::parseOrderValues(Message::mnemoAckInit, parameters));
    }
    else{
        //ordre non reconnu
        ackMessage.setValue(Message::mnemoRobotAck, Message::mnemoAckError + QString(":ignoredbadarg"));
    }

    messageManager->sendMessage(msg.first, ackMessage);

}
unsigned int MapGui::convert(unsigned int coord, unsigned int dim)
{
    return dim - coord -1;

}


void MapGui::initRobot()
{
    QString adress = t_adress->text();
    int x = 1, y = 4, heading = 3, id = 1;
    Position pos = Position(x, y);
    pos = map.getCoordinatesFromPosition(pos);
    if(map.getRobots().find(id) == map.getRobots().end()){ //robot do not exists
        if(map.addRobot(id, Robot(heading, pos))){ //robot was added
            robotColors[id] = colorList[robotColors.size()];
            grid->item(convert(pos.getY(), dimY), pos.getX())->setBackgroundColor(robotColors[id]);
            grid->item(convert(pos.getY(), dimY), pos.getX())->setTextColor(robotColors[id]);
            grid->item(convert(pos.getY(), dimY), pos.getX())->setText("R");

            listRobotColor->insertColumn(listRobotColor->columnCount());
            listRobotColor->setItem(0,listRobotColor->columnCount() - 1, new QTableWidgetItem(QString::number(id)));
            listRobotColor->item(0, listRobotColor->columnCount() - 1)->setTextColor("white");
            //listRobotColor->item(0, listRobotColor->columnCount() - 1)->setTextAlignment(Qt::AlignCenter);
            listRobotColor->item(0, listRobotColor->columnCount() - 1)->setBackgroundColor(robotColors[id]);
            listRobotColor->setFixedSize(listRobotColor->columnCount() * 20 ,20);

            //createSocket
            if(!messageManager->addRobotSocket(id))
                addMessageInDisplay(QString("The robot of id ") + QString::number(id) + QString(" could not open a tcp socket") );


        } else{
            addMessageInDisplay(QString("The robot of id ") + QString::number(id) + QString(" could not be added") );

        }
    }else{
        addMessageInDisplay(QString("The robot of id ") + QString::number(id) + QString("was already initialized") );
        //map.init(id, x, y, heading);
    }

}


/* SLOTS */

void MapGui::run()
{
    map = Map(100, dimX, dimY);
    for(unsigned int x = 0; x< this->dimX; ++x){
        for(unsigned int y=0; y < this->dimY; ++y){
            if(grid->item(y,x)->text() == "")
            {
                map.changeState(x,convert(y, dimY), CellState::empty);
            }
            else
            {
                map.changeState(x,convert(y, dimY), CellState::full);
            }
        }
    }

    isRunning = true;
    bt_run->setEnabled(false);
    sb_selectMaxH->setEnabled(false);
    sb_selectMaxW->setEnabled(false);
    sb_selectX->setEnabled(false);
    sb_selectY->setEnabled(false);
    bt_addRobot->setEnabled(true);
    t_adress->setEnabled(true);
}

void MapGui::cellActivation(const QModelIndex &index){

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
        lb_coord->setText("X: "+ QString::number(cell->column())+" | Y : " + QString::number(convert(cell->row(), dimX)));
        if(cell->text()=="R")
        {
           std::map<int,Robot> robots = map.getRobots();
                   for (std::map<int,Robot>::iterator it=robots.begin(); it!=robots.end(); ++it)
                   {
                       Position robotPosition = it->second.getPosition();
                       if(robotPosition.getX() == cell->column() && robotPosition.getY() == convert(cell->row(), dimX))
                       {
                          if(map.getRobots().size() == 1)
                          {
                              listRobotColor->removeColumn(0);
                              listRobotColor->setFixedSize(0*20, 0*20);
                          }
                          else{
                          listRobotColor->removeColumn(it->first);
                          listRobotColor->setFixedSize(listRobotColor->columnCount() * 20 ,20);
                          }
                          map.deleteRobot(it->first);
                          cell->setText("");
                          cell->setBackgroundColor(cellEmptyColor);
                          cell->setTextColor(cellEmptyColor);
                       }
                   }
        }
        else
        {
            if(cb_selection->isChecked()){

                if(isRunning)
                {
                    map.changeState(cell->column(),convert(cell->row(), dimY), CellState::full);
                }
                cell->setText("_");
                cell->setBackgroundColor(cellFullColor);
                cell->setTextColor(cellFullColor);
            }else{
                if(isRunning)
                {
                    map.changeState(cell->column(),convert(cell->row(), dimY), CellState::empty);
                }
                cell->setText("");
                cell->setBackgroundColor(cellEmptyColor);
                cell->setTextColor(cellEmptyColor);
            }
            cell->setSelected(false);
        }
    }
}

void MapGui::synchronizeDimX(int newDim)
{
    //qDebug() << "Nouvelle dimension :" << newDim;

    //on garde la même largeur
    //on diminue la hauteur
    if(dimY < newDim)
    {
        width = maxW;
        height = ((width) / ((float)newDim/(float)dimY));
        //qDebug() << "w :" << width << " h:" << height;
    }else if(dimY > newDim){
        height = maxH;
        width = (float)(height) / ((float)dimY/(float)newDim);

        //qDebug() << "w :" << width << " h:" << height;
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
    //qDebug() << "Nouvelle dimension " << newDim;

    //on garde la même largeur
    //on diminue la hauteur
    if(dimX > newDim)
    {
        width = maxW;
        height = ((float)(width) / ((float)dimX / (float)newDim));
        //qDebug() << "w :" << width << " h:" << height;
    }else if(dimX < newDim){
        height = maxH;
        width = (float)(height) / ((float)newDim / (float)dimX);
        //qDebug() << "w :" << width << " h:" << height;
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
