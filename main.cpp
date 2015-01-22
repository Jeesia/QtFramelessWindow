#include "test_window.h"
#include <QApplication>

#if (QT_VERSION >= 0x050000)
#include <QtWinExtras>
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TestWindow w;
	w.show();

#if (QT_VERSION < 0x050000)
	w.extendFrameIntoClientArea( -1, -1, -1, -1);
#else
	QtWin::extendFrameIntoClientArea(&w, -1, -1, -1, -1);
#endif
	return a.exec();
}
