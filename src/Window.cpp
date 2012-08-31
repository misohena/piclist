#include <windows.h>
#include <tchar.h>
#include "Window.h"


namespace piclist{

Window::Window()
	: hwnd_(NULL)
	, className_(_T("PicListWindowClass"))
	, vScrollSmall_(1)
	, hScrollSmall_(1)
	, vScrollLarge_(10)
	, hScrollLarge_(10)
{
}

Window::~Window()
{
}

bool Window::create()
{
	if(hwnd_){
		return false; //already create.
	}
	::HINSTANCE hInstance = ::GetModuleHandle(NULL);

	::WNDCLASSEX wc = {sizeof(wc)};
	wc.lpfnWndProc = &wndprocStatic;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = className_.c_str();
	if(!::RegisterClassEx(&wc)){
		return false; //RegisterClassEx failed.
	}

	HWND hwnd = ::CreateWindow(className_.c_str(), _T(""), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		::GetDesktopWindow(),
		NULL,
		hInstance,
		this);
	if(!hwnd){
		hwnd_ = NULL;
		return false; //CreateWindow failed.
	}
	//hwnd_ = hwnd; WM_CREATE時にセットされる。

	return true;
}

LRESULT CALLBACK Window::wndprocStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if(msg == WM_CREATE){
		::SetWindowLongPtr(hwnd, GWL_USERDATA,
			(LONG_PTR)(((CREATESTRUCT *)lparam)->lpCreateParams));
	}

	Window *pThis = (Window *)(LONG_PTR)::GetWindowLongPtr(hwnd, GWL_USERDATA);

