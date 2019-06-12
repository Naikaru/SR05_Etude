#include "pil.h"


// ###################### Constructor #########################

Pil::Pil(int argc, char* argv[]): QWidget(), client(this, "PIL") {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->resize(500,350);
    main = new QVBoxLayout();
    nseq = 0;
    init = false;
    map = new Map();

    // Top bar with 2 buttons: Envoyer - Quitter
    button_area = new QHBoxLayout();
    quit = new QPushButton("Quitter",this);
    send = new QPushButton("Envoyer",this);

    button_area->addWidget(send);
    button_area->addWidget(quit);

    // 2nd part, reception, display the received message
    reception_area = new QGridLayout();
    reception_area->addWidget(new QLabel("Full message:",this),0,0);
    reception_fullmessage = new QLabel("-",this);
    reception_area->addWidget(reception_fullmessage,0,1,1,3);
    reception_area->addWidget(new QLabel("Message reçu:",this),1,0);
    reception_message_received = new QLabel("-",this);
    reception_area->addWidget(reception_message_received,1,1,1,3);
    reception_area->addWidget(new QLabel("Emetteur:",this),2,0);
    reception_sender = new QLabel("-",this);
    reception_area->addWidget(reception_sender,2,1);
    reception_area->addWidget(new QLabel("nseq:",this),2,2);
    reception_nseq= new QLabel("-",this);
    reception_area->addWidget(reception_nseq,2,3);
    reception_area->addWidget(new QLabel("Destinataire:", this),3,0);
    reception_dest = new QLabel("-", this);
    reception_area->addWidget(reception_dest,3,1);
    reception_box = new QGroupBox(tr("Réception"));
    reception_box->setStyleSheet("QGroupBox{font-weight: bold;}");
    reception_box->setLayout(reception_area);

    // Third part, parsing using mnemonic
    parse_area = new QVBoxLayout();
    parse_layout = new QGridLayout();
    parse_area->addLayout(parse_layout);
    parse = new QPushButton("Décoder",this);
    parse_layout->addWidget(new QLabel("Format: /mnemo~value",this),0,0,Qt::AlignCenter);
    parse_layout->addWidget(parse,1,0,Qt::AlignCenter);
    parse_layout->addWidget(new QLabel("mnemonic",this),0,1,Qt::AlignCenter);
    parse_layout->addWidget(new QLabel("value",this),0,2,Qt::AlignCenter);
    mnemonic = new QLineEdit(this);
    mnemonic->setFixedWidth(75);
    parse_value = new QLabel("-",this);
    parse_layout->addWidget(mnemonic, 1,1);
    parse_layout->addWidget(parse_value, 1,2,Qt::AlignCenter);
    parse_layout->setColumnStretch(2,50); // column 2 take 50% of the space
    parse_box = new QGroupBox(tr("Décodage"),this);
    parse_box->setStyleSheet("QGroupBox{font-weight: bold;}");
    parse_box->setLayout(parse_area);


    // Bottom, display a field to enter the message to send and a field to chose destinator
    send_area = new QVBoxLayout();
    send_box = new QGroupBox("Envoi",this);
    send_box->setStyleSheet("QGroupBox{font-weight: bold;}");

    info_area = new QHBoxLayout();
    info_area->addWidget(new QLabel("Ident:"));
    info_ident = new QLabel(QString::number(ident),this);
    info_area->addWidget(info_ident,20);
    info_area->addWidget(new QLabel("nseq:"));
    info_nseq = new  QLabel(QString::number(nseq),this);
    info_area->addWidget(info_nseq);
    send_area->addLayout(info_area);

    text = new QTextEdit("Bonjour",this);
    send_area->addWidget(text);

    dest_area = new QHBoxLayout();
    dest_area->addWidget(new QLabel("Destinataire :", this));
    dest = new QLineEdit("-1",this);
    dest_area->addWidget(dest);
    send_area->addLayout(dest_area);
    send_box->setLayout(send_area);

    // Build window
    setLayout(main);
    main->addLayout(button_area);
    main->addWidget(reception_box);
    main->addWidget(parse_box);
    main->addWidget(send_box);

    notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this); //fileno(stdin)

    initialization(argc,argv);
    std::cerr << "PIL sazejfsqdfh " << ident << std::endl;
    nbRobotsInitialized = nbRobot - 1;
    // qDebug() << "PIL ident = " << ident;

    algo = new Algo(map,ident,nbRobot);
    setWindowTitle(QString("PIL ")+ QString::number(ident) );

    QString tmp("127.0.0."); tmp+=QString::number(ident+1);
    client.connectToRobot(QHostAddress(tmp),4646);
    // initialisation du robot
    QString initAction("");
    initAction += mnemoInit +":" + QString::number(xInit)+","+QString::number(yInit)+","+QString::number(0);
    currentActionToDo << initAction;
    currentIndexOfAction =0;

    // todo : à virer après vérif
    //addInitInBufferAndSend();

    // to test
    // map->initRobot(ident,xInit,yInit,0);
