#ifndef PICTURE_H
#define PICTURE_H

#include <memory>
#include <deque>
#include <cstdlib>
#include <tchar.h>
#include "String.h"

namespace piclist{

	class Picture
	{
		String filepath_;
	public:
		Picture(const String &filepath)
			: filepath_(filepath)
		{}
		const String &getFilePath() const
		{
			return filepath_;
		}
		String getFileNameBase() const
		{
			String::value_type base[1024];
			if(_wsplitpath_s(filepath_.c_str(), NULL, 0, NULL, 0, base, sizeof(base)/sizeof(base[0]), NULL, 0)){
				return String();
			}
			return base;
		}

		// ソートのため

		class LessFilePath
		{
		public:
			bool operator()(const Picture &lhs, const Picture &rhs) const
			{
				return lhs.getFilePath() < rhs.getFilePath();
			}
		};

		// 改行のため
		///@todo PictureではなくAlbumItemのようなクラスを作るべきかもしれない。

		static TCHAR *getLineBreakFilePath()
		{
			return _T("\x1b:br");
		}
		static Picture createLineBreak()
		{
			return Picture(String());
		}
		bool isLineBreak() const
		{
			return filepath_.empty();
		}

	};

	typedef std::deque<Picture> PictureContainer;

}//namespace piclist
#endif
