#ifndef NET_H
#define NET_H

#include <atomic>
#include <iostream>
#include <string.h>
#include <fstream>
#include <regex>
#include <unistd.h>
#include <fcntl.h>

#include "vectorclock.h"
#include "lamportclock.h"

#include <QtWidgets>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QSpinBox>
#include <QtTest/QtTest>

#include <QSocketNotifier>

// enum of all options we can pass via the main
enum Options {
    OP_wrong = 0,
    OP_ident,
    OP_nbNode
};

// Enumeration of possible actions depending on message payload value
enum Actions {
    AC_default = 0,
};

class Net: public QWidget {
Q_OBJECT
private:
    VectorClock* vectorClock;
    LamportClock* lamportClock;

    // Message content
    QString payloadMnemo = "payload";       //the mnemonic to have access to the message 'what'
    QString nseqMnemo = "nseq";             //the mnemonic to have access to the number of message sent by the sender
    QString lamportMnemo = "hlglam";        //the mnemonic to have access to the lamport clock of the bas app
    QString vectorMnemo = "hlgvec";         //the mnemonic to have access to the vector clock of the bas app
    QString senderMnemo = "who";            //the mnemonic to have access to the app identifier
    QString destMnemo = "dest";             //the mnemonic to have access to the app destination
    QString appnetMnemo = "appnet";         //the mnemonic to ensure the message go from NET to NET
    QString msgCounterMnemo = "msgcount";   //the mnemonic to keep track of the messages sent by the net (router)

    QString defaultSep = "/";       // Message separator between information
    QString valueSep = "~";         // Message separator between mnemonic and value

    bool init;          // boolean to know if the ident initialization has been down
    unsigned int ident; // ident of the node
    unsigned int nseq;  // number of messages sent localy ~> to we keep it in NET ?
    unsigned int nbNode;// number of nodes in the topologie

    // Interface attributs
    QVBoxLayout* main; // Window

    QVBoxLayout* button_area; // Top part containing quit button
    QPushButton* quit;

    QGridLayout* reception_area; // 2nd part, reception area, display received message
    QGroupBox* reception_box;
    QLabel* reception_message_label;
    QLabel* reception_message_received;
    QLabel* reception_sender;
    QLabel* reception_nseq;
    QLabel* reception_dest;

    QVBoxLayout* clock_area;    // Clock area, display clocks' value
    QGroupBox* clock_box;
    QGridLayout* clock_layout;
    QLabel* lamport_value;
    QLabel* vector_value;

    QGroupBox* info_box;
    QHBoxLayout* info_area; // layout containing local information
    QLabel* info_ident;

    // Notifier for reading on stdin
    QSocketNotifier* notifier;

    //router
    std::map<int, unsigned int> routerMap;
    unsigned int message_counter;

    bool alreadyReceived(QString message);
    void updateRouter(int sender, unsigned int lamport);
    bool isForMe(QString message);
    bool isForBAS(QString message);
    bool isFromMe(QString message);
    bool withoutAppnet(QString message);
    bool toResend(QString message);
    bool toApply(QString message);

    LamportClock getLamportFromMsg(QString msg);

    QString getFormatedMessage(bool addAppnet, QString payload="", QString dest="");
    QString updateAppnet(QString message, bool addAppnet);
    Options getOption(QString arg);                 //convert a string to a type Options (after that we can use a switch case)
    Actions getAction(QString arg);                 //convert a string to a type Actions (after that we can use a switch case)
    QString getStrFromAction(Actions action);       //convert an action to a string (after that we can use a switch case)
    void initialization(int argc, char* argv[]);    //initialize some parameter according to main options (for example nbNode or the ident)

protected slots:
    void readStdin();
    void parseMessage();
    void sendMessage(bool addAppnet, QString payload="", QString dest="");               // send a message, boolean meaning for NET or not

public:
    //Net();
    Net(int argc, char* argv[]);
    ~Net();
    QString parseMessage(QString mnemo, QString message);   //return the value linked to a mnemonic, "" if no mnemonic find in the message

    void updateClock(QString message="");
    void displayMessage(QString message);
    void payloadAction(QString message);
};

#endif // NET_H
