#include "net.h"


// ###################### Constructor #########################

Net::Net(int argc, char* argv[]): QWidget() {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->resize(300,250);
    nseq = 0;
    nbNode = 0;
    message_counter = 1;
    sc =false;

    main = new QVBoxLayout();

    // Top bar with 2 buttons: Quitter & Instantané
    button_area = new QVBoxLayout();
    quit = new QPushButton("Quitter", this);
    button_area->addWidget(quit);

    // 2nd part, reception, display the received message
    reception_area = new QGridLayout();
    reception_message_label = new QLabel("Message reçu:", this);
    reception_area->addWidget(reception_message_label,0,0);
    reception_message_received = new QLabel("-", this);
    reception_area->addWidget(reception_message_received,0,1,1,3);
    reception_area->addWidget(new QLabel("Emetteur:", this),1,0);
    reception_sender = new QLabel("-", this);
    reception_area->addWidget(reception_sender,1,1);
    reception_area->addWidget(new QLabel("nseq:", this),1,2);
    reception_nseq = new QLabel("-", this);
    reception_area->addWidget(reception_nseq,1,3);
    reception_area->addWidget(new QLabel("Destinataire:", this),2,0);
    reception_dest = new QLabel("-", this);
    reception_area->addWidget(reception_dest,2,1);
    reception_box = new QGroupBox(tr("Réception"),this);
    reception_box->setStyleSheet("QGroupBox{font-weight: bold;}");
    reception_box->setLayout(reception_area);


    // Bottom, display a field to enter the message to send and a field to chose time interval between messages if auto
    info_box = new QGroupBox(tr("Info"), this);
    info_box->setStyleSheet("QGroupBox{font-weight: bold;}");

    info_area = new QHBoxLayout();
    info_area->addWidget(new QLabel("Ident:", this));
    info_ident = new  QLabel(QString::number(ident), this);
    info_area->addWidget(info_ident,20);
    info_box->setLayout(info_area);

    initialization(argc, argv);

    // pour debug
    if(debug){
        refresh = new QPushButton("refresh",this);
        button_area->addWidget(refresh);
        connect(refresh, SIGNAL(clicked()), this, SLOT(debugSCDisplay()));

        debug_sc_area = new QTableWidget(this);
        debug_sc_area->setRowCount(nbNode);
        debug_sc_area->setColumnCount(3);
        debug_sc_area->setShowGrid(true);
        debug_sc_area->setHorizontalHeaderItem(0, new QTableWidgetItem("Site"));
        debug_sc_area->setHorizontalHeaderItem(1, new QTableWidgetItem("Horloge"));
        debug_sc_area->setHorizontalHeaderItem(2, new QTableWidgetItem("état"));

        for(unsigned int i=0 ; i<nbNode; ++i ){
            debug_sc_area->setItem(i,0,new QTableWidgetItem(""));
            debug_sc_area->setItem(i,1,new QTableWidgetItem(""));
            debug_sc_area->setItem(i,2,new QTableWidgetItem(""));
        }
        debugSCDisplay();
    }

    //fin pour debug

    // Bottom part, displaying clocks values
    lamportClock = new class LamportClock(ident);
    vectorClock = new class VectorClock(ident);    // ident should be init by initialization

    clock_area = new QVBoxLayout();
    clock_layout =  new QGridLayout();
    clock_area->addLayout(clock_layout);
    clock_layout->addWidget(new QLabel("Lamport Clock: ", this),0,0,Qt::AlignLeft);
    lamport_value = new QLabel(QString::number(lamportClock->getClock()), this);
    clock_layout->addWidget(lamport_value,0,1,Qt::AlignLeft);
    clock_layout->addWidget(new QLabel("Vector Clock: ", this),1,0,Qt::AlignLeft);
    vector_value = new QLabel(vectorToQString(vectorClock->getClock()), this);
    clock_layout->addWidget(vector_value,1,1, Qt::AlignLeft);
    clock_layout->setColumnStretch(2,50); // column 2 take 50% of the space
    clock_box = new QGroupBox(tr("Horloges"), this);
    clock_box->setStyleSheet("QGroupBox{font-weight: bold;}");
    clock_box->setLayout(clock_area);

    // Build window
    setLayout(main);
    main->addLayout(button_area);
    if(debug)
        main->addWidget(debug_sc_area);
    main->addWidget(reception_box);
    main->addWidget(clock_box);
    main->addWidget(info_box);

    notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this); //fileno(stdin)
    mutex = new QMutex();

    writing_thread = nullptr;
    reading_thread = nullptr;

    setWindowTitle(QString("NET ") + QString::number(ident));

    // Slots linked
    connect(quit, SIGNAL(clicked()), this, SLOT(close())); // Close window

    // Slot for notifier (reading)
    int flags = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, flags | O_NONBLOCK);
    connect(notifier, SIGNAL(activated(int)), this, SLOT(readStdin()));
}

