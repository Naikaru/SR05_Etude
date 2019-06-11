#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#define SEPMNEMVAL '~'

//app = PIL
//who = ROB
//whe = LCH

//Class which handles a message.
class Message
{
private:
    const QString sepMnemVal = "~";
    const QString mainSepVal = "/";
    //Sending APP
    QString APP;
    //Receiving app
    QString WHO;
    //Reveiving location
    QString WHE;
    //Mnemonics~values
    QMap<QString, QString> couples;


    //return the value linked to a mnemonic
    QString getAttributeValue(QString mnemo, QString message);
    void parseMessage(QString message);
public:

    void setValue(QString mnemo, QString value);
    QString getValue(QString mnemo);
    Message(QString receivedMessage);
    Message(QString _APP, QString _WHO, QString _WHE);
    QString getWHO(){return WHO;}
    QString getWHE(){return WHE;}
    QString getAPP(){return APP;}
    QString getCompleteMessage();

};

#endif // MESSAGE_H
