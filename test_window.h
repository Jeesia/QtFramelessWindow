#ifndef TEST_WINDOW_H
#define TEST_WINDOW_H

#include <QtGui/QWidget>
#include "frameless_window.h"

class TestWindow : public FramelessWindow<QWidget>
{
	Q_OBJECT

public:
	TestWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~TestWindow();

private:
};

#endif // TEST_WINDOW_H
