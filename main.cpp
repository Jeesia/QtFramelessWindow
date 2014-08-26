#include "test_window.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TestWindow w;
	w.show();
	return a.exec();
}
