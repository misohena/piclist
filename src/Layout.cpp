#include "Layout.h"

namespace piclist{

Layout::Layout()
	: cellImageWidth_(96)
	, cellImageHeight_(96)
	, cellNameHeight_(10)
	, cellWidth_(cellImageWidth_)
	, cellHeight_(cellImageHeight_ + cellNameHeight_)
	, lineSpace_(0)
	, columnSpace_(1)
	, cellStepX_(cellWidth_ + columnSpace_)
	, cellStepY_(cellHeight_ + lineSpace_)
	, pictureCount_()
	, columns_(1)
	, pageSize_()
{}

void Layout::update(const PictureContainer &pictures, const Size2i &clientSize)
{
	pictureCount_ = pictures.size();
	columns_ = std::max(1, (clientSize.w + columnSpace_) / cellStepX_);
	if(pictureCount_ == 0){
		// 0の場合。
		pageSize_.set(0, 0);
	}
	else if(pictureCount_ <= columns_){
		// 1行でおさまる場合。
		pageSize_.set(
			pictureCount_ * cellStepX_ - columnSpace_,
			cellHeight_);
	}
	else{
		// 複数行にわたる場合。
		pageSize_.set(
			columns_ * cellStepX_ - columnSpace_,
			(pictureCount_ + columns_ - 1) / columns_ * cellStepY_ - lineSpace_);
	}
}

const Size2i Layout::getPageSize() const
{
	return pageSize_;
}

Rect2i Layout::getImageRect(std::size_t index) const
{
	const int x = index % columns_ * cellStepX_;
	const int y = index / columns_ * cellStepY_;
	return Rect2i(x, y, x + cellImageWidth_, y + cellImageHeight_);
}

Rect2i Layout::getNameRect(std::size_t index) const
{
	const int x = index % columns_ * cellStepX_;
	const int y = index / columns_ * cellStepY_ + cellImageHeight_;
	return Rect2i(x, y, x + cellWidth_, y + cellNameHeight_);
}

int Layout::getImageWidth() const
{
	return cellImageWidth_;
}
int Layout::getImageHeight() const
{
	return cellImageHeight_;
}

int Layout::getNameHeight(void) const
{
	return cellNameHeight_;
}

int Layout::getCellStepY(void) const
{
	return cellStepY_;
}

}//namespace piclist
