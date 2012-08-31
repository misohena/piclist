#include <Windows.h>
#include <GdiPlus.h>
#include "Image.h"


namespace piclist{


Image::Image(const String &filepath)
	: im_(new Gdiplus::Image(filepath.c_str()))
{
}

ImagePtr Image::load(const String &filepath)
{
	ImagePtr im(new Image(filepath));
	return im;
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
