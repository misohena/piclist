#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#include "resource.h"
#include "CommandLine.h"
#include "File.h"
#include "ValueInputDialog.h"
#include "Stream.h"
#include "AppWindow.h"


namespace piclist{

static const TCHAR * const REGISTRY_KEY = _T("SOFTWARE\\Misohena\\piclist\\");
static const TCHAR * const REGISTRY_PREFIX_LAYOUT_PARAMS = _T("Layout_");
static const TCHAR * const APPWINDOW_CLASS_NAME = _T("PicListWindowClass");
static const TCHAR * const APPWINDOW_SUFFIX = _T(" - piclist");


AppWindow::AppWindow(const String &windowName)
	: Window(APPWINDOW_CLASS_NAME)
	, windowName_(windowName)
{
	menuMainPopup_.load(IDR_MENU_MAINPOPUP);

	restoreLayoutParams();
}

AppWindow::~AppWindow()
{
}


/**
 * アプリケーションウィンドウのキャプション名を作成します。
 */
String AppWindow::makeWindowCaption(const String &windowName)
{
	return windowName + APPWINDOW_SUFFIX;
}


/**
 * ウィンドウの配置をレジストリから読み取り、復元します。
 */
bool AppWindow::restoreWindowPlacement()
{
	return Window::restoreWindowPlacement(REGISTRY_KEY + windowName_);
}

/**
 * レイアウトパラメータをレジストリへ保存します。
 */
void AppWindow::backupLayoutParams()
{
	///@todo Layoutクラスへ移動すべき？　Layoutは今のところwindows.hに依存していないので、レジストリAPIをラップしたものを作ってからにしたい。
	const String keyPath = REGISTRY_KEY + windowName_;
	HKEY key;
	DWORD disposition;
	const LONG result = ::RegCreateKeyEx(
		HKEY_CURRENT_USER, keyPath.c_str(), NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disposition);
	if(result != ERROR_SUCCESS){
		return;
	}
	for(int lpti = 0; lpti < Layout::LP_COUNT; ++lpti){
		const Layout::LayoutParamType lpt = static_cast<Layout::LayoutParamType>(lpti);
		const DWORD value = layout_.getLayoutParam(lpt);
		const String valueName = REGISTRY_PREFIX_LAYOUT_PARAMS + Layout::getLayoutParamId(lpt);
		::RegSetValueEx(key, valueName.c_str(), NULL, REG_DWORD, (BYTE *)&value, sizeof(value));
	}
	::RegCloseKey(key);
	return;
}

/**
 * レイアウトパラメータをレジストリから読み込みます。
 */
void AppWindow::restoreLayoutParams()
{
	const String keyPath = REGISTRY_KEY + windowName_;
	HKEY key;
	LONG result;
	result = ::RegOpenKeyEx(
		HKEY_CURRENT_USER, keyPath.c_str(), NULL, KEY_ALL_ACCESS, &key);
	if(result != ERROR_SUCCESS){
		return;
	}

	for(int lpti = 0; lpti < Layout::LP_COUNT; ++lpti){
		const Layout::LayoutParamType lpt = static_cast<Layout::LayoutParamType>(lpti);
		DWORD value = 0;
		DWORD type;
		DWORD size = sizeof(value);
		const String valueName = REGISTRY_PREFIX_LAYOUT_PARAMS + Layout::getLayoutParamId(lpt);
		result = ::RegQueryValueEx(key, valueName.c_str(), NULL, &type, (BYTE *)&value, &size);
		if(result == ERROR_SUCCESS && type == REG_DWORD && size == sizeof(value) ){
			layout_.setLayoutParam(lpt, value);
		}
	}
}

void AppWindow::setAlbum(const AlbumItemContainer &items)
{
	albumItems_ = items;
	updateLayout();
}

void AppWindow::updateLayout(void)
{
	const Size2i clientSize1 = getClientSize();
	layout_.update(albumItems_, clientSize1, imageCache_);

	setVScrollBarVisible(layout_.getPageSize().h > clientSize1.h);

	const Size2i clientSize2 = getClientSize();
	layout_.update(albumItems_, clientSize2, imageCache_);

	setVScrollRange(0, layout_.getPageSize().h);
	setVScrollVisibleAmount(clientSize2.h);
	setVScrollLargeAmount(clientSize2.h);
	setVScrollSmallAmount(layout_.getCellStepY());

	invalidate();
}

// Window Message Handlers

void AppWindow::onCreate()
{
	setCaption(makeWindowCaption(windowName_));
	updateLayout();
	dragAcceptFiles(true);
}

void AppWindow::onDestroy()
{
	backupWindowPlacement(REGISTRY_KEY + windowName_);
	backupLayoutParams();
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

	Menu sub;
	sub.setSubMenu(menuMainPopup_, 0);

	sub.checkItem(ID_MAINPOPUP_TOPMOST, isZOrderTopMost());
	sub.popupMenu(screenPt.x, screenPt.y, getWindowHandle());
}

void AppWindow::onCopyData(HWND srcwnd, ULONG_PTR dwData, DWORD cbData, PVOID lpData)
{
	receiveInterProcessCommand(dwData, cbData, static_cast<unsigned char *>(lpData));
}

void AppWindow::onDropFiles(HDROP hDrop)
{
	const unsigned int count = ::DragQueryFile(hDrop, -1, NULL, 0);
	for(unsigned int index = 0; index < count; ++index){

		unsigned int length = ::DragQueryFile(hDrop, index, NULL, 0);
		if(length == 0){
			continue;
		}
		std::vector<TCHAR> buffer(length+1);
		::DragQueryFile(hDrop, index, &buffer[0], buffer.size());
		const String file(&buffer[0]);

		if(isExistingDirectory(file)){
			for(FileEnumerator fe(file + _T("\\*")); fe.valid(); fe.increment()){
				const String fileInDir = fe.getEntryFilePath();
				if(hasSupportedImageFileExtension(fileInDir)){
					albumItems_.push_back(AlbumPicture::create(fileInDir));
				}
			}
		}
		else if(isExistingRegularFile(file)){
			if(hasSupportedImageFileExtension(file)){
				albumItems_.push_back(AlbumPicture::create(file));
			}
		}
	}
	updateLayout();
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
		duplicateWindow();
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
 * 現在のウィンドウの内容と同じウィンドウを新しく作成します。
 */
void AppWindow::duplicateWindow()
{
	String newWindowName;
	for(int i = 0; i < 100; ++i){
		newWindowName = windowName_ + _T("のコピー");
		if(i){
			newWindowName += std::to_wstring(static_cast<long long>(i));
		}
		if(!findWindowByWindowName(newWindowName)){
			break;
		}
	}

	ValueInputDialog dlg(_T("複製先のウィンドウ名を入力してください。"), newWindowName);
	if(dlg.doModal(getWindowHandle())){
		newWindowName = dlg.getResultString();

		if(HWND hwnd = findWindowByWindowName(newWindowName)){
			if(::MessageBox(getWindowHandle(), _T("そのウィンドウ名を持つウィンドウはすでに存在します。そのウィンドウの内容を更新しますか？"), _T("確認"), MB_OKCANCEL) == IDOK){
				sendAlbum(hwnd, albumItems_);
			}
		}
		else if(HWND hwnd = openNewWindow(newWindowName)){
			sendAlbum(hwnd, albumItems_);
		}
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

/**
 * 指定されたウィンドウ名のウィンドウを新しく作成します。
 */
HWND AppWindow::openNewWindow(const String &windowName)
{
	TCHAR exepath[MAX_PATH];
	::GetModuleFileName(NULL, exepath, MAX_PATH);

	const String cmdline = String(exepath) + _T(" -w ") + windowName;
	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi = {};
	if(!::CreateProcess(exepath, const_cast<TCHAR *>(cmdline.c_str()), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)){
		return NULL;
	}
	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);

	for(int i = 0; i < 10; ++i){
		::Sleep(200);
		if(HWND hwnd = findWindowByWindowName(windowName)){
			return hwnd;
		}
	}

	return NULL;
}


// ------------------------------------------------------------------
// プロセス間コマンド処理
// ------------------------------------------------------------------

/**
 * ウィンドウ名からすでに開いているアプリケーションウィンドウを探します。
 */
HWND AppWindow::findWindowByWindowName(const String &windowName)
{
	return ::FindWindow(APPWINDOW_CLASS_NAME, makeWindowCaption(windowName).c_str());
}
/**
 * プロセス間コマンドを受け取ります。
 */
void AppWindow::receiveInterProcessCommand(unsigned int code, unsigned int cbData, unsigned char *lpData)
{
	switch(code){
	case IPC_CMDLINE: receiveCommandLine(cbData, reinterpret_cast<unsigned char *>(lpData)); break;
	case IPC_ALBUM: receiveAlbum(cbData, reinterpret_cast<unsigned char *>(lpData)); break;
	}
}

/**
 * プロセス間コマンドを送信します。
 */
void AppWindow::sendInterProcessCommand(HWND dstWnd, InterProcessCommand code, std::vector<unsigned char> &bytes)
{
	::COPYDATASTRUCT cds;
	cds.dwData = code;
	cds.cbData = bytes.size();
	cds.lpData = bytes.empty() ? NULL : &bytes[0];
	::SendMessage(dstWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
}

/**
 * ターゲットウィンドウへコマンドラインを転送します。
 */
void AppWindow::sendCommandLine(HWND dstWnd, const String &currentDir, const String &cmdlineStr)
{
	VectorOutputStream os;
	os.writeString(currentDir);
	os.writeString(cmdlineStr);
	sendInterProcessCommand(dstWnd, IPC_CMDLINE, os.getBuffer());
}

/**
 * コマンドラインを受け取ります。
 */
void AppWindow::receiveCommandLine(unsigned int cbData, unsigned char *lpData)
{
	MemoryInputStream is(lpData, lpData + cbData);
	const String currentDir = is.readString();
	const String cmdlineStr = is.readString();
	{
		ScopedCurrentDirectory cd(currentDir);

		CommandLineParser cmdline;
		cmdline.parse(cmdlineStr.c_str());

		setAlbum(cmdline.getAlbum());
	}
}

/**
 * アルバムデータを送ります。
 */
void AppWindow::sendAlbum(HWND dstWnd, const AlbumItemContainer &albumItems)
{
	VectorOutputStream os;

	const std::size_t itemCount = albumItems.size();
	os.writeUInt32(itemCount);

	for(std::size_t i = 0; i < itemCount; ++i){
		const AlbumItemPtr &item = albumItems[i];
		const AlbumItem::Type type = item->getType();
		os.writeUInt32(type);

		struct OutputContent
		{
			VectorOutputStream &os;
			OutputContent(VectorOutputStream &os) : os(os){}
			void operator()(const AlbumPicture &pic) const
			{
				os.writeString(pic.getFilePath()); ///@todo 確実にフルパスにしなければならないのでは無いか。今のところたまたまフルパスになるけど。
			}
			void operator()(const AlbumLineBreak &) const
			{
			}
		};
		item->dispatch<void>(OutputContent(os));
	}

	sendInterProcessCommand(dstWnd, IPC_ALBUM, os.getBuffer());
}

/**
 * アルバムデータを受け取ります。
 */
void AppWindow::receiveAlbum(unsigned int cbData, unsigned char *lpData)
{
	MemoryInputStream is(lpData, lpData + cbData);

	const std::size_t itemCount = is.readUInt32();

	AlbumItemContainer albumItems;

	for(std::size_t i = 0; i < itemCount; ++i){
		const unsigned int type = is.readUInt32();
		switch(type){
		case AlbumItem::TYPE_PICTURE:
			{
				const String filepath = is.readString();
				AlbumItemPtr pic(AlbumPicture::create(filepath));
				albumItems.push_back(pic);
			}
			break;
		case AlbumItem::TYPE_LINE_BREAK:
			albumItems.push_back(AlbumLineBreak::create());
			break;
		default:
			//error: unknown type
			return; //abort receive
		}
	}
	if(is.hasError()){
		return;
	}
	setAlbum(albumItems);
}


}//namespace piclist
