#include "net.h"
#include <iostream>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Net net(argc, argv);
    net.show();

    return app.exec();
}