//    map->move(0,5);
//    map->turn(0,90);
//    map->move(0,5);
//    map->turn(0,-90);
//    map->move(0,5);

    map->show();

    //par défaut on ne connait pas les robots qui sont à notre portée.
    nearRobot= QVector<int>(nbRobot,int(0));


//    Cellule* begin =  new Cellule(map->robots[ident].x,map->robots[ident].y);
//    std::cout << "ident : " << ident << "x : " << begin->get_x() << "y : " << begin->get_y() << std::endl;
//    Cellule* frontier = new Cellule(49,0);
//    frontier->set_cost(0);
//    frontier->set_heuristique(estimation_heuristique(begin->get_x(), begin->get_y(), 49, 0));
//    AStar astar(begin, frontier, ident, map);
//    astar.astar();

//    Cellule* path = astar.get_path();
//    if(path != NULL){
//        while((path != NULL) && (path->m_xp != -1) && (path->m_yp != -1)){
//            qDebug() << "Coordonnees ou avancer : (" << path->m_xp << "," << path->m_yp << ")";
//            path = astar.lookfor_cell(astar.get_closedList(), path->m_xp, path->m_yp);
//        }
//    }

//    QStringList path = astar.get_path();
//    for (QStringList::const_iterator it=path.constBegin(); it!=path.constEnd(); ++it)
//        std::cout << (*it).toLocal8Bit().constData() << std::endl;


    // Slots linked
    connect(quit, SIGNAL(clicked()), this, SLOT(close())); // Close window
    connect(send, SIGNAL(clicked()), this, SLOT(sendMessage())); // Send message
    connect(parse, SIGNAL(clicked()), this, SLOT(parseMessage())); // parse message
    connect(mnemonic, SIGNAL(returnPressed()), this, SLOT(parseMessage())); // parse message
    // Slot for notifier (reading)
    connect(notifier, SIGNAL(activated(int)), this, SLOT(readStdin()));
    connect(&client, SIGNAL(receivedMessage(Message)), this, SLOT(rmtMessage(Message)));

    while(!client.isHandshakeFinished())
    {
        //int a(0);
        QTest::qWait(50);
    }
    QTest::qWait(50);

    applyAction();
//        sendingThread = new SendingThread();
//        sendingThread->setParam(this);
//        sendingThread->start();
}


Pil::~Pil() {
    if (sendingThread != nullptr) {
        sendingThread->stopThread();
        delete sendingThread;
    }
}

//Message Format
// /mnemo~value/mnemo~value
// the first caracter is the separator
// ~ separe mnemo and value
QString Pil::parseMessage(QString mnemo, QString message){
    QString sepMnemVal('~');
    QStringList listMnemoVal = message.split(message[0],QString::SkipEmptyParts); // first caractere of the message = separator
    QStringList listMnemo;QStringList listVal;
    foreach (const QString &str, listMnemoVal) {
       QStringList tmp = str.split(sepMnemVal);
       if(tmp.size() == 2){
            listMnemo<<tmp[0];listVal<<tmp[1];
       }
       else
            return "";
    }
    int id = listMnemo.indexOf(mnemo);
    if(id != -1)
        return listVal[id];
    return "";
}

// format the message with mnemonic format
QString Pil::getFormatedMessage(QString payload, QString desti)
{
    QString ret("");

    //      apply    dest   envoyer à ts les robots
    //ret+= "PIL"+  "ROB" +  "AIR";// who

    ret+= defaultSep+ senderMnemo+ valueSep+info_ident->text(); // who

    // payload
    if(payload == "")
        ret+=defaultSep+payloadMnemo+valueSep+text->toPlainText();
    else
        ret+=defaultSep+payloadMnemo+valueSep+payload;

    ret+=defaultSep+nseqMnemo+valueSep+QString::number(nseq);   // nseq

    // dest
    if(desti == "")
        ret += defaultSep + destMnemo + valueSep + dest->text();
    else
         ret += defaultSep + destMnemo + valueSep + desti;  //for demand SC desti is him self

    return ret;
}

