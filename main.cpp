﻿#include <QApplication>
#include "SelectionWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SelectionWindow window;
    window.show();
    return app.exec();
}