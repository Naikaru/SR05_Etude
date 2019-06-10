#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#define SEPMNEMVAL '~'

//Class which handles a message.
class Message
{
private:
    static const QString sepMnemVal;
    static const QString mainSepVal;
    static const QString sepOrderMnem ;
    static const QString sepOrderValueMnem;

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

    QString getValue(QString mnemo) const;
    Message(QString receivedMessage);
    Message(QString _APP, QString _WHO, QString _WHE);
    QString getWHO(){return WHO;}
    QString getWHE(){return WHE;}
    QString getAPP(){return APP;}
    QString getCompleteMessage() const;


    static const QString mnemoRobotOrder;
    static const QString mnemoRobotAck;
    static const QString mnemoAckMove;
    static const QString mnemoAckTurn;
    static const QString mnemoAckError;
    static std::vector<int> getOrderValue(const QString& order);
    static QString parseOrderValues(const QString& order, const std::vector<int>& values);

};

#endif // MESSAGE_H
