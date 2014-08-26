#ifndef UI_FRAMELESS_H
#define UI_FRAMELESS_H

#include <QtGui>

#ifdef Q_OS_WIN
#include <windows.h>
#include <WinUser.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <GdiPlusColor.h>
#if(WINVER >= 0x0600)
#include <dwmapi.h>
#endif
#endif //Q_OS_WIN

// Example usage:
//		class MyWindow : public FramelessWindow<QWidget>
//
template <typename WindowType>
class FramelessWindow : public WindowType
{	
public:
	explicit FramelessWindow(QWidget *parent = 0, Qt::WindowFlags f = 0 )
		: WindowType(parent,f),
		m_firstShow(true)
	{
		resize(100,30);	
	}

protected:
	virtual void showEvent(QShowEvent *event); 

#ifdef Q_OS_WIN
	void windowBorderless();
	virtual bool winEvent(MSG *message, long *result);
#endif

private:
#ifdef Q_OS_WIN
	void borderHitTest(MSG *msg, long *result);
#endif

	bool m_firstShow;
};

template <typename WindowType>
void FramelessWindow<WindowType>::showEvent(QShowEvent *event)
{
#ifdef Q_OS_WIN
	if(m_firstShow){
		m_firstShow = false;
		windowBorderless();
	}
#endif
	WindowType::showEvent(event);
}

#ifdef Q_OS_WIN

template <typename WindowType>
void FramelessWindow<WindowType>::windowBorderless()
{
	if (isVisible())
	{
		//see also: http://tunps.com/ws_caption-cause-duilib-program-title-bar-cannot-hide
		//          http://bbs.csdn.net/topics/90193232
		SetWindowLong(winId(), GWL_STYLE,
			GetWindowLong(winId(), GWL_STYLE) & ~WS_SYSMENU);	//WS_CAPTION

#if(WINVER >= 0x0600)
		const MARGINS shadow_on = { 1, 1, 1, 1 };
		DwmExtendFrameIntoClientArea(winId(), &shadow_on);
#endif 
		SetWindowPos(winId(), NULL, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW |SWP_NOZORDER);
	}
}

template <typename WindowType>
bool FramelessWindow<WindowType>::winEvent(MSG *msg, long *result)
{
	//see also: https://github.com/melak47/BorderlessWindow
	//known Issues: QTBUG-30085
	//qt5 Issues: QTBUG-40578
	switch (msg->message)
	{
		/*
		case WM_LBUTTONDOWN:{
		QWidget *action = QApplication::widgetAt(QCursor::pos());
		if(action == this && ReleaseCapture()){
		SendMessage(winId(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION , 0);
		// SendMessage(winId(),WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(msg->pt.x, msg->pt.y));
		return true;
		}
		break;
		}
		case WM_LBUTTONDBLCLK:{
		QWidget *action = QApplication::widgetAt(QCursor::pos());
		if(action == this){
		if(isMaximized())
		showNormal();
		else
		showMaximized();
		return true;
		}
		break;
		}
		*/
	case WM_NCCALCSIZE:
		{
			//this kills the window frame and title bar we added with
			//WS_THICKFRAME and WS_CAPTION
			*result = 0;
			return true;
			break;
		}
	case WM_NCHITTEST:
		{
			*result = 0;

			if(dynamic_cast<QDialog*>(this) == NULL){
				borderHitTest(msg, result);
				if(*result != 0)
					return true;
			}

			QWidget *action = QApplication::widgetAt(QCursor::pos());
			if(action == this){
				//*result = HTCLIENT;
				*result = HTCAPTION;
				return true;
			}
			break;
		} //end case WM_NCHITTEST
	}

	return QWidget::winEvent(msg,result);
}

template <typename WindowType>
void FramelessWindow<WindowType>::borderHitTest(MSG *msg, long *result)
{
	const int border_width = 7; //in pixels
	RECT winrect;
	GetWindowRect(winId(), &winrect);

	const long x = GET_X_LPARAM(msg->lParam);
	const long y = GET_Y_LPARAM(msg->lParam);
	const long leftRange = winrect.left + border_width;
	const long rightRange = winrect.right - border_width;
	const long topRange = winrect.top + border_width;
	const long bottomRange = winrect.bottom - border_width;

	bool fixedWidth = minimumWidth() == maximumWidth();
	bool fixedHeight = minimumHeight() == maximumHeight();

	if(!fixedWidth)
	{
		//left border
		if (x >= winrect.left && x < leftRange)
		{
			*result = HTLEFT;
		}
		//right border
		if (x < winrect.right && x >= rightRange)
		{
			*result = HTRIGHT;
		}
	}
	if(!fixedHeight)
	{
		//bottom border
		if (y < winrect.bottom && y >= bottomRange)
		{
			*result = HTBOTTOM;
		}
		//top border
		if (y >= winrect.top && y < topRange)
		{
			*result = HTTOP;
		}
	}
	if(!fixedWidth && !fixedHeight)
	{
		//bottom left corner
		if (x >= winrect.left && x < leftRange &&
			y < winrect.bottom && y >= bottomRange)
		{
			*result = HTBOTTOMLEFT;
		}
		//bottom right corner
		if (x < winrect.right && x >= rightRange &&
			y < winrect.bottom && y >= bottomRange)
		{
			*result = HTBOTTOMRIGHT;
		}
		//top left corner
		if (x >= winrect.left && x < leftRange &&
			y >= winrect.top && y < topRange )
		{
			*result = HTTOPLEFT;
		}
		//top right corner
		if (x < winrect.right && x >= rightRange &&
			y >= winrect.top && y < topRange )
		{
			*result = HTTOPRIGHT;
		}
	}
}

#endif //end Q_OS_WIN

#endif //FRAMELESSWINDOW_H
