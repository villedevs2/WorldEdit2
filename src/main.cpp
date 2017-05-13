#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

	w.resize(800, 600);
	w.showFullScreen();
	w.showMaximized();
	w.show();

	w.newDocument();

    return a.exec();
}
