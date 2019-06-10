#include "message.h"

const QString Message::mnemoRobotOrder = "robord";
const QString Message::mnemoRobotAck = "roback";
// Create a Message object from a received string.
Message::Message(QString receivedMessage)
{
    parseMessage(receivedMessage);
}

// Create  a new message from scratch
Message::Message(QString _APP, QString _WHO, QString _WHE)
{
    APP = _APP;
    WHO = _WHO;
    WHE = _WHE;
}


// Create a message object from a received messages
void Message::parseMessage(QString message){
    APP = message.left(3);
    WHO = message.mid(3,3);
    WHE = message.mid(6,3);
    message = message.right(message.length()-9);
    couples.clear();
    // Separator between couples is the first charactere of the messages
    QStringList listMnemoVal = message.split(mainSepVal,QString::SkipEmptyParts);
    foreach (const QString &str, listMnemoVal) {
       QStringList tmp = str.split(sepMnemVal);
       if(tmp.size() == 2){
           couples.insert(tmp[0],tmp[1]);
       }else{
           qDebug() << QString("ERROR - malformed message");
       }
    }
}


void Message::setValue(QString mnemo, QString value){
    couples.insert(mnemo,value);
}
QString Message::getValue(QString mnemo) const{
    return couples.value(mnemo);
}

QString Message::getCompleteMessage() const{
    QString mess = APP+WHO+WHE;
    for (auto key : couples.keys()){
        mess += mainSepVal + key + sepMnemVal + couples.value(key);
    }
    return mess;
}

QString Message::getOrderValue(const QString &order)
{
    int index = order.indexOf(":");
    return (index != - 1) ? order.mid(index + 1) : QString("");
}


