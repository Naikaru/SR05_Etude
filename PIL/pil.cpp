#include "pil.h"


// ###################### Constructor #########################

Pil::Pil(int argc, char* argv[]): QWidget() {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->resize(500,350);
    main = new QVBoxLayout();
    nseq = 0;
    init = false;
    // Top bar with 5 buttons: Envoyer - Quitter - Fin SC - Instantané - Charger configuration
    button_area = new QHBoxLayout();
    quit = new QPushButton("Quitter",this);
    send = new QPushButton("Envoyer",this);
    //askSc = new QPushButton("Demande SC",this);
    endSc = new QPushButton("Fin SC",this);
    snapshot = new QPushButton("Instantané", this);
    load_snapshot = new QPushButton("Charger configuration", this);

    button_area->addWidget(send);
    button_area->addWidget(quit);
    //button_area->addWidget(askSc);
    button_area->addWidget(endSc);
    button_area->addWidget(snapshot);
    button_area->addWidget(load_snapshot);

    compteurNbDebutSC = 0;

    section_critique_area = new QGridLayout();
    section_critique_box =new QGroupBox(tr("Section Critique"));
    section_critique_box->setStyleSheet("QGroupBox{font-weight: bold;}");
    shared_variable_label = new QLabel("Texte partagé",this);
    shared_variable = new CustomLineEdit(this);
    section_critique_area->addWidget(shared_variable_label,0,0);
    section_critique_area->addWidget(shared_variable,0,1);
    indicateurSC = new QLabel("SC");
    indicateurSC->setStyleSheet("QLabel {color : red;font-weight: bold; }");
    section_critique_area->addWidget(indicateurSC,0,2);
    section_critique_box->setLayout(section_critique_area);
    shared_variable->setReadOnly(true);
    var = "";
    varChanged = false;

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
    main->addWidget(section_critique_box);
    main->addWidget(reception_box);
    main->addWidget(parse_box);
    main->addWidget(send_box);

    notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this); //fileno(stdin)

    // mutex = new QMutex();

    // writing_thread = nullptr;
    // reading_thread = nullptr;

    initialization(argc,argv);

    setWindowTitle(QString("BAS ")+ QString::number(ident) );

    // Slots linked
    connect(quit, SIGNAL(clicked()), this, SLOT(close())); // Close window
    connect(send, SIGNAL(clicked()), this, SLOT(sendMessage())); // Send message
    connect(endSc, SIGNAL(clicked()), this, SLOT(sendEndSC())); // Send message
    connect(endSc, SIGNAL(clicked()), this, SLOT(disableSectionCritique())); // Send message
    connect(snapshot, SIGNAL(clicked()), this, SLOT(sendSnasphotRequest()));
    connect(load_snapshot, SIGNAL(clicked()), this, SLOT(loadSnapshot()));

    connect(parse, SIGNAL(clicked()), this, SLOT(parseMessage())); // parse message
    connect(mnemonic, SIGNAL(returnPressed()), this, SLOT(parseMessage())); // parse message
    // Slot for notifier (reading)
    connect(notifier, SIGNAL(activated(int)), this, SLOT(readStdin()));
    connect(shared_variable,SIGNAL(returnPressed()), this, SLOT(sendUpdateSC()));
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
                default:
                    break;
            }
        }
    }
}

void Pil::enableSectionCritique(){
    ++compteurNbDebutSC;
    sectionCritique = true;
    indicateurSC->setStyleSheet("QLabel {color : green;font-weight: bold; }");
    shared_variable->setReadOnly(false);
}

void Pil::disableSectionCritique(){
    sectionCritique = false;
    indicateurSC->setStyleSheet("QLabel {color : red;font-weight: bold; }");
    shared_variable->setReadOnly(true);
}

void Pil::sendUpdateSC(){
//    WritingThread* writing_thread = new WritingThread();
//    QString payload("");
//    payload += updateSC + " "+ shared_variable->text();
//    writing_thread->setParam(this, payload,"-1"); // dest = him self
//    writing_thread->start();
//    writing_thread->wait();
//    delete writing_thread;
//    writing_thread = nullptr;
}


