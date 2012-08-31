#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#pragma comment (lib,"Gdiplus.lib")
#include "CommandLine.h"
#include "AppWindow.h"


namespace piclist{

/**
 * GDI+の初期化と終了処理を行うクラスです。
 *
 * このオブジェクトをアプリケーション内で最初に構築し、最後に破棄するようにしてください。
 */
class GdiPlusSetup
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
public:
	GdiPlusSetup()
		: gdiplusToken()
	{
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	}
	~GdiPlusSetup()
	{
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
};

/**
 * piclistアプリケーションを表すクラスです。
 */
class PicListApp
{
	GdiPlusSetup gp_;
	std::unique_ptr<AppWindow> window_; //順番に注意。gp_より後。GdiPlus::Imageへの参照が残ったままGdiplusShutdownしないように。
public:
	bool init(int nCmdShow)
	{
		TCHAR * const cmdlineStr = GetCommandLine();;
		CommandLineParser cmdline;
		cmdline.parse(cmdlineStr);

		static const TCHAR * const WINDOW_SUFFIX = _T(" - piclist");
		static const TCHAR * const WINDOW_CLASS_NAME = _T("PicListWindowClass");

		const String windowCaption = cmdline.getWindowName() + WINDOW_SUFFIX;

		if(HWND hwnd = ::FindWindow(WINDOW_CLASS_NAME, windowCaption.c_str())){
			::COPYDATASTRUCT cd;
			cd.dwData = 0;
			cd.cbData = (lstrlen(cmdlineStr) + 1) * sizeof(TCHAR);
			cd.lpData = cmdlineStr;
			::SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)&cd);
			return false;
		}

		window_.reset(new AppWindow(WINDOW_CLASS_NAME, cmdline.getWindowName()));
		if(!window_->create()){
			return false;
		}
		window_->setCaption(windowCaption);

		window_->setPictures(cmdline.getPictures());

		if(!window_->restoreWindowPlacement()){
			window_->showWindow(nCmdShow);
		}
		window_->updateWindow();

		return true;
	}
};

}//namespace piclist


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	using namespace piclist;

	PicListApp app;
	if(!app.init(nCmdShow)){
		return 0;
	}

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
