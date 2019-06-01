#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H
#include <QString>

class MessageManager
{
public:
    //return the value linked to a mnemonic
    static QString getAttributeValue(QString mnemo, QString message);
    MessageManager();
};

#endif // MESSAGEMANAGER_H
