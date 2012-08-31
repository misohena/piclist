#ifndef PICTURE_H
#define PICTURE_H

#include <memory>
#include <deque>
#include <cstdlib>
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
	};

	typedef std::deque<Picture> PictureContainer;

}//namespace piclist
#endif