void Pil::enableSnapshot(){
    snapshot->setEnabled(true);
}


// --------------- SLOTS

// Click to send the message
void Pil::sendMessage() {
    info_nseq->setText(QString::number(++nseq));
    // sending message to NET
    std::cout << getFormatedMessage().toStdString() << std::endl;
}
/*
void Pil::sendMessage() {
    writing_thread = new WritingThread();
    writing_thread->setParam(this);
    writing_thread->start();
    writing_thread->wait();
    delete writing_thread;
    writing_thread = nullptr;
}
*/

// When attempting to modify shared var
void Pil::sendAskSC() {
//    WritingThread* writing_thread = new WritingThread();
//    writing_thread->setParam(this, demandSC,QString::number(ident)); // dest = him self
//    writing_thread->start();
//    writing_thread->wait();
//    delete writing_thread;
//    writing_thread = nullptr;
}

// Click to send to other app that SC is over on this BAS
void Pil::sendEndSC() {
//    WritingThread* writing_thread = new WritingThread();
//    writing_thread->setParam(this, endSC,QString::number(ident));
//    writing_thread->start();
//    writing_thread->wait();
//    delete writing_thread;
//    writing_thread = nullptr;
}

void Pil::sendSnasphotRequest(){
//    snapshot->setEnabled(false);
//    WritingThread* writing_thread = new WritingThread();
//    QString payloadToSend = snapshotRequest + QString::fromStdString("|") + shared_variable->text();
//    writing_thread->setParam(this, payloadToSend, QString::number(ident));
//    writing_thread->start();
//    writing_thread->wait();
//    delete writing_thread;
//    writing_thread = nullptr;
}

void Pil::sendSnapshotLoading(QString fileName) {
//    WritingThread* writing_thread = new WritingThread();
//    QString payloadToSend = snapshotLoading + QString::fromStdString("|") + QString::fromStdString(fileName.toStdString().substr(fileName.lastIndexOf("/") + 1, fileName.size()));
//    writing_thread->setParam(this, payloadToSend, QString::number(ident));
//    writing_thread->start();
//    writing_thread->wait();
//    delete writing_thread;
//    writing_thread = nullptr;
}

void Pil::loadSnapshot(){
    try {
        QString fileName = QFileDialog::getOpenFileName(this,
                        tr("Charger le fichier"), "",
                        tr("Adresse du fichier (*.txt)")
                    );
        std::regex extensionTxt("(.*)(.txt)");
        if (!std::regex_match(fileName.toStdString(), extensionTxt)) {
            std::cerr << "Le fichier à charger n'est pas au bon format, il doit être au format .txt" << std::endl;
            return;
        }

        sendSnapshotLoading(fileName);

    } catch (...) {}
}

void Pil::updateSectionCritique(QString message){
    QString tmp (parseMessage(payloadMnemo, message));
    this->var = tmp.split("@updateSC ",QString::SkipEmptyParts)[0];
    varChanged = true;
}

