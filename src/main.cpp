#include <algorithm>
#include <vector>
#include <windows.h>
#include <tchar.h>
#include <GdiPlus.h>
#pragma comment (lib,"Gdiplus.lib")
#include "File.h"
#include "AppWindow.h"


namespace piclist{

/**
 * �R�}���h���C���������g�[�N�����ɕ������܂��B
 * @param cmdLineStr �R�}���h���C��������ł��B
 * @return cmdLineStr�𕪊����Ăł���������z��ł��B
 */
std::vector<String> splitCmdLineArgs(const String::value_type *cmdLineStr)
{
	std::vector<String> args;
	typedef String::value_type CharType;

	String token;
	bool insideQuoted = false;
	bool insideMbcChar = false;
	int continuousBackSlashCount = 0;
	int continuousDelimiterCount = 1;//�ŏ��͋�؂蕶����������̂Ƃ��Ĉ����B

	//�����������Ă�1�o�C�g�����J�ɏ��������Ă����B
	for(const CharType *p = cmdLineStr; *p != _T('\0'); p++){
		//�����ŕ����̎�ނ����S�ɔ��f���Ă��܂��B
		const CharType ch = *p;
		const bool chIsBackSlash = (ch == _T('\\') && !insideMbcChar);
		const bool chIsQuote = (ch == _T('"') && !insideMbcChar);
		const bool chIsSpace = ((ch == _T(' ') || ch == _T('\t')) && !insideMbcChar);
		const bool chIsDelimiter = (chIsSpace && !insideQuoted);
		const bool chIsNormalChar = (!chIsBackSlash && !chIsQuote && !chIsDelimiter);//�X�y�[�X�ł����Ă��N�H�[�g����Ă���Βʏ�̕����ɂȂ邱�Ƃɒ��ӁB

		//������ʂ��Ƃ̏���

		if(chIsDelimiter){
			if(continuousDelimiterCount == 0){
				args.push_back(token);
				token.clear();
			}
		}
		else if(chIsQuote){
			//�N�H�[�g�O�̃o�b�N�X���b�V����W�J����B
			token.append(continuousBackSlashCount / 2, _T('\\'));
			if((continuousBackSlashCount % 2) == 0){
				//�o�b�N�X���b�V���������Ȃ�]��̃o�b�N�X���b�V���������A�N�H�[�g�͒P�ƂŗL���B
				//�N�H�[�g���J�n����B
				insideQuoted = !insideQuoted;
			}
			else{
				//�o�b�N�X���b�V������Ȃ�]��̃o�b�N�X���b�V��������A�N�H�[�g�̓G�X�P�[�v�����B
				//�G�X�P�[�v���ꂽ�N�H�[�g���󂯓����B
				token += _T('"');
			}
		}
		else if(chIsNormalChar){
			//�o�b�N�X���b�V����W�J����B
			token.append(continuousBackSlashCount, _T('\\'));
			//���ʂ̕������󂯓����B
			token += ch;
		}
#ifdef _MBCS
		//�}���`�o�C�g��������E�o
		if(insideMbcChar){
			insideMbcChar = false;
		}
		//�}���`�o�C�g�����̒��ɓ��������B
		else if(::IsDBCSLeadByte((BYTE)ch)){
			insideMbcChar = true;
		}
#endif
		//�A������o�b�N�X���b�V���𐔂���B
		if(chIsBackSlash){
			continuousBackSlashCount++;
		}
		else{
			continuousBackSlashCount = 0;
		}

		//�A������f���~�^�𐔂���B
		if(chIsDelimiter){
			continuousDelimiterCount++;
		}
		else{
			continuousDelimiterCount = 0;
		}
	}

	//�������̃o�b�N�X���b�V������������B
	token.append(continuousBackSlashCount, _T('\\'));

	//�Ō�̃g�[�N����ǉ�����B
	if( ! token.empty() || insideQuoted){
		args.push_back(token);
	}

	//���ʂ̕ԋp
	return args;
}


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
		static const TCHAR * const WINDOW_SUFFIX = _T(" - piclist");
		static const TCHAR * const WINDOW_CLASS_NAME = _T("PicListWindowClass");
		static const String windowName = _T("Default");

		const String windowCaption = windowName + WINDOW_SUFFIX;
		window_.reset(new AppWindow(WINDOW_CLASS_NAME, windowName));
		if(!window_->create()){
			return false;
		}
		window_->setCaption(windowCaption);

		const std::vector<String> args = splitCmdLineArgs(GetCommandLine());
		if(args.size() > 1){
			PictureContainer pictures;
			for(std::vector<String>::const_iterator it = args.begin() + 1, itEnd = args.end(); it != itEnd; ++it){
				for(FileEnumerator fe(*it); fe.valid(); fe.increment()){
					pictures.push_back(Picture(fe.getEntryFilePath()));
				}
			}
			std::sort(pictures.begin(), pictures.end(), Picture::LessFilePath());
			window_->addPictures(pictures);
			window_->updateLayout();
		}

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
