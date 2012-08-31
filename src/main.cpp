#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#pragma comment (lib,"Gdiplus.lib")

#include "AppWindow.h"


namespace piclist{

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

class PicListApp
{
	GdiPlusSetup gp;
	AppWindow window;
public:
	bool init(int nCmdShow)
	{

		if(!window.create()){
			return false;
		}
		window.showWindow(nCmdShow);
		window.updateWindow();

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
