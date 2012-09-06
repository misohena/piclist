#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#pragma comment (lib,"Gdiplus.lib")
#include "CommandLine.h"
#include "AppWindow.h"
#include "File.h"


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

		if(HWND hwnd = AppWindow::findWindowByWindowName(cmdline.getWindowName())){
			AppWindow::sendCommandLine(hwnd, getCurrentDirectory(), cmdlineStr);//カレントディレクトリも送らないとコマンドラインを正しく解釈できない。
			return false;
		}

		window_.reset(new AppWindow(cmdline.getWindowName()));
		if(!window_->create()){
			return false;
		}

		window_->setAlbum(cmdline.getAlbum());

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