// ###################### RDestructor #########################

Net::~Net() {

    delete notifier;
    delete mutex;

    if (writing_thread != nullptr && writing_thread->isRunning()) {
        writing_thread->continue_thread = false;
        delete writing_thread;
    }
    delete reading_thread;
    writing_thread = nullptr;
    reading_thread = nullptr;

    delete lamportClock;
    delete vectorClock;
}

// ###################### FUNCTIONS #########################

// Message Format
// /mnemo~value/mnemo~value
// the first caracter is the separator
// ~ separe mnemo and value
QString Net::parseMessage(QString mnemo, QString message){
    QString sepMnemVal('~');
    QStringList listMnemoVal = message.split(message[0],QString::SkipEmptyParts); // first caractere of the message = separator
    QStringList listMnemo;
    QStringList listVal;
    foreach (const QString &str, listMnemoVal) {
       QStringList tmp = str.split(sepMnemVal);
       if(tmp.size() == 2){
            listMnemo<<tmp[0];
            listVal<<tmp[1];
       }
       else
            return "";
    }
    int id = listMnemo.indexOf(mnemo);
    if(id != -1)
        return listVal[id];
    return "";
}

//payload and dest = "" by default
//allow to change the payload if needed
QString Net::getFormatedMessage(bool addAppnet, QString payload, QString dest) {
    QString ret("");
    ret += defaultSep + senderMnemo + valueSep + info_ident->text(); // who

    // payload
    if (payload != "")
        ret += defaultSep + payloadMnemo + valueSep + payload;
    else
        ret += defaultSep + payloadMnemo + valueSep + reception_message_received->text();

    ret += defaultSep + nseqMnemo + valueSep + QString::number(nseq); // nseq
    ret += defaultSep + msgCounterMnemo + valueSep + QString::number(message_counter++); // message_counter
    ret += defaultSep + lamportMnemo + valueSep + QString::number(lamportClock->getClock()); //hlglam
    ret += defaultSep + vectorMnemo + valueSep + vectorToQString(vectorClock->getClock()); //hlgvec

    // dest
    if(dest == "")
        ret += defaultSep + destMnemo + valueSep + reception_dest->text();
    else
        ret += defaultSep + destMnemo + valueSep + dest;

    if (addAppnet == true) {
        ret += defaultSep + appnetMnemo + valueSep + QString("net");                         // appnet
    }
    return ret;
}

QString Net::updateAppnet(QString message, bool addAppnet) {
    // construct the message to be transmitted to the BAS coming from another NET
    // appnet and clocks are removed
    QString ret("");
    ret += defaultSep + senderMnemo + valueSep + parseMessage(senderMnemo, message);                        // who
    ret += defaultSep + payloadMnemo + valueSep + parseMessage(payloadMnemo, message);      // payload
    ret += defaultSep + nseqMnemo + valueSep + parseMessage(nseqMnemo, message);            // nseq
    ret += defaultSep + destMnemo + valueSep + parseMessage(destMnemo, message);            // dest
    if (addAppnet == true) {
        ret += defaultSep + appnetMnemo + valueSep + QString("net");                         // appnet
        ret += defaultSep + lamportMnemo + valueSep + parseMessage(lamportMnemo, message);      // hlglam
        ret += defaultSep + vectorMnemo + valueSep + parseMessage(vectorMnemo, message);        // hlgvec
    }
    ret += defaultSep + msgCounterMnemo + valueSep + parseMessage(msgCounterMnemo, message);; // message counter for the net
    return ret;
}


// Retrieve arguments
Options Net::getOption(QString arg){
    if(arg == "--ident") return OP_ident;
    else if(arg == "--nbNode") return OP_nbNode;
    else return OP_wrong;
}

// this method take the main argument to initialize some parameters like the ident...
void Net::initialization(int argc, char* argv[])
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
                case OP_nbNode:
                    nbNode = arg[1].toUInt();;
                    break;
                default:
                    break;
            }
        }
    }
    //if 0 then not initializaded

    if (nbNode>0){
        waitingList.resize(nbNode);
    }
}

