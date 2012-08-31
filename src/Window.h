#ifndef WINDOW_H
#define WINDOW_H

#include "String.h"
#include "Math.h"

namespace piclist{

	/**
	 * ウィンドウの基本クラスです。
	 * Win32のウィンドウに共通する事柄をまとめたクラスです。
	 */
	class Window
	{
		HWND hwnd_;
		String className_;
	public:
		Window();
		virtual ~Window();
		bool create();

		//HWND getWindowHandle() const { return hwnd_;}

		void showWindow(int nCmdShow);
		void updateWindow();
		void updateStyle();

		Size2i getClientSize() const;

		void invalidate(void);

		void setScrollBarVisible(int bar, bool visible);
		void setHScrollBarVisible(bool visible);
		void setVScrollBarVisible(bool visible);
		void setScrollRange(int bar, int minValue, int maxValue);
		void setHScrollRange(int minValue, int maxValue);
		void setVScrollRange(int minValue, int maxValue);
		void setScrollPosition(int bar, int value);
		void setHScrollPosition(int value);
		void setVScrollPosition(int value);
		void setScrollVisibleAmount(int bar, int value);
		void setHScrollVisibleAmount(int value);
		void setVScrollVisibleAmount(int value);


	private:
		static LRESULT CALLBACK wndprocStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	protected:
		virtual LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		virtual void onCreate(void);
		virtual void onPaint(HDC hdc);
		virtual void onSizing(int edge, const Rect2i &rect);
	};

}//namespace piclist
#endif
