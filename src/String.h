#ifndef STRING_H
#define STRING_H

#include <string>

namespace piclist{

	/**
	 * piclist���Ŏg�p���镶����^�ł��B
	 * ANSI�r���h�̏ꍇ��std::string�AUNICODE�r���h�̏ꍇ��std::wstring�ł��B
	 */
#ifdef _MBCS
	typedef std::string String;
#else
	typedef std::wstring String;
#endif

#ifdef _MBCS
# define STRING_LIT(s) s
#else
# define STRING_LIT(s) L##s
#endif

}//namespace piclist
#endif