// Retrieve argument
Options Pil::getOption(QString arg){
    if(arg == "--ident") return OP_ident;
    else if(arg == "--nbNode") return OP_nbRobot;
    else if(arg == "--x") return OP_x;
    else if(arg == "--y") return OP_y;
    else if(arg == "--remote") return OP_remote;
    else return OP_wrong;
}

// this method takes the main argument to initialize some parameters like the ident...
void Pil::initialization(int argc, char* argv[])
{
    for(int i =1 ; i<argc;i++) // i = 1 because first arg in the path programme
    {
        QStringList arg= QString(argv[i]).split('=');
        if(arg.size() == 2){
            switch (getOption(arg[0]))
            {
                case OP_ident:
                    ident = arg[1].toUInt();
                    info_ident->setText(QString::number(ident));
                    break;
                case OP_nbRobot:
                    nbRobot=arg[1].toUInt();
                    break;
                case OP_x:
                    xInit=arg[1].toInt();
                    break;
                case OP_y:
                    yInit=arg[1].toInt();
                    break;
                default:
                    break;
            }
        }
    }
}

// --------------- SLOTS

// Click to send the message
void Pil::sendMessage() {
    info_nseq->setText(QString::number(++nseq));
    // sending message to NET
    //std::cout << getFormatedMessage().toStdString() << std::endl;
}

// Click to send the message
void Pil::sendBuffer(QString pay) {
    info_nseq->setText(QString::number(++nseq));
    // sending message to NET
    std::cout << getFormatedMessage(pay, "-1").toStdString() << std::endl;
}

// Slot to read from stdin, signal received because a message arrived on stdin
void Pil::readStdin() {
    notifier->setEnabled(false);
    // disconnect the socket and make another messages waiting
    std::string message;
    int c;

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    c = ungetc(std::getc(stdin), stdin);
    fcntl(STDIN_FILENO, F_SETFL, flags);
    while(c != EOF) {
        std::getline(std::cin, message);
        //si le message vient du robot ou de la simu
        if(message.substr(0,3) == "ROB")
        {

        }
        else if (parseMessage(appnetMnemo, QString::fromStdString(message)) == "") {
            QString payload = parseMessage(payloadMnemo, QString::fromStdString(message));
            if (payload.startsWith(bufferPayload)){
                applyBufferFromMessage(QString::fromStdString(message));
                reception_fullmessage->setText(QString::fromStdString(message));
                reception_message_received->setText(parseMessage(payloadMnemo, QString::fromStdString(message)));
                reception_nseq->setText(parseMessage(nseqMnemo, QString::fromStdString(message)));
                reception_sender->setText(parseMessage(senderMnemo, QString::fromStdString(message)));
                reception_dest->setText(parseMessage(destMnemo, QString::fromStdString(message)));
            }
        }

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        c = ungetc(std::getc(stdin), stdin);
        fcntl(STDIN_FILENO, F_SETFL, flags);
    }
    notifier->setEnabled(true);
}

// Slot to parse the message according to a specific mnemonic
void Pil::parseMessage() {
    QString message = reception_fullmessage->text();
    QString mnemo = mnemonic->text();
    parse_value->setText(parseMessage(mnemo,message));
}

void Pil::moveMovementReceived(unsigned int realDistance, unsigned int expectedDistance) {
    QString distance = QString(QString::number(realDistance) + "," + QString::number(expectedDistance));
    Robot rob = map->robots[ident];
    QString finalPosition = QString(
                                QString::number(rob.x) + "," +
                                QString::number(rob.y) + "," +
                                QString::number(rob.heading)
                            );
    addMovementInBuffer(nbActionsRobot[ident], mnemoMove, distance, finalPosition);
}

void Pil::turnMovementReceived(unsigned int t) {
    QString turn = QString(QString::number(t) + "," + QString::number(t));
    Robot rob = map->robots[ident];
    QString finalPosition = QString(
                                QString::number(rob.x) + "," +
                                QString::number(rob.y) + "," +
                                QString::number(rob.heading)
                            );
    addMovementInBuffer(nbActionsRobot[ident], mnemoTurn, turn, finalPosition);
}

void Pil::addMovementInBuffer(unsigned int nbAction, QString movement, QString distance, QString finalPosition) {
    if (buffer.length() >= MAX_BUFFER) {
        buffer.pop_front();
    }
    // nbAction = nb of actions of robot : compteur local à chaque robot
    // movement = action ("move", "turn")
        // movement format: move
    // distance = real distance, expected distance (to see if there is an obstacle)
        // distance format: 10,10
    // finalPosition: position after applying movements (x,y, heading)
        // finalPosition format: 1,1,90
    // format final: nbAction:move:10,10:1,2,90
    QStringList line;
    line.append(QString::number(nbAction));
    line.append(movement);
    line.append(distance);
    line.append(finalPosition);
    buffer.push_back(line);
}

