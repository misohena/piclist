#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#include "CommandLine.h"
#include "File.h"
#include "resource.h"
#include "AppWindow.h"
#include "ValueInputDialog.h"


namespace piclist{

static const TCHAR * const REGISTRY_KEY = _T("SOFTWARE\\Misohena\\piclist\\");

AppWindow::AppWindow(const String &className, const String &windowName)
	: Window(className)
	, windowName_(windowName)
{
	menuMainPopup_.load(IDR_MENU_MAINPOPUP);
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
	Gdiplus::Graphics graphics(hdc);

	// set scroll matrix
	const int scrollY = getVScrollPosition();
	Gdiplus::Matrix mat;
	mat.Translate(0, static_cast<Gdiplus::REAL>(-scrollY));
	graphics.MultiplyTransform(&mat);

	// prepare drawing text
	Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 0, 0, 0));
	Gdiplus::FontFamily fontFamily(_T("MS GOTHIC"));
	Gdiplus::Font font(&fontFamily, static_cast<Gdiplus::REAL>(layout_.getNameHeight()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat stringFormat;
	stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	// draw cells
	const std::size_t itemCount = albumItems_.size();
	for(std::size_t index = 0; index < itemCount; ++index){
		const AlbumItemPtr &item = albumItems_[index];

		if(const AlbumPicture * const pic = dynamic_cast<AlbumPicture *>(item.get())){

			// image.
			if(ImagePtr im = imageCache_.getImage(pic->getFilePath(), Size2i(layout_.getImageWidth(), layout_.getImageHeight()))){
				const double srcW = im->getWidth();
				const double srcH = im->getHeight();
				const Rect2i cellImageRect = layout_.getImageRect(index);
				const double scale = min(cellImageRect.getWidth() / srcW, cellImageRect.getHeight() / srcH);
				const int dstW = static_cast<int>(srcW * scale);
				const int dstH = static_cast<int>(srcH * scale);
				graphics.DrawImage(
					im->getGdiPlusImage(),
					cellImageRect.left + (cellImageRect.getWidth() - dstW) / 2,
					cellImageRect.top + (cellImageRect.getHeight() - dstH) / 2,
					dstW,
					dstH);
			}

			// name.
			const Rect2i cellNameRect = layout_.getNameRect(index);
			Gdiplus::RectF nameRect(
				static_cast<Gdiplus::REAL>(cellNameRect.left),
				static_cast<Gdiplus::REAL>(cellNameRect.top),
				static_cast<Gdiplus::REAL>(cellNameRect.getWidth()),
				static_cast<Gdiplus::REAL>(cellNameRect.getHeight()));
			graphics.DrawString(pic->getFileNameBase().c_str(), -1, &font, nameRect, &stringFormat, &textBrush);

		}
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

void AppWindow::onRButtonUp(unsigned int keys, int x, int y)
{
	const Point2i screenPt = clientToScreen(Point2i(x, y));
	menuMainPopup_.popupSubMenu(0, screenPt.x, screenPt.y, getWindowHandle());
}

void AppWindow::onCommand(int notificationCode, int id, HWND hWndControl)
{
	switch(id){
	case ID_MAINPOPUP_TOPMOST:
		if(isZOrderTopMost()){
			setZOrderNoTopMost();
		}
		else{
			setZOrderTopMost();
		}
		break;
	case ID_MAINPOPUP_DUP_WINDOW:
		//duplicateWindow();
		break;
	case ID_MAINPOPUP_IMAGE_WIDTH:
		inputLayoutParam(Layout::LP_IMAGE_WIDTH);
		break;
	case ID_MAINPOPUP_IMAGE_HEIGHT:
		inputLayoutParam(Layout::LP_IMAGE_HEIGHT);
		break;
	case ID_MAINPOPUP_NAME_HEIGHT:
		inputLayoutParam(Layout::LP_NAME_HEIGHT);
		break;
	case ID_MAINPOPUP_COLUMN_SPACE:
		inputLayoutParam(Layout::LP_COLUMN_SPACE);
		break;
	case ID_MAINPOPUP_LINE_SPACE:
		inputLayoutParam(Layout::LP_LINE_SPACE);
		break;
	}
}

/**
 * ユーザーからレイアウトに関する値を受け付けて、レイアウトを更新します。
 */
void AppWindow::inputLayoutParam(Layout::LayoutParamType lpt)
{
	ValueInputDialog dlg(Layout::getLayoutParamName(lpt) + _T("を入力してください。"), layout_.getLayoutParam(lpt));
	if(dlg.doModal(getWindowHandle())){
		layout_.setLayoutParam(lpt, dlg.getResultInt());
		updateLayout();
	}
}

void AppWindow::onCopyData(HWND srcwnd, ULONG_PTR dwData, DWORD cbData, PVOID lpData)
{
	const TCHAR * const currentDir = (TCHAR *)lpData;
	const int lenCurrentDir = lstrlen(currentDir);
	const TCHAR * const cmdlineStr = currentDir + lenCurrentDir + 1;

	{
		ScopedCurrentDirectory cd(currentDir);

		CommandLineParser cmdline;
		cmdline.parse(cmdlineStr);

		setAlbum(cmdline.getAlbum());
	}
}

void AppWindow::updateLayout(void)
{
	const Size2i clientSize1 = getClientSize();
	layout_.update(albumItems_, clientSize1);

	setVScrollBarVisible(layout_.getPageSize().h > clientSize1.h);

	const Size2i clientSize2 = getClientSize();
	layout_.update(albumItems_, clientSize2);

	setVScrollRange(0, layout_.getPageSize().h);
	setVScrollVisibleAmount(clientSize2.h);
	setVScrollLargeAmount(clientSize2.h);
	setVScrollSmallAmount(layout_.getCellStepY());

	invalidate();
}

}//namespace piclist
