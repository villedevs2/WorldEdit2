#include <QApplication>
#include <QPushButton>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	app.setApplicationName("WorldEdit");
	app.setOrganizationName("vlinde");
	app.setOrganizationDomain("vlinde.com");

	MainWindow *window = new MainWindow();

	window->resize(800, 600);
	window->showFullScreen();
	window->showMaximized();
	window->show();

	window->newDocument();

    return app.exec();
}