Actions  Net::getAction(QString arg){
    if(arg.startsWith(payloadSnapshot))
        return AC_snapshot_request_BAS;
    else if(arg.startsWith(payloadSnapshotForAll))
        return AC_snapshot_for_all;
    else if(arg.startsWith(payloadBasStateSnapshot))
        return AC_snapshot_bas_state;
    else if (arg.startsWith(payloadStateSnapshot))
        return AC_state_snapshot;
    else if (arg.startsWith(payloadSnapshotLoading))
        return AC_snapshot_loading;
    else if (arg.startsWith(payloadSnapshotStateToLoad))
        return AC_snapshot_state_to_load;
    else if (arg == demandSC)           // mean that the BAS send a demand of SC
        return AC_SC_demand;
    else if (arg == endSC)              // mean that the BAS stop is SC
        return AC_SC_end;
    else if (arg == acknowledgeSC)       // mean that the NET receive a response
        return AC_SC_acknowledge;
    else if (arg == requestSC)          // mean that the NET receive a response
        return AC_SC_request;
    else if (arg == freeSC)             // mean that the NET receive a response
        return AC_SC_free;
    else if (arg.startsWith(updateSC))
        return AC_SC_update;
    else
        return AC_default;
}

QString Net::getStrFromAction(Actions action){
    if(action == AC_snapshot_request_BAS)
        return payloadSnapshot;
    else if (action == AC_snapshot_for_all)
        return payloadSnapshotForAll;
    else if (action == AC_snapshot_bas_state)
        return payloadBasStateSnapshot;
    else if (action == AC_state_snapshot)
        return payloadStateSnapshot;
    else if (action == AC_snapshot_loading)
        return payloadSnapshotLoading;
    else if (action == AC_snapshot_state_to_load)
        return payloadSnapshotStateToLoad;
    else if (action == AC_SC_demand )           // mean that the BAS send a demand of SC
        return demandSC;
    else if (action == AC_SC_end  )              // mean that the BAS stop is SC
        return endSC;
    else if (action == AC_SC_acknowledge )       // mean that the NET receive a response
        return acknowledgeSC;
    else if (action == AC_SC_request)          // mean that the NET receive a response
        return requestSC;
    else if (action == AC_SC_free)             // mean that the NET receive a response
        return freeSC;
    else if (action == AC_SC_update)
        return updateSC;
    else
        return "defaut";
}


void Net::updateClock(QString message) {
    if (message == "" || parseMessage(senderMnemo, message).toUInt() == ident) {
        // msg comes from BAS : local update
        lamport_value->setText(QString::number(lamportClock->incrAndGet()));
        vector_value->setText(vectorToQString(vectorClock->incrAndGet()));
    } else {
        // msg comes from another BAS or NET : update with these clocks
        lamport_value->setText(QString::number(lamportClock->updateAndGet(parseMessage(lamportMnemo, message).toUInt())));
        vector_value->setText(vectorToQString(vectorClock->updateAndGet(qstringToVector(parseMessage(vectorMnemo, message)))));

    }
}

// Rebuilding a configuration from a snapsht
void Net::loadFile(QString message) {
    QString payload = parseMessage(payloadMnemo, message);
    std::string fileName = payload.toStdString();
    fileName = fileName.substr(QString::fromStdString(fileName).indexOf("|") + 1, QString::fromStdString(fileName).size());
    std::ifstream input("./snapshots/" + fileName);
    std::vector<std::string> line;

    if (!coherentSnapshot("./snapshots/" + fileName)) {
        std::cerr << "L'instantané n'est pas cohérent, pas de chargement" << std::endl;
        return;
    }

    QRegularExpression rx("[0-9]+");
    QRegularExpressionMatch match;

    QString shared_variable;

    int i = 0;
    std::string s;
    while (std::getline(input, s)) {
        line.push_back(s);
        if (line[i].find("nbNode") != std::string::npos) {
            match = rx.match(QString::fromStdString(line[i]));
            QString nbNodeFile = match.captured(0);
            if (nbNodeFile.toUInt() != nbNode) {
                std::cerr << "Le nombre de noeuds n'est pas égal à celui de la topologie actuelle" << std::endl;
                return;
            }
        } else if (line[i].find("shared") != std::string::npos) {
            QString lineQString = QString::fromStdString(line[i]);
            shared_variable = QString::fromStdString(line[i].substr(lineQString.indexOf("=") + 1, lineQString.size()));
        }
        i++;
    }

    int lineContainingMyState = 0;

    for (unsigned int i = 2; i < nbNode + 2; i++) {
        match = rx.match(QString::fromStdString(line[i]));
        if (match.captured(0).toUInt() == ident) {
            lineContainingMyState = i;
        } else {
            sendStateToLoad(QString::fromStdString(line[i]), shared_variable, true);
        }
    }
    sendStateToLoad(QString::fromStdString(line[lineContainingMyState]), shared_variable, false);
    loadState(QString::fromStdString("@a|" + line[lineContainingMyState]), true);
}

