#include <algorithm>
#include <iterator>
#include <Windows.h>
#include <tchar.h>
#include "File.h"
#include "CommandLine.h"

namespace piclist{

/**
 * コマンドライン引数をトークン毎に分割します。
 * @param cmdLineStr コマンドライン文字列です。
 * @return cmdLineStrを分割してできた文字列配列です。
 */
inline std::vector<String> splitCmdLineArgs(const String::value_type *cmdLineStr)
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

CommandLineParser::CommandLineParser()
	: windowName_(_T("Default"))
{}

void CommandLineParser::parse(const String::value_type *cmdLineStr)
{
	std::vector<String> args = splitCmdLineArgs(cmdLineStr);

	if(args.size() > 1){
		for(std::vector<String>::const_iterator it = args.begin() + 1, itEnd = args.end(); it != itEnd; ++it){
			const String &arg = *it;
			if(!arg.empty() && arg[0] == _T('-')){
				if(arg == _T("-w")){
					if(it + 1 != itEnd){
						++it;
						windowName_ = *it;
					}
				}
				else if(arg == _T("-br")){
					albumItems_.push_back(AlbumLineBreak::create());
				}
				else{
					//unknown
				}
			}
			else if(isExistingDirectory(arg)){
				std::vector<String> files;
				for(FileEnumerator fe(arg + _T("\\*")); fe.valid(); fe.increment()){
					const String &f = fe.getEntryFilePath();
					if(hasSupportedImageFileExtension(f)){
						files.push_back(f);
					}
				}
				std::sort(files.begin(), files.end());
				std::transform(files.begin(), files.end(), std::back_inserter(albumItems_), &AlbumPicture::create);
			}
			else if(isExistingRegularFile(arg)){
				albumItems_.push_back(AlbumPicture::create(getFullPathName(arg)));
			}
			else{ //wildcard?
				std::vector<String> files;
				for(FileEnumerator fe(arg); fe.valid(); fe.increment()){
					files.push_back(fe.getEntryFilePath());
				}
				std::sort(files.begin(), files.end());
				std::transform(files.begin(), files.end(), std::back_inserter(albumItems_), &AlbumPicture::create);
			}
		}
	}
}

}//namespace piclist
