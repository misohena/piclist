#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#include "File.h"
#include "AppWindow.h"


namespace piclist{

static const TCHAR * const REGISTRY_KEY = _T("SOFTWARE\\Misohena\\piclist\\");

AppWindow::AppWindow(const String &className, const String &windowName)
	: Window(className)
	, windowName_(windowName)
{
}

AppWindow::~AppWindow()
{
}

bool AppWindow::restoreWindowPlacement()
{
	return Window::restoreWindowPlacement(REGISTRY_KEY + windowName_);
}

void AppWindow::onCreate()
{
	updateLayout();
}

void AppWindow::onDestroy()
{
	backupWindowPlacement(REGISTRY_KEY + windowName_);
}

void AppWindow::onPaint(HDC hdc)
{
	const int scrollY = getVScrollPosition();

	const Size2i clientSize = getClientSize();

	Gdiplus::Graphics graphics(hdc);

	Gdiplus::Matrix mat;
	mat.Translate(0, static_cast<Gdiplus::REAL>(-scrollY));
	graphics.MultiplyTransform(&mat);

	Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 0, 0, 0));
	Gdiplus::FontFamily fontFamily(_T("MS GOTHIC"));
	Gdiplus::Font font(&fontFamily, static_cast<Gdiplus::REAL>(layout_.getNameHeight()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

	const std::size_t pictureCount = pictures_.size();
	for(std::size_t index = 0; index < pictureCount; ++index){

		const Picture &pic = pictures_[index];
		if(ImagePtr im = imageCache_.getImage(pic.getFilePath())){
			const double srcW = im->getWidth();
			const double srcH = im->getHeight();
			const Rect2i cellImageRect = layout_.getImageRect(index);
			const double scale = min(cellImageRect.getWidth() / srcW, cellImageRect.getHeight() / srcH);
			const int dstW = static_cast<int>(srcW * scale);
			const int dstH = static_cast<int>(srcH * scale);
			graphics.DrawImage(im->getGdiPlusImage(), cellImageRect.left, cellImageRect.top, dstW, dstH);
		}

		const Rect2i cellNameRect = layout_.getNameRect(index);
		Gdiplus::PointF namePos(static_cast<Gdiplus::REAL>(cellNameRect.left), static_cast<Gdiplus::REAL>(cellNameRect.top));
		graphics.DrawString(pic.getFileNameBase().c_str(), -1, &font, namePos, &textBrush);
	}
}


void AppWindow::onSizing(int edge, const Rect2i &)
{
	updateLayout();
}

void AppWindow::onSize(int cause, int newClientWidth, int newClientHeight)
{
	updateLayout();
}

void AppWindow::onVScrollPositionChanged(int oldPos, int newPos)
{
	if(oldPos != newPos){
		scrollWindowContent(0, oldPos - newPos);
		updateWindow();
	}
}

void AppWindow::onMouseWheel(int delta, unsigned int keys, int x, int y)
{
	scrollV(-100 * delta/WHEEL_DELTA);
}

void AppWindow::updateLayout(void)
{
	const Size2i clientSize1 = getClientSize();
	layout_.update(pictures_, clientSize1);

	setVScrollBarVisible(layout_.getPageSize().h > clientSize1.h);

	const Size2i clientSize2 = getClientSize();
	layout_.update(pictures_, clientSize2);

	setVScrollRange(0, layout_.getPageSize().h);
	setVScrollVisibleAmount(clientSize2.h);
	setVScrollLargeAmount(clientSize2.h);
	setVScrollSmallAmount(layout_.getCellHeight());

	invalidate();
}

}//namespace piclist