void Net::loadState(QString msg, bool myself) {
    QString payloadValue = msg;
    if (!myself) {
        payloadValue = parseMessage(payloadMnemo, msg);
    }
    // format payload: @state_load_snapshot|variable|(ident,hlgLamp)|(hlgVecto);
    QStringList value = payloadValue.split("|");

    QString hlgLamport = value[2];
    hlgLamport = QString::fromStdString(hlgLamport.toStdString().substr(1, hlgLamport.size() - 2));
    hlgLamport = hlgLamport.split(",")[1];
    lamportClock->setClock(hlgLamport.toUInt());
    lamport_value->setText(QString::number(lamportClock->getClock()));

    QString hlgVect = value[3];
    hlgVect = QString::fromStdString(hlgVect.toStdString().substr(1, hlgVect.size() - 3));
    vectorClock->setClock(qstringToVector(hlgVect));
    vector_value->setText(vectorToQString(vectorClock->getClock()));
}

void Net::payloadAction(QString message) {
    QString payload = parseMessage(payloadMnemo,message);
    switch (getAction(payload))
    {
        case AC_snapshot_request_BAS:   // reception d'une demande de snapshot par un BAS
            receiveSnapshotDemandFromBas(payload);
            break;
        case AC_snapshot_for_all:       // reception d'une demande de snapshot par un NET
            receiveSnapshotDemandForAll(message);
            break;
        case AC_state_snapshot:         // reception d'un état d'un NET donc save
            saveStateSnapshot(message);
            break;
        case AC_snapshot_loading:
            loadFile(message);
            break;
        case AC_snapshot_state_to_load:
            loadState(message);
            break;
        case AC_SC_demand:              // reception demande SC du BAS
            receiveDemandSC(message);
            break;
        case AC_SC_end:                 // reception fin SC du BAS
            receiveEndSC(message);
            break;
        case AC_SC_request:             // reception requete SC d'un autre NET
            receiveRequestSC(message);
            break;
        case AC_SC_acknowledge:         // reception accusé de reception d'un autre NET
            receiveAcknowledgeSC(message);
            break;
        case AC_SC_free:                // reception libération d'un NET
            receiveFreeSC(message);
            break;
        case AC_SC_update:
            receiveUpdateSC(message);
            break;
        case AC_default:                // reception message
            displayMessage(message);
            break;
        default:
            break;
    }
}

void Net::displayMessage(QString message)
{
    reception_message_received->setText(parseMessage(payloadMnemo, message));
    reception_dest->setText(parseMessage(destMnemo, message));
    reception_sender->setText(parseMessage(senderMnemo, message));
    reception_nseq->setText(parseMessage(nseqMnemo, message));
}


// ########## Snapshot ##########

void Net::receiveSnapshotDemandFromBas(QString payloadMsg) {
    sendSnapshotRequestForAll();

    // Save my state
     saveStateSnapshot("", true, payloadMsg);
}

void Net::receiveSnapshotDemandForAll(QString msg) {
    sendStateForSnapshot(msg);
}

