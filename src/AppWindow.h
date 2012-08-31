#ifndef APPWINDOW_H
#define APPWINDOW_H

#include "Window.h"
#include "Picture.h"
#include "ImageCache.h"
#include "Layout.h"

namespace piclist{

	/**
	 * �A�v���P�[�V�������\����E�B���h�E�ł��B
	 *
	 * �A�v���P�[�V�����J�n���ɐ�������A�A�v���P�[�V�����I�����ɔj������܂��B
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
