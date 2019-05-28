#include "vectorclock.h"

std::map<int, unsigned int> qstringToVector(QString vector) {
    std::map<int, unsigned int> string_to_vector;
    QString delimiter = " ";
    QString separator = ":";
    QStringList tokens = vector.split(delimiter, QString::SkipEmptyParts);
    foreach (const QString &token, tokens) {
        QStringList tmp = token.split(separator);
        string_to_vector[tmp[0].toInt()] = tmp[1].toUInt();
    }
    return string_to_vector;
}

QString vectorToQString(std::map<int, unsigned int> vector) {
    QString vector_to_string;
    for (std::map<int, unsigned int>::iterator it=vector.begin(); it!=vector.end(); ++it) {
        vector_to_string += QString::number(it->first) + QString(":") + QString::number(it->second) + QString(" ");
    }
    return vector_to_string;
}


VectorClock::VectorClock(int id): ident(id) {
    clock[id] = 0;
}

std::map<int, unsigned int> VectorClock::incrAndGet() {
    clock[ident] += 1;
    return clock;
}

std::map<int, unsigned int> VectorClock::updateAndGet(std::map<int, unsigned int> foreignClock) {
    clock[ident] += 1;
    for (std::map<int, unsigned int>::iterator it=foreignClock.begin(); it!=foreignClock.end(); ++it) {
        if (it->first != ident) {
            if (clock.find(it->first) != clock.end()) {                
                clock[it->first] = (clock[it->first]<it->second)?it->second:clock[it->first];
            }
            else {
                clock[it->first] = it->second;
            }
        }
    }
    return clock;
}
