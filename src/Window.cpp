#include <windows.h>
#include <tchar.h>
#include "Window.h"


namespace piclist{

Window::Window()
	: hwnd_(NULL)
	, className_(_T("PicListWindowClass"))
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

void Window::setScrollBarVisible(int bar, bool visible)
{
	if(hwnd_){
		::ShowScrollBar(hwnd_, bar, visible ? TRUE : FALSE);
	}
}
void Window::setHScrollBarVisible(bool visible)
{
	setScrollBarVisible(SB_HORZ, visible);
}
void Window::setVScrollBarVisible(bool visible)
{
	setScrollBarVisible(SB_VERT, visible);
}

void Window::setScrollRange(int bar, int minValue, int maxValue)
{
	if(hwnd_){
		::SetScrollRange(hwnd_, bar, minValue, maxValue, TRUE);
	}
}

void Window::setHScrollRange(int minValue, int maxValue)
{
	setScrollRange(SB_HORZ, minValue, maxValue);
}

void Window::setVScrollRange(int minValue, int maxValue)
{
	setScrollRange(SB_VERT, minValue, maxValue);
}

void Window::setScrollPosition(int bar, int value)
{
	if(hwnd_){
		::SetScrollPos(hwnd_, bar, value, TRUE);
	}
}

void Window::setHScrollPosition(int value)
{
	setScrollPosition(SB_HORZ, value);
}

void Window::setVScrollPosition(int value)
{
	setScrollPosition(SB_VERT, value);
}

void Window::setScrollVisibleAmount(int bar, int value)
{
	if(hwnd_){
		SCROLLINFO si = {sizeof(si)};
		si.fMask = SIF_PAGE;
		si.nPage = value;
		::SetScrollInfo(hwnd_, bar, &si, TRUE);
	}
}

void Window::setHScrollVisibleAmount(int value)
{
	setScrollVisibleAmount(SB_HORZ, value);
}

void Window::setVScrollVisibleAmount(int value)
{
	setScrollVisibleAmount(SB_VERT, value);
}

void Window::onCreate(void){}
void Window::onPaint(HDC hdc){}
void Window::onSizing(int edge, const Rect2i &rect){}

}//namespace piclist
