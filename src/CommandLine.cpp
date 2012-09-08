#include <algorithm>
#include <iterator>
#include <Windows.h>
#include <tchar.h>
#include "File.h"
#include "CommandLine.h"

namespace piclist{

/**
 * �R�}���h���C���������g�[�N�����ɕ������܂��B
 * @param cmdLineStr �R�}���h���C��������ł��B
 * @return cmdLineStr�𕪊����Ăł���������z��ł��B
 */
inline std::vector<String> splitCmdLineArgs(const String::value_type *cmdLineStr)
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
