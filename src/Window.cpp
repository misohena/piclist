#include <windows.h>
#include <tchar.h>
#include "Window.h"


namespace piclist{

Window::Window(const String &className)
	: hwnd_(NULL)
	, className_(className)
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
	//hwnd_ = hwnd; WM_CREATE時にセットされる。でないと、onCreateの中でgetClientRect等を呼び出せない。

	return true;
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

/**
 * ウィンドウのキャプション文字列を変更します。
 */
void Window::setCaption(const String &str)
{
	if(hwnd_){
		::SetWindowText(hwnd_, str.c_str());
	}
}

/**
 * ウィンドウのサイズを変更します。
 */
void Window::setWindowSize(int width, int height)
{
	if(hwnd_){
		::SetWindowPos(hwnd_, 0, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
	}
}

/**
 * ウィンドウの位置とサイズを変更します。
 */
void Window::setWindowRect(const Rect2i rect)
{
	if(hwnd_){
		::SetWindowPos(hwnd_, 0, rect.left, rect.top, rect.getWidth(), rect.getHeight(), SWP_NOZORDER);
	}
}

/**
 * 最大化されているかどうかを返します。
 */
bool Window::isMaximized() const
{
	return hwnd_ ? (::IsZoomed(hwnd_) ? true : false) : false;
}

/**
 * 最小化されているかどうかを返します。
 */
bool Window::isMinimized() const
{
	return hwnd_ ? (::IsIconic(hwnd_) ? true : false) : false;
}

/**
 * ウィンドウの矩形を返します。
 */
Rect2i Window::getWindowRect() const
{
	if(hwnd_){
		RECT r;
		if(::GetWindowRect(hwnd_, &r)){
			return Rect2i(r.left, r.top, r.right, r.bottom);
		}
	}
	return Rect2i(0, 0, 0, 0);
}

/**
 * ウィンドウのクライアント領域のサイズを返します。
 */
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

/**
 * クライアント領域座標系の点をスクリーン座標系へ変換します。
 */
Point2i Window::clientToScreen(const Point2i &p) const
{
	POINT pt = {p.x, p.y};
	if(::ClientToScreen(hwnd_, &pt)){
		return Point2i(pt.x, pt.y);
	}
	else{
		return Point2i();
	}
}

/**
 * ウィンドウを最前面に配置します。
 */
void Window::setZOrderTopMost()
{
	if(hwnd_){
		::SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

/**
 * ウィンドウを最前面に配置しないようにします。
 */
void Window::setZOrderNoTopMost()
{
	if(hwnd_){
		::SetWindowPos(hwnd_, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

/**
 * ウィンドウが最前面に配置されているならtrue、そうでないならfalseを返します。
 */
bool Window::isZOrderTopMost() const
{
	return hwnd_ && (::GetWindowLong(hwnd_, GWL_EXSTYLE) & WS_EX_TOPMOST);
}


/**
 * ウィンドウのクライアント領域内を無効化して再描画を促します。
 */
void Window::invalidate()
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

static const TCHAR *REG_VALUE_WINDOW_PLACEMENT = _T("WindowPlacement");

/**
 * ウィンドウの状態をレジストリに待避します。
 */
bool Window::backupWindowPlacement(const String &keyname)
{
	if(!hwnd_){
		return false;
	}
	WINDOWPLACEMENT wp;
	if(!::GetWindowPlacement(hwnd_, &wp)){
		return false;
	}
	if(isMaximized()){
		wp.flags = WPF_RESTORETOMAXIMIZED;
	}

	HKEY key;
	DWORD disposition;
	const LONG result = ::RegCreateKeyEx(
		HKEY_CURRENT_USER, keyname.c_str(), NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disposition);
	if(result != ERROR_SUCCESS){
		return false;
	}
	::RegSetValueEx(key, REG_VALUE_WINDOW_PLACEMENT, NULL, REG_BINARY, (BYTE *)&wp, sizeof(wp));
	::RegCloseKey(key);
	return true;
}

/**
 * ウィンドウの状態をレジストリから復元します。
 */
bool Window::restoreWindowPlacement(const String &keyname)
{
	if(!hwnd_){
		return false;
	}
	HKEY key;
	LONG result;
	result = ::RegOpenKeyEx(
		HKEY_CURRENT_USER, keyname.c_str(), NULL, KEY_ALL_ACCESS, &key);
	if(result != ERROR_SUCCESS){
		return false;
	}
	WINDOWPLACEMENT wp;
	DWORD type;
	DWORD size = sizeof(wp);
	result = ::RegQueryValueEx(key, REG_VALUE_WINDOW_PLACEMENT, NULL, &type, (BYTE *)&wp, &size);
	if(!(result == ERROR_SUCCESS && type == REG_BINARY && size == sizeof(wp) && wp.length == sizeof(wp))){
		return false;
	}
	::SetWindowPlacement(hwnd_, &wp);
	return true;
}



void Window::dragAcceptFiles(bool accept)
{
	if(hwnd_){
		::DragAcceptFiles(hwnd_, accept ? TRUE : FALSE);
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
			if(maxPos < minPos){
				maxPos = minPos;
			}
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
// Window Message Handlers.
// --------------------------------------------------------------------------

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
	case WM_SIZE:
		onSize(wparam, LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_HSCROLL:
		onHScroll(LOWORD(wparam), HIWORD(wparam));
		break;
	case WM_VSCROLL:
		onVScroll(LOWORD(wparam), HIWORD(wparam));
		break;
	case WM_MOUSEWHEEL:
		onMouseWheel((SHORT)HIWORD(wparam), LOWORD(wparam), LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_LBUTTONDOWN:
		onLButtonDown(wparam, LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_LBUTTONUP:
		onLButtonUp(wparam, LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_RBUTTONDOWN:
		onRButtonDown(wparam, LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_RBUTTONUP:
		onRButtonUp(wparam, LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_COMMAND:
		onCommand(HIWORD(wparam), LOWORD(wparam), (HWND)lparam);
		break;
	case WM_COPYDATA:
		{
			COPYDATASTRUCT *cd = (COPYDATASTRUCT *)lparam;
			onCopyData((HWND)wparam, cd->dwData, cd->cbData, cd->lpData);
		}
		break;
	case WM_DROPFILES:
		onDropFiles((HDROP)wparam);
		::DragFinish((HDROP)wparam);
		break;
	case WM_DESTROY:
		onDestroy();
		PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

void Window::onCreate(void){}
void Window::onPaint(HDC hdc){}
void Window::onSizing(int edge, const Rect2i &rect){}
void Window::onSize(int cause, int newClientWidth, int newClientHeight){}
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
void Window::onLButtonDown(unsigned int keys, int x, int y){}
void Window::onLButtonUp(unsigned int keys, int x, int y){}
void Window::onRButtonDown(unsigned int keys, int x, int y){}
void Window::onRButtonUp(unsigned int keys, int x, int y){}
void Window::onCommand(int notificationCode, int id, HWND hWndControl){}
void Window::onCopyData(HWND srcwnd, ULONG_PTR dwData, DWORD cbData, PVOID lpData){}
void Window::onDropFiles(HDROP hDrop){}
void Window::onDestroy(){}

}//namespace piclist
