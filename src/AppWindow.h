#ifndef APPWINDOW_H
#define APPWINDOW_H

#include "Window.h"
#include "Picture.h"
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
		PictureContainer pictures_;
		ImageCache imageCache_;
		Layout layout_;

	public:
		AppWindow();
		virtual ~AppWindow();
	private:
		virtual void onCreate();
		virtual void onPaint(HDC hdc);
		virtual void onSizing(int edge, const Rect2i &rect);
	};

}//namespace piclist
#endif
