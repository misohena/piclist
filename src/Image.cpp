#include <Windows.h>
#include <GdiPlus.h>
#include "Image.h"


namespace piclist{


Image::Image(std::unique_ptr<Gdiplus::Image> &&im)
	: im_(std::move(im))
{
}

ImagePtr Image::load(const String &filepath, const Size2i &size)
{
	Gdiplus::Image source(filepath.c_str());
	const double sourceW = source.GetWidth();
	const double sourceH = source.GetHeight();
	if(sourceW <= 0 || sourceH <= 0){
		return ImagePtr();
	}
	const double scale
		= size.h < 0 ? //高さが負の時は自動で決める。-size.hが許容する最大の高さを意味する。
			(sourceH * (size.w / sourceW) > -size.h) ? -size.h / sourceH //幅で合わせたときに最大の高さを超えるなら、その最大の高さで合わせる。
			: size.w / sourceW //幅に合わせて高さを決める。
		: min(size.w / sourceW, size.h / sourceH); //より小さくなる方で決める。
	const int thumbnailW = static_cast<int>(sourceW * scale + 0.5);
	const int thumbnailH = static_cast<int>(sourceH * scale + 0.5);

	std::unique_ptr<Gdiplus::Image> thumbnail(source.GetThumbnailImage(thumbnailW, thumbnailH, NULL, NULL));

	return ImagePtr(new Image(std::move(thumbnail)));
}

int Image::getWidth() const
{
	return im_->GetWidth();
}

int Image::getHeight() const
{
	return im_->GetHeight();
}

}//namespace piclist