void Net::saveStateSnapshot(QString message, bool myself, QString payloadMsg) {
    std::string fileName;
    std::string time_launch = datetime.toString(Qt::ISODate).toStdString();
    if (myself) {
        // lamport clock + 1 because clock increased before sending message
        // name of file without seconds
        fileName =  "./snapshots/" + time_launch.substr(0, time_launch.size() - 3) + "_" + std::to_string(lamportClock->getClock() + 1) + ".txt";
    } else {
        QRegularExpression rx("[0-9]+");
        QRegularExpressionMatch match = rx.match(parseMessage(payloadMnemo, message));
        QString number = match.captured(0);
        fileName = "./snapshots/" + time_launch.substr(0, time_launch.size() - 3) + "_" + number.toStdString() + ".txt";
    }

    std::ofstream fichier;
    fichier.open(fileName, std::ios::app);
    nbAnswersForSnapshot++;
    if (fichier) {
        if (myself) {
            fichier << "nbNode=" << nbNode << std::endl;
            std::string shared_variable = payloadMsg.toStdString().substr(payloadMsg.indexOf("|") + 1, payloadMsg.size());
            fichier << "shared variable=" << shared_variable << std::endl;
        }
        // L'état sauvegardé est de type:
        // num_site|(num_site, horloge_lamport)|(ident:horloge_vect ident:horloge_vec ...);
        std::string sender_ident;
        if (!myself) {
            sender_ident = parseMessage(senderMnemo, message).toStdString();
        }
        std::string write = "";
        // If saving the present state
        if (myself) {
            write += std::to_string(ident) + "|";
            write += "(" + std::to_string(ident) + "," + std::to_string(lamportClock->getClock()) + ")|";
            write += "(" + vectorToQString(vectorClock->getClock()).toStdString();
        } else {
            write += sender_ident + "|";
            write += "(" + sender_ident + "," + parseMessage(lamportMnemo, message).toStdString() + ")|";
            write += "(" + parseMessage(vectorMnemo, message).toStdString();
        }
        write = write.substr(0, write.size() - 1);
        write += ");\n";

        fichier << write;

        fichier.close();
        if (nbAnswersForSnapshot == nbNode) {
            nbAnswersForSnapshot = 0;
            sendEndSnapshotToBas();
            if (coherentSnapshot(fileName)) {
                fichier.open(fileName, std::ios::app);
                fichier << "Coherent snapshot" << std::endl;
                fichier.close();
            } else {
                fichier.open(fileName, std::ios::app);
                fichier << "Not coherent snapshot" << std::endl;
                fichier.close();
            }
        }
    }
}

// check if the snapshot is consistent
bool Net::coherentSnapshot(std::string fileName) {
    std::ifstream input(fileName);
    std::string line;
    //skip first line (nbNode=x : not interesting)
    std::getline(input, line);
    // skip second line with "variable partagee="
    std::getline(input, line);
    std::map<int, std::map<int, unsigned int>> mapHlg;
    while (std::getline(input, line)) {
        // Skip last line if coherent has already been checked
        if (line.find("snapshot") == std::string::npos) {
            QRegularExpression rx("[0-9]+");
            QRegularExpressionMatch match = rx.match(QString::fromStdString(line));
            QString numberOfLineNode = match.captured(0);
            std::string hlgVectori = line.substr(QString::fromStdString(line).lastIndexOf("("), line.size());
            hlgVectori = hlgVectori.substr(1, hlgVectori.size() - 3);
            mapHlg[numberOfLineNode.toInt()] = qstringToVector(QString::fromStdString(hlgVectori));
        }
    }
    for (unsigned int i = 0; i < nbNode; i++) {
        for (unsigned int j = 0; j < nbNode; j++) {
            if (mapHlg[j][i] > mapHlg[i][i]){
                input.close();
                return false;
            }
        }
    }
    input.close();
    return true;
}

// ############################# Waiting List #############################

// we suppose that before all this functions the clock had been incr

void Net::receiveDemandSC(QString msg)
{
    //check if is a demand of our own BAS (normaly impossible)
    if(isFromMe(msg)){
        if(waitingList[ident].demand != AC_SC_request){
            waitingList[ident].clk = *lamportClock;
            waitingList[ident].demand = AC_SC_request;
            testSC();
            sendRequestSC();
        }
    }

}

void Net::receiveEndSC(QString msg){
    if(isFromMe(msg)){
        waitingList[ident].clk = *lamportClock;
        waitingList[ident].demand = AC_SC_free;
        sc = false;
        debugSCDisplay();
        sendFreeSC();
    }
}

void Net::receiveRequestSC(QString msg){
    unsigned int id = parseMessage(senderMnemo,msg).toUInt();
    if(id != ident){
        waitingList[id].clk = getLamportFromMsg(msg);
        waitingList[id].demand = AC_SC_request;
        testSC();
        sendAcknowledgeSC(QString::number(id));

    }
}

void Net::receiveFreeSC(QString msg){
    unsigned int id = parseMessage(senderMnemo,msg).toUInt();
    if(id != ident){
        waitingList[id].clk = getLamportFromMsg(msg);
        waitingList[id].demand = AC_SC_free;

        testSC();
    }

}

void Net::receiveAcknowledgeSC(QString msg){
    unsigned int id = parseMessage(senderMnemo,msg).toUInt();
    if(!isFromMe(msg) && id != ident && waitingList[id].demand != AC_SC_request){
        waitingList[id].clk = getLamportFromMsg(msg);
        waitingList[id].demand = AC_SC_acknowledge;
        testSC();
    }
}

