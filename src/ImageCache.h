#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <map>
#include "String.h"
#include "Image.h"

namespace piclist{

	/**
	 * �摜�L���b�V���ł��B
	 */
	class ImageCache
	{
		typedef std::map<String, ImagePtr> FileImageMap;
		FileImageMap fileImageMap_;
	public:
		ImagePtr getImage(const String &filepath)
		{
			///@todo filepath���΃p�X�֕ϊ�����B
			const FileImageMap::iterator it = fileImageMap_.lower_bound(filepath);
			if(it != fileImageMap_.end() && !fileImageMap_.key_comp()(filepath, it->first)){
				return it->second;
			}
			const ImagePtr im = Image::load(filepath);
			fileImageMap_.insert(it, FileImageMap::value_type(filepath, im));
			return im;
		}
	};

}//namespace piclist
#endif
