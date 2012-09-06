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

		int vScrollSmall_;
		int hScrollSmall_;
		int vScrollLarge_;
		int hScrollLarge_;
	public:
		explicit Window(const String &className);
		virtual ~Window();
		bool create();

		HWND getWindowHandle() const { return hwnd_;}

		void showWindow(int nCmdShow);
		void updateWindow();
		void updateStyle();

		void setCaption(const String &str);
		void setWindowSize(int width, int height);
		void setWindowRect(const Rect2i rect);

		bool isMaximized() const;
		bool isMinimized() const;
		Rect2i getWindowRect() const;
		Size2i getClientSize() const;

		void setZOrderTopMost();
		void setZOrderNoTopMost();
		bool isZOrderTopMost() const;

		Point2i clientToScreen(const Point2i &p) const;

		void invalidate();
		void scrollWindowContent(int dx, int dy);

		bool backupWindowPlacement(const String &keyname);
		bool restoreWindowPlacement(const String &keyname);

		// ScrollBar

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

		void scroll(int bar, int delta);
		void scrollH(int delta);
		void scrollV(int delta);

		int getScrollPosition(int bar) const;
		int getHScrollPosition() const;
		int getVScrollPosition() const;

		int getScrollVisibleAmount(int bar) const;
		int getHScrollVisibleAmount() const;
		int getVScrollVisibleAmount() const;

		void setVScrollSmallAmount(int v) { vScrollSmall_ = v;}
		void setHScrollSmallAmount(int v) { hScrollSmall_ = v;}
		void setVScrollLargeAmount(int v) { vScrollLarge_ = v;}
		void setHScrollLargeAmount(int v) { hScrollLarge_ = v;}

		int getVScrollSmallAmount() const { return vScrollSmall_;}
		int getHScrollSmallAmount() const { return hScrollSmall_;}
		int getVScrollLargeAmount() const { return vScrollLarge_;}
		int getHScrollLargeAmount() const { return hScrollLarge_;}

		void onScrollDefault(int bar, int action, int pos);

		// Message Handlers
	private:
		static LRESULT CALLBACK wndprocStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	protected:
		virtual LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		virtual void onCreate(void);
		virtual void onPaint(HDC hdc);
		virtual void onSizing(int edge, const Rect2i &rect);
		virtual void onSize(int cause, int newClientWidth, int newClientHeight);
		virtual void onVScroll(int action, int pos);
		virtual void onHScroll(int action, int pos);
		virtual void onVScrollPositionChanged(int oldPos, int newPos);
		virtual void onHScrollPositionChanged(int oldPos, int newPos);
		virtual void onMouseWheel(int delta, unsigned int keys, int x, int y);
		virtual void onLButtonDown(unsigned int keys, int x, int y);
		virtual void onLButtonUp(unsigned int keys, int x, int y);
		virtual void onRButtonDown(unsigned int keys, int x, int y);
		virtual void onRButtonUp(unsigned int keys, int x, int y);
		virtual void onCommand(int notificationCode, int id, HWND hWndControl);
		virtual void onCopyData(HWND srcwnd, ULONG_PTR dwData, DWORD cbData, PVOID lpData);
		virtual void onDestroy();

	};

}//namespace piclist
#endif
