#include "pil.h"

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
    Pil pil(argc, argv);
    pil.show();

    return app.exec();
}