void Pil::updateLoadStateSnapshot(QString message){
    this->var = message;
    varChanged = true;
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

            if (parseMessage(payloadMnemo, QString::fromStdString(message)) == beginSC ){
               enableSectionCritique();
            }
            //mise à jour section critique
            else if(parseMessage(payloadMnemo, QString::fromStdString(message)).startsWith(updateSC)) {
               //pour ne pas recevoir notre propre demande de mise à jour (sécurité)
               if(parseMessage(senderMnemo, QString::fromStdString(message)).toUInt() != ident){
                   updateSectionCritique(QString::fromStdString(message));
               }
            }
            // si reception message état pour snapshot
            else if (parseMessage(payloadMnemo, QString::fromStdString(message)).startsWith(snapshotState)
            // si reception message all snapshot, on envoie l'état de la variable partagée au NET
                   || parseMessage(payloadMnemo, QString::fromStdString(message)).startsWith(snapshotStateRequest)){

            }
            else if (parseMessage(payloadMnemo, QString::fromStdString(message)) == snapshotEnd){
               enableSnapshot();
            }
            else if (parseMessage(payloadMnemo, QString::fromStdString(message)).startsWith(snapshotStateToLoad)) {
               // format payload: @state_load_snapshot|variable|(ident,hlgLamp)|(hlgVecto)
               QString payload = parseMessage(payloadMnemo, QString::fromStdString(message));
               QStringList value = payload.split("|");
               updateLoadStateSnapshot(value[1]);
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

/*
void Pil::readStdin() {

    reading_thread = new ReadingThread();
    reading_thread->setParam(this);
    reading_thread->start();
    reading_thread->wait();
    if (varChanged){
        shared_variable->setText(var);
        varChanged = false;
    }
    delete reading_thread;
    reading_thread = nullptr;
}
*/

// Slot to parse the message according to a specific mnemonic
void Pil::parseMessage() {
    QString message = reception_fullmessage->text();
    QString mnemo = mnemonic->text();
    parse_value->setText(parseMessage(mnemo,message));
}

/*
// Function called during writing_thread->start()
void WritingThread::run() {
    if (interf->mutex->try_lock_for(std::chrono::milliseconds(interf->timeout_mutex_ms)) ) {
        interf->info_nseq->setText(QString::number(++interf->nseq));
        // sending message to NET
        std::cout << interf->getFormatedMessage(payload,dest).toStdString() << std::endl;
        interf->mutex->unlock();
    }
}


// Function called during reading_thread->start()
void ReadingThread::run() {
    // ask for mutex: wait until mutex available or mutex timeout
    if (interf->mutex->try_lock_for(std::chrono::milliseconds(interf->timeout_mutex_ms)) ) {
        //std::cerr << "ReadingThead : "<< std::endl;
        std::string message;
        while(!std::cin.eof()) {
            std::getline(std::cin, message);
            std::cerr << "Reiceved : " << message << std::endl;
            // Message intercepted only if it contains non empty appnet value
            if (interf->parseMessage(interf->appnetMnemo, QString::fromStdString(message)) == "")
            {
               if (interf->parseMessage(interf->payloadMnemo, QString::fromStdString(message)) == interf->beginSC ){
                   interf->enableSectionCritique();
               }
               //mise à jour section critique
               else if(interf->parseMessage(interf->payloadMnemo, QString::fromStdString(message)).startsWith(interf->updateSC))
               {
                   //pour ne pas recevoir notre propre demande de mise à jour (sécurité)
                   if(interf->parseMessage(interf->senderMnemo, QString::fromStdString(message)).toUInt() != interf->ident){
                       interf->updateSectionCritique(QString::fromStdString(message));
                   }
               }
               // si reception message état pour snapshot
               else if (interf->parseMessage(interf->payloadMnemo, QString::fromStdString(message)).startsWith(interf->snapshotState)
               // si reception message all snapshot, on envoie l'état de la variable partagée au NET
                       || interf->parseMessage(interf->payloadMnemo, QString::fromStdString(message)).startsWith(interf->snapshotStateRequest)){

               }
               else if (interf->parseMessage(interf->payloadMnemo, QString::fromStdString(message)) == interf->snapshotEnd){
                   interf->enableSnapshot();
               }
               else if (interf->parseMessage(interf->payloadMnemo, QString::fromStdString(message)).startsWith(interf->snapshotStateToLoad)) {
                   // format payload: @state_load_snapshot|variable|(ident,hlgLamp)|(hlgVecto)
                   QString payload = interf->parseMessage(interf->payloadMnemo, QString::fromStdString(message));
                   QStringList value = payload.split("|");
                   interf->updateLoadStateSnapshot(value[1]);
               }
               else {
                   // Recepted message is for a BAS app
                   interf->reception_fullmessage->setText(QString::fromStdString(message));
                   interf->reception_message_received->setText(interf->parseMessage(interf->payloadMnemo, QString::fromStdString(message)));
                   interf->reception_nseq->setText(interf->parseMessage(interf->nseqMnemo, QString::fromStdString(message)));
                   interf->reception_sender->setText(interf->parseMessage(interf->senderMnemo, QString::fromStdString(message)));
                   interf->reception_dest->setText(interf->parseMessage(interf->destMnemo, QString::fromStdString(message)));
               }
            }
        }
    }
    interf->mutex->unlock();
    // thread release the mutex
}
*/