void Net::receiveUpdateSC(QString msg)
{
    sendUpdateSCToBas(parseMessage(payloadMnemo,msg));
}


void  Net::sendSnapshotRequestForAll(){
    WritingThread w_thread;
    updateRouter(ident, message_counter);
    w_thread.setParam(this,payloadSnapshotForAll + QString::number(lamportClock->getClock() + 1),QString("-2"));
    w_thread.appnet = true;
    w_thread.start();
    w_thread.wait();
}

void Net::sendStateToLoad(QString line, QString shared_variable, bool others) {
    QRegularExpression rx("[0-9]+");
    QRegularExpressionMatch match = rx.match(line);
    QString dest = match.captured(0);
    QString payloadToSend = payloadSnapshotStateToLoad + "|";
    payloadToSend += shared_variable + QString::fromStdString(line.toStdString().substr(line.indexOf("|"), line.size() - 1));
    // format payload: @state_load_snapshot|variable|(ident,hlgLamp)|(hlgVecto)

    WritingThread w_thread;
    w_thread.setParam(this,payloadToSend,dest);
    w_thread.appnet = others;
    w_thread.start();
    w_thread.wait();
}

void Net::sendStateForSnapshot(QString msg) {
    //msg received format: @bas_state_snapshot|number|variable|XX (XX = destinator)
    WritingThread w_thread;
    updateRouter(ident, message_counter);

    //Find number after payload and put it into payload
    QRegularExpression rx("[0-9]+");
    QRegularExpressionMatch match = rx.match(parseMessage(payloadMnemo, msg));
    QString number = match.captured(0);
    w_thread.setParam(this, payloadStateSnapshot + number, parseMessage(senderMnemo, msg));
    w_thread.appnet = true;
    w_thread.start();
    w_thread.wait();
}

void Net::sendEndSnapshotToBas() {
    WritingThread w_thread;
    w_thread.setParam(this, payloadEndSnapshot, QString::number(ident));
    w_thread.appnet = false;
    w_thread.start();
    w_thread.wait();
}

//test if we can go on SC
void Net::testSC(){
    debugSCDisplay();

    bool lower = true;

    if(waitingList[ident].demand == AC_SC_request)
    {
        for(unsigned int i=0; i< waitingList.size() && lower;++i)
        {
            if(i != ident){
                if(waitingList[ident].clk <waitingList[i].clk ){
                }
                else
                    lower = false;
            }
        }

        if (lower && !sc ){
            sendBeginSCToBas();
            sc = true;
        }

    }
}


// ############################# End Waiting List #############################

// ###################### SLOTS #########################

// Click to send the message
/*
void Net::sendMessage(bool addAppnet) {
    writing_thread = new WritingThread();
    writing_thread->setParam(this);
    writing_thread->appnet = addAppnet;
    writing_thread->start();
    writing_thread->wait();
    delete writing_thread;
}
*/

// Read from stdin, signal received because of something in stdin
/*
void Net::readStdin() {
    reading_thread = new ReadingThread();
    reading_thread->setParam(this);
    reading_thread->start();
    reading_thread->wait();
    delete reading_thread;
}
*/

// Parse the message according to a specific mnemonic
void Net::parseMessage() {
    QString message = reception_message_received->text();
}

// ############################# Waiting List Slots #############################

void  Net::sendRequestSC() {
    WritingThread w_thread;
    w_thread.setParam(this,requestSC,"-2");
    w_thread.appnet = true;
    w_thread.start();
    w_thread.wait();
}

void  Net::sendFreeSC() {
    WritingThread w_thread;
    w_thread.setParam(this,freeSC,"-2");
    w_thread.appnet = true;
    w_thread.start();
    w_thread.wait();
}

void  Net::sendAcknowledgeSC(QString dest) {
    WritingThread w_thread;
    w_thread.setParam(this,acknowledgeSC,dest);
    w_thread.appnet = true;
    w_thread.start();
    w_thread.wait();
}

void  Net::sendBeginSCToBas() {
    WritingThread w_thread;
    w_thread.setParam(this,beginSC,QString::number(ident));
    w_thread.appnet = false;
    w_thread.start();
    w_thread.wait();
}

void Net::debugSCDisplay(){
    if(debug){
        for(unsigned int i=0 ; i< waitingList.size(); ++i ){
            debug_sc_area->item(i,0)->setText(QString::number(waitingList[i].clk.getId()));
            debug_sc_area->item(i,1)->setText(QString::number(waitingList[i].clk.getClock()));
            debug_sc_area->item(i,2)->setText(getStrFromAction(waitingList[i].demand));
        }
    }
}

