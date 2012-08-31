#ifndef IMAGE_H
#define IMAGE_H

#include <memory>
#include "String.h"

namespace Gdiplus{ class Image;}

namespace piclist{
	class Image;
	typedef std::shared_ptr<Image> ImagePtr;

	/**
	 * 画像データを保持するクラスです。
	 */
	class Image
	{
		std::unique_ptr<Gdiplus::Image> im_;
		explicit Image(const String &filepath);
	public:
		static ImagePtr load(const String &filepath);
		int getWidth() const;
		int getHeight() const;
		Gdiplus::Image *getGdiPlusImage() const { return im_.get();}
	};

}//namespace piclist
#endif
