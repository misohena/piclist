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

		static const TCHAR * const WINDOW_SUFFIX = _T(" - piclist");
		static const TCHAR * const WINDOW_CLASS_NAME = _T("PicListWindowClass");

		const String windowCaption = cmdline.getWindowName() + WINDOW_SUFFIX;

		if(HWND hwnd = ::FindWindow(WINDOW_CLASS_NAME, windowCaption.c_str())){
			const String cd = getCurrentDirectory(); //�J�����g�f�B���N�g��������Ȃ��ƃR�}���h���C���𐳂������߂ł��Ȃ��B
			std::vector<TCHAR> data;
			data.insert(data.end(), cd.begin(), cd.end());
			data.push_back(_T('\0'));
			data.insert(data.end(), cmdlineStr, cmdlineStr + lstrlen(cmdlineStr));
			data.push_back(_T('\0'));

			::COPYDATASTRUCT cds;
			cds.dwData = 0;
			cds.cbData = data.size() * sizeof(TCHAR);
			cds.lpData = &data[0];
			::SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)&cds);
			return false;
		}

		window_.reset(new AppWindow(WINDOW_CLASS_NAME, cmdline.getWindowName()));
		if(!window_->create()){
			return false;
		}
		window_->setCaption(windowCaption);

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
