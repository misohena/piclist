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
		= size.h < 0 ? //���������̎��͎����Ō��߂�B-size.h�����e����ő�̍������Ӗ�����B
			(sourceH * (size.w / sourceW) > -size.h) ? -size.h / sourceH //���ō��킹���Ƃ��ɍő�̍����𒴂���Ȃ�A���̍ő�̍����ō��킹��B
			: size.w / sourceW //���ɍ��킹�č��������߂�B
		: min(size.w / sourceW, size.h / sourceH); //��菬�����Ȃ���Ō��߂�B
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