void Pil::addInitInBufferAndSend() {
    addMovementInBuffer(1, mnemoInit, "0,0", QString(QString::number(xInit) + "," + QString::number(yInit) + ",0"));
    sendBufferToNet();
}

void Pil::sendBufferToNet() {
    QString payload = bufferPayload;
    QVector<QStringList> buf = getBuffer();
    // qDebug() << "ici";
    if (buf.size() > 0) {
        for (QVector<QStringList>::iterator it = buf.begin(); it != buf.end(); it++) {
            payload += "|" + (*it)[0] + ":" + (*it)[1] + ":" + (*it)[2] + ":" + (*it)[3];
        }
        // payload format:
        // @buffer|numAction:move:realMovement,expectedMovement:x_final, y_final, heading_final
        sendBuffer(payload);
    }
}


QVector<QStringList> Pil::parseBuffer(QString payload) {
    // message: @buffer|numAction:move:realMovement,expectedMovement:x_final, y_final, heading_final
    QVector<QStringList> vector;
    QStringList parse = payload.split("|", QString::SkipEmptyParts);

    for (QString action: parse) {
        QStringList line;
        if (action != bufferPayload) {
            QStringList param = action.split(":");
            for (QString p: param) {
                line.append(p);
            }
            vector.push_back(line);
        }
    }
    return vector;
}

void Pil::applyBufferFromMessage(QString message){
    QString identRobot = parseMessage(senderMnemo, message);
    QString payload = parseMessage(payloadMnemo, message);
//    qDebug() << "before, nbrobot=" << nbRobot << "ident=" << identRobot;
    if (identRobot.toInt() < nbRobot) {
//        qDebug() << "identRobot" << identRobot << " payload = " << payload;
        QVector<QStringList> buffer = parseBuffer(payload);
//        qDebug() << "length vector buffer" << buffer.length();
        QPair<unsigned int, unsigned int> myPos = map->getRobotPosition(ident);

        QString finalPosition = buffer.last()[3];
        QPair<unsigned int, unsigned int> otherPos(finalPosition.split(",")[0].toInt(), finalPosition.split(",")[1].toInt());
        if (!robotsTooFar(myPos, otherPos)) {
            reset_connected();
            applyBufferForRobot(identRobot.toUInt(), buffer);
        } else {
            std::cerr << "Robots too far, message not received\n";
        }
    }
}

void Pil::applyBufferForRobot(unsigned int r, QVector<QStringList> buffer) {
    if (nbActionsRobot.find(r) == nbActionsRobot.end()) {
        nbActionsRobot[r] = 0;
    }
    unsigned int nbActions = nbActionsRobot[r];
//     qDebug() << "PIL " << ident << ", apply robot number" << r;
//     qDebug() << "action number" << buffer.first()[0];

    QVector<QStringList>::iterator it = buffer.begin();
    while (it != buffer.end() && (*it)[0].toUInt() <= nbActions) {
        // qDebug() << "action number" << (*it)[0];
        it++;
    }

//     qDebug() << "fin 1er while";

    // On est arrivé aux actions à appliquer pour le robot
    while (it != buffer.end()) {
        applyActionFromBuffer(r, (*it));
        it++;
    }
}

void Pil::applyActionFromBuffer(int r, QStringList action){
//    qDebug() << "applyAction";
    unsigned int numAction = action[0].toUInt();
    QString movement = action[1];
    QString destination = action[2];
    int realDestination = destination.split(",").first().toInt();
    int expectedDestination = destination.split(",").last().toInt();
    QString finalPosition = action[3];
    int x_final = finalPosition.split(",")[0].toInt();
    int y_final = finalPosition.split(",")[1].toInt();
    int heading_final = finalPosition.split(",")[2].toInt();

//    qDebug() << "PIL " << ident << " nbactionrobot " << r << " = " << nbActionsRobot[r];
    // On vérifie que le robot ait été initialisé
    if (nbActionsRobot[r] == 0) {
        if (movement == mnemoInit) {
            map->initRobot(r, x_final, y_final, heading_final);
//            qDebug() << "caca";
            nbActionsRobot[r] = std::max(numAction, (unsigned int)1);
//            qDebug() << "prout";
            nbRobotsInitialized--;
            if (nbRobotsInitialized == 0) {
//                qDebug() << "avant thread";
                sendingThread = new SendingThread();
                sendingThread->setParam(this);
                sendingThread->start();
                if (is_connected()) {

//                    qDebug() << "avant algo";
//                    runAlgo();
//                    qDebug() << "après algo";
                }
                else
                    reach_nearestRob();
            }

            std::cerr << "Robot " << r << " initialisé";

        } else {
            //qDebug() << "Le robot " << r << " n'a jamais été initialisé";
        }
    } else if (movement == mnemoMove) {
        // qDebug() << "move, realDestination=" << realDestination << "expected=" << expectedDestination << "set obstacle" << realDestination != expectedDestination;
        map->move(r, realDestination, realDestination != expectedDestination);
        nbActionsRobot[r] = numAction;
    } else if (movement == mnemoTurn) {
        // qDebug() << "turn";
        map->turn(r, realDestination);
        nbActionsRobot[r] = numAction;
    }
//    qDebug() << "PIL " << ident << " nbactionrobot " << r << " = " << nbActionsRobot[r];
    // qDebug() << "maj action" << nbActionsRobot[r];
}


