#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#pragma comment (lib,"Gdiplus.lib")
#include "CommandLine.h"
#include "AppWindow.h"
#include "File.h"


namespace piclist{

/**
 * GDI+�̏������ƏI���������s���N���X�ł��B
 *
 * ���̃I�u�W�F�N�g���A�v���P�[�V�������ōŏ��ɍ\�z���A�Ō�ɔj������悤�ɂ��Ă��������B
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
 * piclist�A�v���P�[�V������\���N���X�ł��B
 */
class PicListApp
{
	GdiPlusSetup gp_;
	std::unique_ptr<AppWindow> window_; //���Ԃɒ��ӁBgp_����BGdiPlus::Image�ւ̎Q�Ƃ��c�����܂�GdiplusShutdown���Ȃ��悤�ɁB
public:
	bool init(int nCmdShow)
	{
		TCHAR * const cmdlineStr = GetCommandLine();;
		CommandLineParser cmdline;
		cmdline.parse(cmdlineStr);

		if(HWND hwnd = AppWindow::findWindowByWindowName(cmdline.getWindowName())){
			AppWindow::sendCommandLine(hwnd, getCurrentDirectory(), cmdlineStr);//�J�����g�f�B���N�g��������Ȃ��ƃR�}���h���C���𐳂������߂ł��Ȃ��B
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