void Net::sendUpdateSCToBas(QString payload){
    WritingThread w_thread;
    w_thread.setParam(this,payload,QString::number(ident));
    w_thread.appnet = false;
    w_thread.start();
    w_thread.wait();
}



// ###################### Writing Thread #########################

void Net::sendMessage() {

/*
void WritingThread::run() {
    if (net->mutex->try_lock_for(std::chrono::milliseconds(net->timeout_mutex_ms)) ) {
        //on update le router afin de ne pas lire le message qu'on envoit si celui ci revient sur nous
        net->updateRouter(net->ident, net->message_counter);

        std::string output = net->getFormatedMessage(appnet, payload,dest).toStdString();
        std::cout << output << std::endl;
        net->mutex->unlock();
    }
}
*/

// ###################### Reading Thread #########################
// contains the diffusion logic, checks if the net has already received the message and wether or not to apply it/send it to the BAS.

void Net::readStdin() {
    bool payload(false);
    std::string tmp;
    QString message;
    notifier->setEnabled(false);

    do {
        std::getline(std::cin, tmp);
        message = QString::fromStdString(tmp);
        // passage dans le routeur
        // We suppose an action increase clocks only if it comes from BAS or is for BAS

        if (!alreadyReceived(message)) {
            if (withoutAppnet(message) && isFromMe(message)) {
                // Message comes from our own BAS:
                // Is it for local use (no resend) (dest == ident)
                if (parseMessage(destMnemo, message).toInt() == ident) {
                    payload = true;
                    updateClock(message);
                    updateRouter(-1, parseMessage(nseqMnemo, message).toUInt());
                } else {
                    payload = true;
                    updateClock(message);
                    //ici on update le router 2 fois, une pour le message reçu du BAS, et une pour le message qu'on renvoit pour les autres net
                    updateRouter(-1, parseMessage(nseqMnemo, message).toUInt()); //pour le bas key=-1
                    updateRouter(ident, message_counter);                        // pour le net key = sender
                    std::string outstr = getFormatedMessage(true, parseMessage(payloadMnemo, message),
                                                              parseMessage(destMnemo, message)).toStdString();
                    std::cout << outstr << std::endl;
                    QTest::qWait(30);
                }
            } else if (!withoutAppnet(message)) {
                // Message comes from another NET (msg contains appnet)
                if (toResend(message)) {
                    // si dest =! ident (-1 ou -2 ou autre) alors le message est renvoyé
                    // pas besoin de mettre à jour le routeur car la fonction AlreadyReceived le fait deja.
                    std::cout << message.toStdString() << std::endl;
                    QTest::qWait(30);
                }
                if (toApply(message)) {
                    // Message is for this NET and needs an action to be done
                    payload = true;
                    updateClock(message);
                    updateRouter(parseMessage(senderMnemo, message).toInt(), parseMessage(msgCounterMnemo, message).toUInt());

                    if(isForBAS(message)){
                        // send a message to the BAS
                        //on envoit un message au bas, il ne sera lu que par le bas et pas par les autres net, pas besoin d'update ici
                        std::cout << updateAppnet(message, false).toStdString() << std::endl;
                        QTest::qWait(30);
                    }
                }
            }
        }

        if (payload == true) {
            payloadAction(message);
        }

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        c = ungetc(std::getc(stdin), stdin);
        fcntl(STDIN_FILENO, F_SETFL, flags);

    } while (c!= EOF);
    notifier->setEnabled(true);
}

/*
void ReadingThread::run() {
    bool payload(false);
    std::string tmp;
    QString message;
    if (net->mutex->try_lock_for(std::chrono::milliseconds(net->timeout_mutex_ms)) ) {
        std::getline(std::cin, tmp);
        message = QString::fromStdString(tmp);
        // passage dans le routeur
        // We suppose an action increase clocks only if it comes from BAS or is for BAS

        if (!net->alreadyReceived(message)) {
            if (net->withoutAppnet(message) && net->isFromMe(message)) {
                // Message comes from our own BAS:
                // Is it for local use (no resend) (dest == ident)
                if (net->parseMessage(net->destMnemo, message).toInt() == net->ident) {
                    payload = true;
                    net->updateClock(message);
                    net->updateRouter(-1, net->parseMessage(net->nseqMnemo, message).toUInt());
                } else {
                    payload = true;
                    net->updateClock(message);
                    //ici on update le router 2 fois, une pour le message reçu du BAS, et une pour le message qu'on renvoit pour les autres net
                    net->updateRouter(-1, net->parseMessage(net->nseqMnemo, message).toUInt()); //pour le bas key=-1
                    net->updateRouter(net->ident, net->message_counter);                        // pour le net key = sender
                    std::string outstr = net->getFormatedMessage(true, net->parseMessage(net->payloadMnemo, message),
                                                              net->parseMessage(net->destMnemo, message)).toStdString();
                    std::cout << outstr << std::endl;
                    QTest::qWait(30);
                }
            } else if (!net->withoutAppnet(message)) {
                // Message comes from another NET (msg contains appnet)
                if (net->toResend(message)) {
                    // si dest =! ident (-1 ou -2 ou autre) alors le message est renvoyé
                    // pas besoin de mettre à jour le routeur car la fonction AlreadyReceived le fait deja.
                    std::cout << message.toStdString() << std::endl;
                    QTest::qWait(30);
                }
                if (net->toApply(message)) {
                    // Message is for this NET and needs an action to be done
                    payload = true;
                    net->updateClock(message);
                    net->updateRouter(net->parseMessage(net->senderMnemo, message).toInt(), net->parseMessage(net->msgCounterMnemo, message).toUInt());

                    if(net->isForBAS(message)){
                        // send a message to the BAS
                        //on envoit un message au bas, il ne sera lu que par le bas et pas par les autres net, pas besoin d'update ici
                        std::cout << net->updateAppnet(message, false).toStdString() << std::endl;
                        QTest::qWait(30);
                    }
                }
            }
        }
    }
    net->mutex->unlock();
    //il faut faire le payload après le unlock sinon certaines actions de payload seront blockées par le Mutex
    if (payload == true) {
        net->payloadAction(message);
    }
}
*/


// ###################### Router functions #########################

// permet de faire la différence entre un message deja reçu ou non
// utilise la key -1 pour un message venant du BAS, celui-ci utilisera alors le nseq comme value
bool Net::alreadyReceived(QString message){
    int sender = parseMessage(senderMnemo , message).toInt();
    unsigned int msg_counter = parseMessage(msgCounterMnemo , message).toUInt();
    unsigned int nseq = parseMessage(nseqMnemo , message).toUInt();

    if (withoutAppnet(message) && isFromMe(message)) {
        //receiving a msg from my bas, check map at -1 to see the last nseq received from my own bas
        if (routerMap.find(-1)!=routerMap.end() && routerMap[-1]>=nseq){
                return true;
        } else if (getAction(parseMessage(payloadMnemo, message)) == AC_state_snapshot){
                return true;
                // + ajouté dans isForBAS
        } else {
                routerMap[-1] = nseq;
                return false;
            }
    } else if (routerMap.find(sender)!=routerMap.end() && routerMap[sender]>=msg_counter){
        return true;
    } else {
        routerMap[sender] = msg_counter;
        return false;
    }
}

void Net::updateRouter(int sender, unsigned int count) {
    routerMap[sender] = count;
}

bool Net::withoutAppnet(QString message) {
    return parseMessage(appnetMnemo, message).toStdString() == "";
}

bool Net::toResend(QString message){
    return (parseMessage(destMnemo, message).toStdString() != std::to_string(ident));
}

// le NET applique un message reçu si celui-ci a pour dest:
// -1 (tout le monde est un destinataire)
// -2 (tout le monde est destinataire mais pas de message au BAS)
// son propre id
bool Net::toApply(QString message){
    return (parseMessage(destMnemo, message).toStdString() == std::to_string(ident) ||
            parseMessage(destMnemo, message).toStdString() == "-2" ||
            parseMessage(destMnemo, message).toStdString() == "-1");
}

bool Net::isFromMe(QString message){
    return (parseMessage(senderMnemo, message).toInt() == ident);
}

bool Net::isForBAS(QString message){
    return (parseMessage(destMnemo, message).toStdString() != "-2" &&
            getAction(parseMessage(payloadMnemo, message)) != AC_state_snapshot &&
            getAction(parseMessage(payloadMnemo, message)) != AC_SC_acknowledge);
}

bool Net::isForMe(QString message){
    if(parseMessage(destMnemo, message).toStdString() == "-1"){
        return true;
    } else {
        return parseMessage(destMnemo, message).toStdString() == std::to_string(ident);
    }
}


// ###################### Lamport Clock #########################

LamportClock Net::getLamportFromMsg(QString msg){
    LamportClock c(parseMessage(senderMnemo,msg).toInt(), parseMessage(lamportMnemo,msg).toInt());
    return c;
}
