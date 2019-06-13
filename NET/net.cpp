#include "net.h"


// ###################### Constructor #########################

Net::Net(int argc, char* argv[]): QWidget() {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->resize(300,250);
    nseq = 0;
    nbNode = 0;
    message_counter = 1;

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
    main->addWidget(reception_box);
    main->addWidget(clock_box);
    main->addWidget(info_box);

    notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this); //fileno(stdin)

    setWindowTitle(QString("NET ") + QString::number(ident));

    // Slots linked
    connect(quit, SIGNAL(clicked()), this, SLOT(close())); // Close window
    // Slot for notifier (reading)
    connect(notifier, SIGNAL(activated(int)), this, SLOT(readStdin()));
}

// ###################### RDestructor #########################

Net::~Net() {}

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
}

QString Net::getStrFromAction(Actions action){
    // define action
    // if (action == ACTION)
    return "defaut";
}

Actions  Net::getAction(QString arg){
    // define cmpt
    // if (arg == ARG)
    return AC_default;
}

void Net::payloadAction(QString message) {
    QString payload = parseMessage(payloadMnemo,message);
    switch (getAction(payload))
    {
        // case AC_action
        case AC_default:                // reception message
            displayMessage(message);
            break;
        default:
            break;
    }
}

void Net::displayMessage(QString message)
{
    //reception_message_received->setText(parseMessage(payloadMnemo, message));
    reception_dest->setText(parseMessage(destMnemo, message));
    reception_sender->setText(parseMessage(senderMnemo, message));
    reception_nseq->setText(parseMessage(nseqMnemo, message));
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

// ###################### SLOTS #########################

// Parse the message according to a specific mnemonic
void Net::parseMessage() {
    QString message = reception_message_received->text();
}

// ###################### Writing #########################
void Net::sendMessage(bool addAppnet, QString payload, QString dest) {
    updateRouter(ident, message_counter);
    std::string output = getFormatedMessage(addAppnet, payload,dest).toStdString();
    std::cout << output << std::endl;
}


// ###################### Reading #########################
// contains the diffusion logic, checks if the net has already received the message and wether or not to apply it/send it to the BAS.

void Net::readStdin() {
    bool payload(false);
    std::string tmp;
    QString message;
    notifier->setEnabled(false);
    char c;

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    c = ungetc(std::getc(stdin), stdin);
    fcntl(STDIN_FILENO, F_SETFL, flags);
    while(c != EOF) {
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

    }
    notifier->setEnabled(true);
}

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
        /*
        } else if (getAction(parseMessage(payloadMnemo, message)) == AC_state_snapshot){
                return true;
        */
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
    return (parseMessage(destMnemo, message).toStdString() != "-2");
            // && getAction(parseMessage(payloadMnemo, message)) != AC_SC_acknowledge
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
