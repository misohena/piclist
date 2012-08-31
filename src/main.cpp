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
 * コマンドライン引数をトークン毎に分割します。
 * @param cmdLineStr コマンドライン文字列です。
 * @return cmdLineStrを分割してできた文字列配列です。
 */
std::vector<String> splitCmdLineArgs(const String::value_type *cmdLineStr)
{
	std::vector<String> args;
	typedef String::value_type CharType;

	String token;
	bool insideQuoted = false;
	bool insideMbcChar = false;
	int continuousBackSlashCount = 0;
	int continuousDelimiterCount = 1;//最初は区切り文字があるものとして扱う。

	//効率が悪くても1バイトずつ丁寧に処理をしていく。
	for(const CharType *p = cmdLineStr; *p != _T('\0'); p++){
		//ここで文字の種類を完全に判断してしまう。
		const CharType ch = *p;
		const bool chIsBackSlash = (ch == _T('\\') && !insideMbcChar);
		const bool chIsQuote = (ch == _T('"') && !insideMbcChar);
		const bool chIsSpace = ((ch == _T(' ') || ch == _T('\t')) && !insideMbcChar);
		const bool chIsDelimiter = (chIsSpace && !insideQuoted);
		const bool chIsNormalChar = (!chIsBackSlash && !chIsQuote && !chIsDelimiter);//スペースであってもクォートされていれば通常の文字になることに注意。

		//文字種別ごとの処理

		if(chIsDelimiter){
			if(continuousDelimiterCount == 0){
				args.push_back(token);
				token.clear();
			}
		}
		else if(chIsQuote){
			//クォート前のバックスラッシュを展開する。
			token.append(continuousBackSlashCount / 2, _T('\\'));
			if((continuousBackSlashCount % 2) == 0){
				//バックスラッシュが偶数なら余りのバックスラッシュが無く、クォートは単独で有効。
				//クォートを開始する。
				insideQuoted = !insideQuoted;
			}
			else{
				//バックスラッシュが奇数なら余りのバックスラッシュがあり、クォートはエスケープされる。
				//エスケープされたクォートを受け入れる。
				token += _T('"');
			}
		}
		else if(chIsNormalChar){
			//バックスラッシュを展開する。
			token.append(continuousBackSlashCount, _T('\\'));
			//普通の文字を受け入れる。
			token += ch;
		}
#ifdef _MBCS
		//マルチバイト文字から脱出
		if(insideMbcChar){
			insideMbcChar = false;
		}
		//マルチバイト文字の中に入ったか。
		else if(::IsDBCSLeadByte((BYTE)ch)){
			insideMbcChar = true;
		}
#endif
		//連続するバックスラッシュを数える。
		if(chIsBackSlash){
			continuousBackSlashCount++;
		}
		else{
			continuousBackSlashCount = 0;
		}

		//連続するデリミタを数える。
		if(chIsDelimiter){
			continuousDelimiterCount++;
		}
		else{
			continuousDelimiterCount = 0;
		}
	}

	//未処理のバックスラッシュを処理する。
	token.append(continuousBackSlashCount, _T('\\'));

	//最後のトークンを追加する。
	if( ! token.empty() || insideQuoted){
		args.push_back(token);
	}

	//結果の返却
	return args;
}


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