	if(pThis != NULL){
		if(!pThis->hwnd_){
			pThis->hwnd_ = hwnd;
		}
		return pThis->wndproc(hwnd, msg, wparam, lparam);
	}
	else{
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

LRESULT Window::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg){
	case WM_CREATE:
		onCreate();
		break;
	case WM_PAINT:
		{
			::PAINTSTRUCT ps;
			HDC hdc = ::BeginPaint(hwnd, &ps);
			onPaint(hdc);
			::EndPaint(hwnd, &ps);
		}
		break;
	case WM_SIZING:
		{
			LPRECT pr = (LPRECT)lparam;
			Rect2i r(pr->left, pr->top, pr->right, pr->bottom);
			onSizing(wparam, r);
		}
		break;
	case WM_HSCROLL:
		{
			onHScroll(LOWORD(wparam), HIWORD(wparam));
		}
		break;
	case WM_VSCROLL:
		{
			onVScroll(LOWORD(wparam), HIWORD(wparam));
		}
		break;
	case WM_MOUSEWHEEL:
		onMouseWheel((SHORT)HIWORD(wparam), LOWORD(wparam), LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

void Window::showWindow(int nCmdShow)
{
	if(hwnd_){
		::ShowWindow(hwnd_, nCmdShow);
	}
}

void Window::updateWindow()
{
	if(hwnd_){
		::UpdateWindow(hwnd_);
	}
}

void Window::updateStyle()
{
	if(hwnd_){
		::SetWindowPos(hwnd_,
			0, 0, 0, 0, 0, //z, x, y, w, h
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE  | SWP_FRAMECHANGED);
	}
}

Size2i Window::getClientSize() const
{
	RECT cr;
	if(hwnd_ != NULL && ::GetClientRect(hwnd_, &cr)){
		return Size2i(cr.right - cr.left, cr.bottom - cr.top);
	}
	else{
		return Size2i(0, 0);
	}
}

void Window::invalidate(void)
{
	if(hwnd_){
		::InvalidateRect(hwnd_, NULL, TRUE);
	}
}

/**
 * ウィンドウのクライアント領域の中身をスライドさせます。
 */
void Window::scrollWindowContent(int dx, int dy)
{
	if(hwnd_){
		::ScrollWindow(hwnd_, dx, dy, NULL, NULL);
	}
}


// --------------------------------------------------------------------------
// ScrollBar
// --------------------------------------------------------------------------

/**
 * スクロールバーを表示するかどうかを設定します。
 */
void Window::setScrollBarVisible(int bar, bool visible)
{
	if(hwnd_){
		::ShowScrollBar(hwnd_, bar, visible ? TRUE : FALSE);
	}
}
void Window::setHScrollBarVisible(bool visible){setScrollBarVisible(SB_HORZ, visible);}
void Window::setVScrollBarVisible(bool visible){setScrollBarVisible(SB_VERT, visible);}

/**
 * スクロールバーの位置の有効範囲を設定します。
 */
void Window::setScrollRange(int bar, int minValue, int maxValue)
{
	if(hwnd_){
		::SetScrollRange(hwnd_, bar, minValue, maxValue, TRUE);
	}
}
void Window::setHScrollRange(int minValue, int maxValue){setScrollRange(SB_HORZ, minValue, maxValue);}
void Window::setVScrollRange(int minValue, int maxValue){setScrollRange(SB_VERT, minValue, maxValue);}

/**
 * スクロールバーの位置を設定します。
 *
 * スクロールバーの位置が変化したとき、onHScrollPositionChangedまたはonVScrollPositionChangedを呼び出します。
 */
void Window::setScrollPosition(int bar, int value)
{
	if(hwnd_){
		const int old = getScrollPosition(bar);
		int minPos, maxPos;
		if(::GetScrollRange(hwnd_, bar, &minPos, &maxPos)){
			maxPos -= getScrollVisibleAmount(bar);
			if(value < minPos){
				value = minPos;
			}
			if(value > maxPos){
				value = maxPos;
			}
		}
		if(value != old){
			::SetScrollPos(hwnd_, bar, value, TRUE);

			if(bar == SB_HORZ){
				onHScrollPositionChanged(old, value);
			}
			else if(bar == SB_VERT){
				onVScrollPositionChanged(old, value);
			}
		}
	}
}
void Window::setHScrollPosition(int value){setScrollPosition(SB_HORZ, value);}
void Window::setVScrollPosition(int value){setScrollPosition(SB_VERT, value);}

/**
 * スクロールバーのつまみの長さ(見えている範囲を表す量)を設定します。
 */
void Window::setScrollVisibleAmount(int bar, int value)
{
	if(hwnd_){
		SCROLLINFO si = {sizeof(si)};
		si.fMask = SIF_PAGE;
		si.nPage = value;
		::SetScrollInfo(hwnd_, bar, &si, TRUE);
	}
}
void Window::setHScrollVisibleAmount(int value){setScrollVisibleAmount(SB_HORZ, value);}
void Window::setVScrollVisibleAmount(int value){setScrollVisibleAmount(SB_VERT, value);}

/**
 * 指定された量だけスクロールバーの位置を変化させます。
 */
void Window::scroll(int bar, int delta)
{
	setScrollPosition(bar, getVScrollPosition() + delta);
}
void Window::scrollH(int delta){scroll(SB_HORZ, delta);}
void Window::scrollV(int delta){scroll(SB_VERT, delta);}

/**
 * 現在のスクロールバーの位置を返します。
 */
int Window::getScrollPosition(int bar) const
{
	if(hwnd_){
		return ::GetScrollPos(hwnd_, bar);
	}
	else{
		return 0;
	}
}
int Window::getHScrollPosition() const{return getScrollPosition(SB_HORZ);}
int Window::getVScrollPosition() const{return getScrollPosition(SB_VERT);}

/**
 * スクロールバーのつまみの長さ(見えている範囲を表す量)を返します。
 */
int Window::getScrollVisibleAmount(int bar) const
{
	if(hwnd_){
		SCROLLINFO si = {sizeof(si)};
		si.fMask = SIF_PAGE;
		if(::GetScrollInfo(hwnd_, bar, &si)){
			return si.nPage;
		}
	}
	return 0;
}
int Window::getHScrollVisibleAmount() const{return getScrollVisibleAmount(SB_HORZ);}
int Window::getVScrollVisibleAmount() const{return getScrollVisibleAmount(SB_VERT);}

/**
 * WM_VSCROLLやWM_HSCROLLのデフォルト実装です。
 *
 * Window::setScrollPositionを呼び出してスクロールバーの位置を変更します。
 *
 * Window::setScrollPositionはスクロールバーの位置を変更した後、
 * Window::onHScrollPositionChangedまたはWindow::onVScrollPositionChangedを
 * 呼び出します。スクロールバーの位置が変わったときの処理を行いたい場合は、
 * それらの関数をオーバーライドしてください。
 */
void Window::onScrollDefault(int bar, int action, int pos)
{
	switch(action){
	case SB_LINEUP:
		scroll(bar, -getVScrollSmallAmount());
		break;
	case SB_LINEDOWN:
		scroll(bar, getVScrollSmallAmount());
		break;
	case SB_PAGEUP:
		scroll(bar, -getVScrollLargeAmount());
		break;
	case SB_PAGEDOWN:
		scroll(bar, getVScrollLargeAmount());
		break;
	case SB_THUMBPOSITION:
		setScrollPosition(bar, pos);
		break;
	case SB_THUMBTRACK:
		setScrollPosition(bar, pos);
		break;
	}
}


// --------------------------------------------------------------------------
// Default Window Message Handlers.
// --------------------------------------------------------------------------


void Window::onCreate(void){}
void Window::onPaint(HDC hdc){}
void Window::onSizing(int edge, const Rect2i &rect){}
void Window::onVScroll(int action, int pos)
{
	onScrollDefault(SB_VERT, action, pos);
}
void Window::onHScroll(int action, int pos)
{
	onScrollDefault(SB_HORZ, action, pos);
}
void Window::onVScrollPositionChanged(int oldPos, int newPos){}
void Window::onHScrollPositionChanged(int oldPos, int newPos){}
void Window::onMouseWheel(int delta, unsigned int keys, int x, int y){}

}//namespace piclist
