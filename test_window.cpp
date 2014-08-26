#include "test_window.h"
#include <QPushButton>

TestWindow::TestWindow(QWidget *parent, Qt::WFlags flags)
	: FramelessWindow<QWidget>(parent, flags)
{
	QPushButton *btnClose = new QPushButton("Close",this);
	connect(btnClose, SIGNAL(clicked()),SLOT(close()));
}

TestWindow::~TestWindow()
{

}
