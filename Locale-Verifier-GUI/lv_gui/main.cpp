#include <QtWidgets>
#include "dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Dialog ui;

    ui.show();

    return app.exec();
}