bool Pil::robotsTooFar(QPair<int, int> a, QPair<int, int> b) {
    return euclidean_dist(a, b) > DISTANCE_MAX;
}

void Pil::applyAction()
{
    //si on a fini les actions
    if(currentIndexOfAction == currentActionToDo.size()){
        if (is_connected())
            runAlgo();
        else
            reach_nearestRob();
    }
    //sinon on applique les actions en l'envoyant au robot ou à la simu
    else
    {
        Message m("PIL","ROB","LCH");
        m.setValue(mnemoRobotOrder,currentActionToDo[currentIndexOfAction]);
        client.send(m);
        currentIndexOfAction++;
    }

    return;
}


void Pil::runAlgo()
{
    if (nbRobotsInitialized == 0) {
        currentActionToDo = algo->runMinPosOpti();
    //    for (QStringList::const_iterator it=currentActionToDo.constBegin(); it!=currentActionToDo.constEnd(); ++it) {
    //        //std::cout << (*it).toLocal8Bit().constData() << std::endl;
    //        QString message = (*it).mid(0,4);
    //        // std::cout << message.toStdString() << std::endl;
    //        if (message == "turn")
    //            map->turn(0,((*it).mid(5)).toInt());
    //        else if (message == "move")
    //            map->move(0,((*it).mid(5)).toUInt());
    //    }

        currentIndexOfAction = 0;


        //si il est vide -> plus de frontières -> on a fini l'exploration !
        if(!currentActionToDo.isEmpty())
            applyAction();
        //FIN FIN FIN
        else{
            QMessageBox::information(this,"INFO","FIN FIN FIN FIN !!!!");
        }
    }
}


void Pil::reach_nearestRob()
{
    currentActionToDo = algo->runMinRobots();
    currentIndexOfAction = 0;
    applyAction();
}

// Slot to handle the message from the robot OR the simu
void Pil::rmtMessage(Message mess){
    //reception d'une réponse de la simu
    QStringList tmp= currentActionToDo[currentIndexOfAction-1].split(":");
    QString action = tmp[0];
    QString order= mess.getValue(mnemoRobotAck);
    bool obs = false;
    std::vector<int> val;
    //si c'est un move
    if(action == mnemoMove)
    {
        val = Message::getOrderValue(order);

        obs = tmp[1].toInt() != val[0];
        map->move(ident,val[0],obs);
        nbActionsRobot[ident]++;
        moveMovementReceived(val[0], tmp[1].toUInt());
        //QTest::qWait(tmp[1].toUInt()*100); //100ms par case

    }
    //sinon si c'est un turn
    else if (action == mnemoTurn)
    {

        val = Message::getOrderValue(order);

        obs = tmp[1].toInt() != val[0];

        map->turn(ident,val[0]);
        nbActionsRobot[ident]++;
        turnMovementReceived(val[0]);

    }
    else if (action == mnemoInit)
    {
        std::cerr << "PIL " << ident << " s'est initialise" << std::endl;
        val = Message::getOrderValue(order);
        map->initRobot(ident,val[0],val[1],val[2]);
        addInitInBufferAndSend();
    }

    if(obs) {
        if (is_connected())
            runAlgo();
        else
            reach_nearestRob();
    }
    else
        applyAction();
}


void SendingThread::run() {
    while (cont) {
        pil->decr_connected();
//        qDebug() << "PIL " << " envoi periodique",
        pil->sendBufferToNet();
        QTest::qWait(WAITING_TIME);
    }
}



