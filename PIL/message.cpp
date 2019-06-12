#include "message.h"

const QString Message::sepMnemVal = "~";
const QString Message::mainSepVal = "/";
const QString Message::sepOrderMnem = ":";
const QString Message::sepOrderValueMnem = ",";

const QString Message::mnemoRobotOrder = "robord";
const QString Message::mnemoRobotAck = "roback";
const QString Message::mnemoAckMove = "moved";
const QString Message::mnemoAckTurn = "turned";
const QString Message::mnemoAckError = "order";
const QString Message::mnemoAckInit = "init";


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
//return the parameters of a robord
std::vector<int> Message::getOrderValue(const QString &order)
{
    std::vector<int> values = std::vector<int>();
    int startIndex;
    int endIndex;


    QString currentString = order;
    startIndex = currentString.indexOf(sepOrderMnem);
    qDebug() << "order : " << order << " sep :" << sepOrderMnem;
    qDebug() << "startIndex = " << startIndex;
    while(startIndex !=  -1){

        endIndex = order.indexOf(sepOrderValueMnem, startIndex);
        if(endIndex != -1){
            values.push_back(order.mid(startIndex + 1, endIndex - (startIndex + 1)).toInt());
            qDebug() << "valeur ajoutée :" << values.back();
            qDebug() << "index de fin :" <<endIndex;
            startIndex = endIndex + 1; // après la virgule
        } else{
            startIndex = -1;
        }

     }

    return values;
}

//parse the parameters of a robord (order:param1,param2)
QString Message::parseOrderValues(const QString &order, const std::vector<int> &values)
{
    QString result = order + sepOrderMnem;
    for(int value : values){
        qDebug() << "valeur : " << value;
        result += QString::number(value) + sepOrderValueMnem;
    }
    result = result.mid(0,result.lastIndexOf(sepOrderValueMnem));
    qDebug() << result;
    return result;

}


