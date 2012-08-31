#ifndef STRING_H
#define STRING_H

#include <string>

namespace piclist{

	/**
	 * piclist内で使用する文字列型です。
	 * ANSIビルドの場合はstd::string、UNICODEビルドの場合はstd::wstringです。
	 */
#ifdef _MBCS
	typedef std::string String;
#else
	typedef std::wstring String;
#endif

}//namespace piclist
#endif
