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
		const String windowName_;
		PictureContainer pictures_;
		ImageCache imageCache_;
		Layout layout_;

	public:
		explicit AppWindow(const String &windowName);
		virtual ~AppWindow();

		bool restoreWindowPlacement();
		void addPictures(const PictureContainer &pics) { pictures_.insert(pictures_.end(), pics.begin(), pics.end());}
		void updateLayout();

	private:
		virtual void onCreate();
		virtual void onDestroy();
		virtual void onPaint(HDC hdc);
		virtual void onSizing(int edge, const Rect2i &rect);
		virtual void onSize(int cause, int newClientWidth, int newClientHeight);
		virtual void onVScrollPositionChanged(int oldPos, int newPos);
		virtual void onMouseWheel(int delta, unsigned int keys, int x, int y);

	};

}//namespace piclist
#endif
