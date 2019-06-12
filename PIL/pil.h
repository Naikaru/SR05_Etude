#ifndef PIL_H
#define PIL_H

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
#include <QDebug>

#include <QSocketNotifier>
#include "map.h"
#include "astar.h"
#include "message.h"
#include "tcpclientmanager.h"
#include "algo.h"

enum Options {
    OP_wrong = 0,
    OP_ident,
    OP_dest,
    OP_nbRobot,
    OP_x,
    OP_y,
    OP_remote
};

enum Actions {
    AC_buffer = 0,
};

/*
class WritingThread;
class ReadingThread;
*/
class CustomLineEdit;
class SendingThread;

class Pil: public QWidget {
    Q_OBJECT
    friend class CustomLineEdit;

    protected:

        QString payloadMnemo = "payload";   //the mnemonic to have access to the message 'what'
        QString nseqMnemo = "nseq";         //the mnemonic to have access to the number of message sent by the sender
        QString senderMnemo = "who";        //the mnemonic to have access to the sender
        QString destMnemo = "dest";         //the mnemonic to have access to the destinator
        QString appnetMnemo = "appnet";     //the mnemonic to have access to the appnet
        QString defaultSep= "/";            //separator between information in message
        QString valueSep= "~";              //separator between mnemonic and value
        const QString mnemoAckMove = "moved";
        const QString mnemoAckTurn = "turned";
        const QString mnemoAckError = "order";
        const QString mnemoAckInit = "init";
        const QString mnemoMove = "move";
        const QString mnemoInit = "init";
        const QString mnemoTurn = "turn";
        const QString mnemoRobotOrder = "robord";
        const QString mnemoRobotAck = "roback";

        QString bufferPayload = "@buffer";

        unsigned int nseq;  // number of message sent localy
        bool init;          // boolean to know if the ident initialization has been done
        unsigned int ident=0; // ident of the robot (unique)

        unsigned int nbActions = 1;

        unsigned int nbRobot = 1;   //number of robots
        unsigned int nbRobotsInitialized;
        int xInit=0;              //initial pos of the robot
        int yInit=0;

        std::map<unsigned int, unsigned int> nbActionsRobot;

        TcpClientManager client;

        QStringList currentActionToDo;
        unsigned int currentIndexOfAction;
        Algo* algo;

        QVBoxLayout* main; // Window
        QHBoxLayout* button_area;   // Top part containing three buttons
        QPushButton* quit;
        QPushButton* mode;
        QPushButton* send;

        QGridLayout* reception_area; // 2nd part, reception area, display received message
        QGroupBox* reception_box;
        QLabel* reception_fullmessage;   // Know the entire message received
        QLabel* reception_message_received;
        QLabel* reception_sender;
        QLabel* reception_nseq;
        QLabel* reception_dest;

        QVBoxLayout* parse_area;// 3rd part,
        QGroupBox* parse_box;
        QGridLayout* parse_layout; // Layout containing button, label and received message
        QPushButton* parse;
        QLineEdit* mnemonic;
        QLabel* parse_value;

        QVBoxLayout* send_area; // Lower part, sending area, box with message to send and speed
        QGroupBox* send_box;
        QTextEdit* text;

        QHBoxLayout* dest_area;
        QLineEdit* dest;

        QHBoxLayout* info_area; // layout containing local information
        QLabel* info_ident;
        QLabel* info_nseq;

        // Notifier for reading on stdin
        QSocketNotifier* notifier;

        bool reading_writing = false;

        const unsigned int MAX_BUFFER = 20;
        const unsigned int DISTANCE_MAX = 10;

        QVector<QStringList> buffer;

        //permet de savoir les robots à proximités
        QVector<int> nearRobot;

        // sequence of instructions
        Map* map = nullptr;

        // local map of the environment
        QVector<QPair<unsigned int, unsigned int>> frontiers;

        // list of frontiers coords

        SendingThread* sendingThread = nullptr;
        // Functions

        QString getFormatedMessage(QString payload = "",QString desti="");
        Options getOption(QString arg);
        void initialization(int argc, char* argv[]);

        QPair<unsigned int, unsigned int> chooseFrontier(std::list<Cellule*> closedList);
        QVector<QStringList> parseBuffer(QString payload);
        void applyBufferForRobot(unsigned int r, QVector<QStringList> buffer);
        void applyActionFromBuffer(int r, QStringList action);
        void moveMovementReceived(unsigned int, unsigned int);
        void turnMovementReceived(unsigned int);

        bool robotsTooFar(Pos, Pos);

    public:
        void applyBufferFromMessage(QString);
        Pil(int argc, char* argv[]);
        ~Pil();
        QString parseMessage(QString mnemo, QString message); //return the value linked to a mnemonic, "" if no mnemonic find in the message
        void enableSnapshot();

        QVector<QStringList> getBuffer() {return buffer; }
        void addMovementInBuffer(unsigned int nbAction, QString movement, QString distance, QString finalPosition);
        void addInitInBufferAndSend();
        void sendBufferToNet();

        //lance l'algorithme
        void runAlgo();

        //applique l'action courante en l'envoyant au robot ou à la simu
        void applyAction();

    protected slots:
        void sendMessage();
        void sendBuffer(QString);
        void readStdin();
        void parseMessage();

    public slots:
        void rmtMessage(Message mess);
};

class SendingThread: public QThread {
    Q_OBJECT
    private:
        const int WAITING_TIME = 2000;
        Pil* pil;
        bool cont = true;

        void run();

    public:
        void stopThread() {cont = false; }
        void startThread() {cont = true; }
        void setParam(Pil* p) {this->pil = p; }
};



#endif // PIL_H
