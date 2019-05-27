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

/*
#include <QThread>
#include <QMutex>
#include <chrono>
*/

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <regex>
#include <fstream>


enum Options {
    OP_wrong = 0,
    OP_ident,
    OP_dest
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

        // values in payload for specific messages
        QString demandSC = "@demandeSC";    //the bas ask for a SC
        QString beginSC = "@debutSC";       //the net send to is bas that the SC is OK
        QString endSC = "@finSC";           //the bas send that he stop is SC
        QString updateSC = "@updateSC";
        QString snapshotRequest = "@snapshot";  // snapshot demand to NET
        QString snapshotStateRequest = "@all_snapshot"; // state demande from NET
        QString snapshotState = "@state_snapshot";      // message with state info
        QString snapshotBasState = "@bas_state_snapshot"; //bas sending shared variable value
        QString snapshotEnd = "@end_snapshot"; //end of the snapshot
        QString snapshotLoading = "@load_snapshot"; //load a snapshot file
        QString snapshotStateToLoad = "@state_load_snapshot"; //load a state from a snapshot

        bool sectionCritique = false;
        unsigned int compteurNbDebutSC;

        unsigned int nseq;  // number of message sent localy
        bool init;          // boolean to know if the ident initialization has been done
        unsigned int ident; // ident of the site (unique)

        QVBoxLayout* main; // Window

        QHBoxLayout* button_area;   // Top part containing three buttons
        QPushButton* quit;
        QPushButton* mode;
        QPushButton* send;
        QPushButton* endSc;            //fin section critique
        QPushButton* snapshot;         // Demande de réalisation de snapshot
        QPushButton* load_snapshot;    //demande de chargement d'une config

        QGridLayout* section_critique_area;
        QGroupBox* section_critique_box;
        QLabel* shared_variable_label;
        CustomLineEdit* shared_variable;
        QLabel* indicateurSC;
        QString var;
        bool varChanged;

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

        QString getFormatedMessage(QString payload = "",QString desti="");
        Options getOption(QString arg);
        void initialization(int argc, char* argv[]);

        //snapshot
        void sendSnapshotLoading(QString);

    public:
        Pil(int argc, char* argv[]);
        ~Pil();
        QString parseMessage(QString mnemo, QString message); //return the value linked to a mnemonic, "" if no mnemonic find in the message
        void enableSnapshot();

    protected slots:
        void sendMessage();
        void sendAskSC();
        void sendEndSC();
        void sendUpdateSC();
        void sendSnasphotRequest();
        void loadSnapshot();
        void readStdin();
        void parseMessage();
        void enableSectionCritique();
        void disableSectionCritique();
        void updateSectionCritique(QString message = "");
        void updateLoadStateSnapshot(QString message);
};

/*
class WritingThread: public QThread {
    Q_OBJECT
    Interface* interf;
    QString payload;
    QString dest;

    protected:
        bool continue_thread = true;

        friend class Interface;
        void run();

    public:
        void setParam(Interface* it, QString p = "",QString d = ""){
            this->interf = it;
            this->payload= p;
            this->dest= d;
        }
};


class ReadingThread: public QThread {
    Q_OBJECT
    Interface* interf;

    protected:

        void run();

    public:
        void setParam(Interface* it){
            this->interf = it;
        }

};
*/

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomLineEdit(Pil* b,QWidget *parent = nullptr) : QLineEdit(parent), bas(b){}
    Pil* bas;
protected:
    void focusInEvent(QFocusEvent *event)
    {
        QLineEdit::focusInEvent(event);
        if(event->gotFocus()){
            bas->sendAskSC();
        }

    }
};

#endif // PIL_H
