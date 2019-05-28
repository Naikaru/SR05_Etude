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
#include <algorithm>
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

        unsigned int nseq;  // number of message sent localy
        bool init;          // boolean to know if the ident initialization has been done
        unsigned int ident; // ident of the site (unique)

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
        void readStdin();
        void parseMessage();
};

/*
La map est une variable partagée par tous les robots
Elle contiendra les infos de positions et d'orientions de tous les robots
    + les zones découvertes.

*/

struct Robot
{
    int x;
    int y;
    int heading;
    QString color;
};



class Map : private QTableWidget
{
    friend class pil;

    unsigned int nbC;       //nb de colonne
    unsigned int nbL;       //nb de ligne
    unsigned int width;     //largeur
    unsigned int height;    //hauteur
    unsigned int dCell;     //hauteur et largeur d'une cellule

    std::vector<QString> colors; //défini les couleurs que peuvent prendre les robots sur la map
    std::map<int, Robot> robots;       // défini pour chaque robot sa position (x, y) et son angle heading, dans cet ordre

public :

    Map(unsigned int w=300,unsigned int h=300,unsigned int c=20,unsigned int l=20,QWidget* parent = NULL):
            width(w),height(h),nbC(c),nbL(l),QTableWidget(parent)
    {
        init();
    }

    void init()
    {
        QString c[13]={"darkred","darkblue","darkgreen","darkcyan","darkmagenta","darkgray","red","blue","green","cyan","magenta","yellow"};
        colors.insert(colors.begin(),c, c+12);
        //on prend le minimum pour être sur que ça entre
        dCell = std::min(width/nbC,height/nbL);

        setColumnCount(nbC);
        setRowCount(nbL);
        setShowGrid(true);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        verticalHeader()->hide();
        horizontalHeader()->hide();

        for(unsigned int i=0 ; i<nbL; ++i ){
            for (unsigned int j = 0 ; j<nbC;++j){
                QTableWidgetItem* a = new QTableWidgetItem ("");
                a->setFlags(Qt::NoItemFlags);
                a->setFlags(Qt::ItemIsEnabled);
                setItem(i,j,a);
                item(i,j)->setBackgroundColor("gray");
                if(i==0)
                    setColumnWidth(j,(int)(dCell));


            }
            setRowHeight(i,dCell);
        }
        setFixedSize(width,height);

    }

    void initRobot(int id,int x, int y,int heading)
    {
        if(robots.find(id) == robots.end())
        {
            if(colors.size()==0){
                std::cerr<< "Il est impossible d'ajouter un nouveau Robot !\n"<< std::endl;
            }
            else{
                robots[id].x = x;
                robots[id].y = y;
                robots[id].heading = heading;
                robots[id].color= colors.at(colors.size()-1);
                colors.pop_back();
                item(robots[id].x,robots[id].y)->setBackgroundColor(robots[id].color);
            }

        }
    }

    //déplacement uniquement suivant une grille
    void move(int id,int d)
    {
        if(robots[id].heading == 180 || robots[id].heading == 0 )   //déplacement suivant une ligne
        {
            for(int i = std::min(robots[id].y+d,robots[id].y); i<std::max(robots[id].y+d,robots[id].y); ++i)
                item(robots[id].x,i)->setBackgroundColor("white");
             robots[id].y += d;
        }
        else if(robots[id].heading == 90 || robots[id].heading == 270 ) // déplacement suivant une colonne
        {
            for(int i = std::min(robots[id].x+d,robots[id].x); i<std::max(robots[id].x+d,robots[id].x); ++i)
                item(i,robots[id].y)->setBackgroundColor("white");
            robots[id].x += d;
        }
        item(robots[id].x,robots[id].y)->setBackgroundColor(robots[id].color);

    }

    void show()
    {
        QTableWidget::show();
    }

    void turn(int id,int angle)
    {
        if(angle >0)
            robots[id].heading = (robots[id].heading +angle)%360;
    }
};

#endif // PIL_H
