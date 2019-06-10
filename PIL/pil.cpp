#include "pil.h"


// ###################### Constructor #########################

Pil::Pil(int argc, char* argv[]): QWidget() {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->resize(500,350);
    main = new QVBoxLayout();
    nseq = 0;
    init = false;

//    // TODO: changer pour récupérer depuis lancement appli
//    unsigned int nbRobots = 5;
//    map = new Map(nbRobots);
    map = new Map();

//    // to test
//    for (unsigned int i = 0; i < nbRobots; i++) {
//        map->initRobot(i, i, 0, 0);
//    }

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
    setWindowTitle(QString("PIL ")+ QString::number(ident) );

//    // to test
//    map->move(0,5);
//    map->turn(0,270);
//    map->move(0,5);
//    map->turn(0,90);
//    map->move(0,5);
    map->show();
    map->initRobot(ident,xInit,yInit,0);

    // Slots linked
    connect(quit, SIGNAL(clicked()), this, SLOT(close())); // Close window
    connect(send, SIGNAL(clicked()), this, SLOT(sendMessage())); // Send message
    connect(parse, SIGNAL(clicked()), this, SLOT(parseMessage())); // parse message
    connect(mnemonic, SIGNAL(returnPressed()), this, SLOT(parseMessage())); // parse message
    // Slot for notifier (reading)
    connect(notifier, SIGNAL(activated(int)), this, SLOT(readStdin()));
}


Pil::~Pil() {}

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
                    ident = arg[1].toInt();
                    info_ident->setText(QString::number(ident));
                    break;
                case OP_nbRobot:
                    nbRobot=arg[1].toInt();
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
    std::cout << getFormatedMessage().toStdString() << std::endl;
}

// Click to send the message
void Pil::sendBuffer(QString pay) {
    info_nseq->setText(QString::number(++nseq));
    // sending message to NET
    std::cout << getFormatedMessage(pay).toStdString() << std::endl;
}

// Slot to read from stdin, signal received because a message arrived on stdin
void Pil::readStdin() {
    notifier->setEnabled(false);
    // disconnect the socket and make another messages waiting
    std::string message;
    int c;
    do {
        std::getline(std::cin, message);
        if (parseMessage(appnetMnemo, QString::fromStdString(message)) == "") {
            QString payload = parseMessage(payloadMnemo, QString::fromStdString(message));
            if (payload.startsWith(bufferToApply)){
                applyBufferFromMessage(QString::fromStdString(message));
            }
            else {
                // Recepted message is for a BAS app
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

    } while (c != EOF);
    notifier->setEnabled(true);
}

// Slot to parse the message according to a specific mnemonic
void Pil::parseMessage() {
    QString message = reception_fullmessage->text();
    QString mnemo = mnemonic->text();
    parse_value->setText(parseMessage(mnemo,message));
}

void Pil::addMovementInBuffer(unsigned int nbAction, QString movement, QString distance) {
    if (buffer.length() >= MAX_BUFFER) {
        buffer.pop_front();
    }
    // nbAction = nb of actions of robot : compteur local à chaque robot
    // movement = action ("move", "turn")
        // movement format: move
    // distance = real distance, expected distance (to see if there is an obstacle)
        // distance format: 10,10
    // format final: nbAction:move:10,10
    QStringList line;
    line.append(QString::number(nbAction));
    line.append(movement);
    line.append(distance);
    buffer.push_back(line);
}

void Pil::sendBufferToNet() {
    QString payload = bufferToNetPayload;
    QVector<QStringList> buf = getBuffer();

    for (QVector<QStringList>::iterator it = buf.begin(); it != buf.end(); it++) {
        payload += "|" + (*it)[0] + ":" + (*it)[1] + ":" + (*it)[2];
    }
    // payload format:
    // @buffertonet|10:move:10,10|12:turn:90,90
    sendBuffer(payload);
}

QPair<unsigned int, unsigned int> Pil::chooseFrontier() {
    bool first = true;
    unsigned int minDist;
    Cellule* path;
    std::list<Cellule*> closedList;
    QPair<unsigned int, unsigned int> chosenFrontier;
    for(QVector<QPair<unsigned int, unsigned int>>::iterator it=frontiers.begin(); it!=frontiers.end(); ++it) {
        Cellule begin(map->robots[ident].x, map->robots[ident].y);
        Cellule frontier(it->first, it->second);
        frontier.m_cost = 0;
        frontier.m_heuristique = estimation_heuristique(begin.m_x, begin.m_y, it->first, it->second);
        frontier.m_xp = -1;
        frontier.m_yp = -1;

        // Liste containing element of the path
        path = aStar(closedList, map, &begin, &frontier);
        if(first) {
            minDist = closedList.size();
            first = false;
        } else {
            if (closedList.size() < minDist) {
                chosenFrontier = QPair<unsigned int, unsigned int>(it->first, it->second);
                minDist = closedList.size();
            }
        }
    }
    return QPair<unsigned int, unsigned int>(path->m_x, path->m_y);
    // Pas encore fonctionnel
}

QVector<QStringList> Pil::parseBuffer(QString payload) {
    // message: @buffer|1:move:10,10|turn:90,90
    QVector<QStringList> vector;
    QStringList parse = payload.split("|", QString::SkipEmptyParts);

    for (QString action: parse) {
        QStringList line;
        if (action != bufferToApply) {
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
    if (identRobot < nbRobot) {
    //    qDebug() << "identRobot" << identRobot << " payload = " << payload;
        QVector<QStringList> buffer = parseBuffer(payload);
    //    qDebug() << "length vector buffer" << buffer.length();
        map->applyBufferForRobot(identRobot.toUInt(), buffer);
    }
}
