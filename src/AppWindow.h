#ifndef APPWINDOW_H
#define APPWINDOW_H

#include "Window.h"
#include "Album.h"
#include "ImageCache.h"
#include "Layout.h"

namespace piclist{

	/**
	 * アプリケーションを代表するウィンドウです。
	 *
	 * アプリケーション開始時に生成され、アプリケーション終了時に破棄されます。
	 */
	class AppWindow : public Window
	{
		const String windowName_;
		AlbumItemContainer albumItems_;
		ImageCache imageCache_;
		Layout layout_;

	public:
		AppWindow(const String &className, const String &windowName);
		virtual ~AppWindow();

		bool restoreWindowPlacement();
		void setAlbum(const AlbumItemContainer &items) { albumItems_ = items; updateLayout();}
		void updateLayout();

	private:
		virtual void onCreate();
		virtual void onDestroy();
		virtual void onPaint(HDC hdc);
		virtual void onSizing(int edge, const Rect2i &rect);
		virtual void onSize(int cause, int newClientWidth, int newClientHeight);
		virtual void onVScrollPositionChanged(int oldPos, int newPos);
		virtual void onMouseWheel(int delta, unsigned int keys, int x, int y);
		virtual void onCopyData(HWND srcwnd, ULONG_PTR dwData, DWORD cbData, PVOID lpData);

	};

}//namespace piclist
#endif
