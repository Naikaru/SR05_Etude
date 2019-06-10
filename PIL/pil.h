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

#include <QSocketNotifier>
#include "map.h"
#include "astar.h"

enum Options {
    OP_wrong = 0,
    OP_ident,
    OP_dest,
    OP_nbRobot,
    OP_x,
    OP_y,
    OP_remote
};

/*
class WritingThread;
class ReadingThread;
*/
class CustomLineEdit;


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

        QString bufferPayload = "@buffer";

        unsigned int nseq;  // number of message sent localy
        bool init;          // boolean to know if the ident initialization has been done
        unsigned int ident; // ident of the robot (unique)

        unsigned int nbRobot;   //number of robots
        int xInit;              //initial pos of the robot
        int yInit;

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

        const unsigned int MAX_BUFFER = 100;

        QVector<QPair<QString, QString>> buffer;
        // sequence of instructions
        Map* map = nullptr;
        // local map of the environment
        QVector<QPair<unsigned int, unsigned int>> frontiers;
        // list of frontiers coords

        // Functions

        QString getFormatedMessage(QString payload = "",QString desti="");
        Options getOption(QString arg);
        void initialization(int argc, char* argv[]);

    public:
        Pil(int argc, char* argv[]);
        ~Pil();
        QString parseMessage(QString mnemo, QString message); //return the value linked to a mnemonic, "" if no mnemonic find in the message
        void enableSnapshot();

        QVector<QPair<QString, QString>> getBuffer() {return buffer; }
        void addMovementInBuffer(QString, QString);
        void sendBufferToNet();
        QPair<unsigned, unsigned> chooseFrontier();

    protected slots:
        void sendMessage();
        void sendBuffer(QString);
        void readStdin();
        void parseMessage();
};


#endif // PIL_H
