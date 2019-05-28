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
#include <QThread>
#include <QMutex>
#include <chrono>

// enum of all options we can pass via the main
enum Options {
    OP_wrong = 0,
    OP_ident,
    OP_nbNode
};

// Enumeration of possible actions depending on message payload value
enum Actions {
    AC_default = 0,
    AC_snapshot_request_BAS,
    AC_snapshot_for_all,
    AC_snapshot_bas_state,
    AC_state_snapshot,
    AC_snapshot_loading,
    AC_snapshot_state_to_load,
    AC_SC_demand,
    AC_SC_end,
    AC_SC_request,
    AC_SC_free,
    AC_SC_acknowledge,
    AC_SC_update
};


// Structure stocked in the waiting list
struct Sc{
    LamportClock clk;
    Actions demand;

    Sc(): clk(LamportClock()), demand(AC_default){
    }

};


//class dataType;
class WritingThread;
class ReadingThread;

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
    QString payloadSnapshot = "@snapshot";              //value to indicate BAS sent a snapshot request
    QString payloadSnapshotForAll = "@all_snapshot";    //value to indicate snapshot is asked for all
    QString payloadBasStateSnapshot = "@bas_state_snapshot"; //value to indicate bas sent its state to net
    QString payloadStateSnapshot = "@state_snapshot";   //value to indicate it is a state sent back for a snapshot
    QString payloadEndSnapshot = "@end_snapshot";       //value to indicate the snapshot is ended
    QString payloadSnapshotLoading = "@load_snapshot";  //value to indicate to start loading a file
    QString payloadSnapshotStateToLoad = "@state_load_snapshot";  //value to indicate to load data from snapshot


    QString demandSC = "@demandeSC";    //the bas ask for a SC
    QString beginSC = "@debutSC";       //the net send to is bas that the SC is OK
    QString endSC = "@finSC";           //the bas send that he stop is SC

    QString requestSC = "@requetSC";        //the NET ask for a SC to the osers NET
    QString acknowledgeSC = "@accuseSC";    //the net send to is bas that the SC is OK
    QString freeSC  = "@liberationSC";      //the bas send that he stop is SC
    QString updateSC = "@updateSC";         //following by the new value of the shared variable

    bool init;          // boolean to know if the ident initialization has been down
    unsigned int ident; // ident of the node
    unsigned int nseq;  // number of messages sent localy ~> to we keep it in NET ?
    unsigned int nbNode;// number of nodes in the topologie

    //snapshot
    unsigned int nbAnswersForSnapshot = 0; // Number of response for snaphsot (the response contains the state of node)
    QDateTime datetime = QDateTime::currentDateTime(); // current date for the title of the snapshot file

    //critical section
    std::vector<Sc> waitingList;        //waiting list
    bool debug = true;                  //boolean to display the waiting list for debugging
    bool sc;                            //boolean to know if the site is in SC
    QTableWidget* debug_sc_area;
    QPushButton* refresh;


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
    bool reading_writing = false;

    friend class WritingThread;
    friend class ReadingThread;

    // Threads
    WritingThread* writing_thread;
    ReadingThread* reading_thread;

    // Mutex
    const unsigned int timeout_mutex_ms = 10000;// Timeout for accessing mutex
    QMutex* mutex;                              // allow the writing and reading atomicity

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

    void saveStateSnapshot(QString, bool myself = false, QString payloadMsg = "");

    // all functions linked with a reception. SC = critical section
    void receiveDemandSC(QString msg);          //called when receive a demande of SC from is BAS
    void receiveEndSC(QString msg);             //called when receive a end SC from is BAS
    void receiveRequestSC(QString msg);         //called when receive a request SC from a net
    void receiveFreeSC(QString msg);            //called when receive a release  SC from a net
    void receiveAcknowledgeSC(QString msg);     //called when receive a acknoledgement of SC from a net
    void receiveUpdateSC(QString msg);          //called when receive a the new value of the shared variable
    void testSC();                              //test if we can have the SC, called every time de waiting list is updated

    // Snapshot functions
    void receiveSnapshotDemandFromBas(QString);     // called when a message from BAS is received to launch snapshot
    void sendSnapshotRequestForAll();               // send a message to all to send their state
    void receiveSnapshotDemandForAll(QString);      // called when a message from NET is received to send state
    void sendStateForSnapshot(QString msg);         // send message with state information to initial NET
    void sendEndSnapshotToBas();                    // send message to inform snapshot finished and activate button
    bool coherentSnapshot(std::string);             // verify coherence of a file

    // Load snapshot file
    void loadFile(QString);                         // read a file with snapshot information
    void sendStateToLoad(QString, QString, bool);   // send a message with state read in file
    void loadState(QString, bool = false);          // load state with the message information

protected slots:
    void readStdin();

    void parseMessage();
    void sendMessage(bool addAppnet);               // send a message, boolean meaning for NET or not

    //function to send an action linked with SC
    void sendRequestSC();                       // send a request of SC (after a BAS demand)
    void sendFreeSC();                          // send a release of SC (after a BAS end)
    void sendAcknowledgeSC(QString dest);       // send an acknolegment of SC (after reception of NET request)
    void sendBeginSCToBas();                    // send begin SC to is BAS (in method testSC)
    void sendUpdateSCToBas(QString payload);    // send the new value of the shared variable
    void debugSCDisplay();                      //this function display the waiting list, only when 'debug' = true

public:
    //Net();
    Net(int argc, char* argv[]);
    ~Net();
    QString parseMessage(QString mnemo, QString message);   //return the value linked to a mnemonic, "" if no mnemonic find in the message

    void updateClock(QString message="");
    void displayMessage(QString message);
    void payloadAction(QString message);
};

/*
class dataType{
public:
    int data = 0; //class representing the type of the data shared by different sites. To be deleted or replaced by a template later.
};
*/


class WritingThread: public QThread {
    Q_OBJECT
    Net* net;
    protected:
        bool continue_thread;
        QString payload;
        QString dest;
        bool appnet;

        friend class Net;
        void run();
    public:

        WritingThread(): continue_thread(false),payload(""),dest(""),appnet(false)
        {}

        void setParam(Net* it, QString p = "",QString d = ""){  // attributes used to create message
            this->net = it;
            this->payload = p;
            this->dest = d;
        }
};


class ReadingThread: public QThread {
    Q_OBJECT
    Net* net;
    protected:
        void run();
    public:
        void setParam(Net* it){
            this->net = it;
        }
};

#endif // NET_H
