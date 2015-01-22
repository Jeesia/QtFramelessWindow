#ifndef UI_FRAMELESS_H
#define UI_FRAMELESS_H

#include <QtGui>
#include <QDialog>
#include <QApplication>

#ifdef Q_OS_WIN
#include <minmax.h>
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
	explicit FramelessWindow(QWidget *parent = 0, Qt::WindowFlags f = 0)
		: WindowType(parent, f)
	{
		resize(100, 30);
		toggle_borderless();
	}

#if(WINVER >= 0x0600) && (QT_VERSION < 0x050000)
	void extendFrameIntoClientArea(int left, int top, int right, int bottom);
	void extendFrameIntoClientArea(const QMargins & margins);
#endif

protected:
#if QT_VERSION >= 0x050000
	virtual bool nativeEvent(const QByteArray &eventType, void * message, long * result) Q_DECL_OVERRIDE
	{ return handleNativeEvent((MSG *)message, result) || WindowType::nativeEvent(eventType, message, result); }
#else
	virtual bool winEvent(MSG *message, long *result) override
	{	return handleNativeEvent(message, result) || WindowType::winEvent(message, result);	}
#endif

	bool handleNativeEvent(MSG *message, long *result);
	void toggle_borderless();

private:
	bool handleHitTest(MSG *msg, long *result);

};


#if(WINVER >= 0x0600) && (QT_VERSION < 0x050000)
template <typename WindowType>
void FramelessWindow<WindowType>::extendFrameIntoClientArea(int left, int top, int right, int bottom)
{
	extendFrameIntoClientArea(QMargins(left, top, right, bottom));
}

template <typename WindowType>
void FramelessWindow<WindowType>::extendFrameIntoClientArea(const QMargins & margins)
{
	MARGINS shadow;
	shadow.cxLeftWidth = margins.left();
	shadow.cxRightWidth = margins.right();
	shadow.cyBottomHeight = margins.bottom();
	shadow.cyTopHeight = margins.top();
	DwmExtendFrameIntoClientArea((HWND)winId(), &shadow);
}
#endif

template <typename WindowType>
void FramelessWindow<WindowType>::toggle_borderless()
{
	HWND hwnd = (HWND)winId();
	//see also: http://tunps.com/ws_caption-cause-duilib-program-title-bar-cannot-hide
	//          http://bbs.csdn.net/topics/90193232
	SetWindowLong(hwnd, GWL_STYLE,
		GetWindowLong(hwnd, GWL_STYLE) & ~WS_SYSMENU);	//WS_CAPTION 

	//redraw frame
	SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
}

template <typename WindowType>
bool FramelessWindow<WindowType>::handleNativeEvent(MSG *msg, long *result)
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
						 if (dynamic_cast<QDialog*>(this) == NULL){
							 if (handleHitTest(msg, result))
								 return true;
						 }

						 QWidget *action = QApplication::widgetAt(QCursor::pos());
						 if (action == this){
							 //*result = HTCLIENT;
							 *result = HTCAPTION;
							 return true;
						 }
						 break;
	} //end case WM_NCHITTEST
	}

	return false;
}

template <typename WindowType>
bool FramelessWindow<WindowType>::handleHitTest(MSG *msg, long *result)
{
	// const int border_width = 7; //in pixels
	const LONG border_width = 8; //in pixels
	RECT winrect;
	GetWindowRect((HWND)winId(), &winrect);

	const long x = GET_X_LPARAM(msg->lParam);
	const long y = GET_Y_LPARAM(msg->lParam);
	const long leftRange = winrect.left + border_width;
	const long rightRange = winrect.right - border_width;
	const long topRange = winrect.top + border_width;
	const long bottomRange = winrect.bottom - border_width;

	bool fixedWidth = minimumWidth() == maximumWidth();
	bool fixedHeight = minimumHeight() == maximumHeight();
	if (!fixedWidth && !fixedHeight)
	{
		//bottom left corner
		if (x >= winrect.left && x < leftRange &&
			y < winrect.bottom && y >= bottomRange)
		{
			*result = HTBOTTOMLEFT;
			return true;
		}
		//bottom right corner
		if (x < winrect.right && x >= rightRange &&
			y < winrect.bottom && y >= bottomRange)
		{
			*result = HTBOTTOMRIGHT;
			return true;
		}
		//top left corner
		if (x >= winrect.left && x < leftRange &&
			y >= winrect.top && y < topRange)
		{
			*result = HTTOPLEFT;
			return true;
		}
		//top right corner
		if (x < winrect.right && x >= rightRange &&
			y >= winrect.top && y < topRange)
		{
			*result = HTTOPRIGHT;
			return true;
		}
	}
	if (!fixedWidth)
	{
		//left border
		if (x >= winrect.left && x < leftRange)
		{
			*result = HTLEFT;
			return true;
		}
		//right border
		if (x < winrect.right && x >= rightRange)
		{
			*result = HTRIGHT;
			return true;
		}
	}
	if (!fixedHeight)
	{
		//bottom border
		if (y < winrect.bottom && y >= bottomRange)
		{
			*result = HTBOTTOM;
			return true;
		}
		//top border
		if (y >= winrect.top && y < topRange)
		{
			*result = HTTOP;
			return true;
		}
	}
	return false;
}

#endif //FRAMELESSWINDOW_H
