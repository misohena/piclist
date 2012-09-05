#include <algorithm>
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
	lines_.clear();
	pictureCount_ = pictures.size();
	columns_ = std::max(1, (clientSize.w + columnSpace_) / cellStepX_);

	typedef PictureContainer::const_iterator PicIt;

	class LineDivider
	{
		const unsigned int columns_;
		const PicIt beg_;
		const PicIt end_;
		PicIt curr_;
	public:
		LineDivider(unsigned int columns, const PicIt &beg, const PicIt &end)
			: columns_(columns), beg_(beg), end_(end), curr_(beg) {}

		bool isTerminated() const { return curr_ == end_;}
		const PicIt &getCurrent() const { return curr_;}
		PicIt getLineEnd()
		{
			unsigned int count = 0;
			for(;;){
				if(curr_ == end_){
					return curr_;
				}
				if(curr_->isLineBreak()){
					const PicIt lineEnd = curr_;
					++curr_;
					return lineEnd;
				}
				if(count++ >= columns_){
					return curr_;
				}
				++curr_;
			}
		}
	};

	LineDivider lineDivider(columns_, pictures.begin(), pictures.end());

	int lineTop = 0;
	while(!lineDivider.isTerminated()){
		const PicIt lineBeg = lineDivider.getCurrent();
		const PicIt lineEnd = lineDivider.getLineEnd();

		lines_.insert(LineContainer::value_type(lineBeg - pictures.begin(), lineTop));

		///@todo ‰æ‘œ‚Ì‚‚³‚ªAUTO‚Ìê‡As“à‚ÌÅ‘å‚Ì‰æ‘œ‚Ì‚‚³‚ğ‹‚ß‚éB

		lineTop += cellStepY_;
	}

	pageSize_.set(
		columns_ * cellStepX_ - columnSpace_,
		lineTop ? lineTop - lineSpace_ : 0);
}

const Size2i Layout::getPageSize() const
{
	return pageSize_;
}

std::pair<std::size_t, int> Layout::findLine(std::size_t index) const
{
	if(lines_.empty()){
		return std::pair<std::size_t, int>(0, 0);
	}

	LineContainer::const_iterator it = lines_.lower_bound(index);
	if(it == lines_.end()){
		return *lines_.rbegin();
	}
	if(it->first == index){
		return *it;
	}
	if(it == lines_.begin()){
		return std::pair<std::size_t, int>(0, 0);
	}
	--it;
	return *it;
}

Rect2i Layout::getImageRect(std::size_t index) const
{
	const std::pair<std::size_t, int> line = findLine(index);
	const int x = (index - line.first) % columns_ * cellStepX_;
	const int y = (index - line.first) / columns_ * cellStepY_ + line.second;

	return Rect2i(x, y, x + cellImageWidth_, y + cellImageHeight_);
}

Rect2i Layout::getNameRect(std::size_t index) const
{
	const std::pair<std::size_t, int> line = findLine(index);
	const int x = (index - line.first) % columns_ * cellStepX_;
	const int y = (index - line.first) / columns_ * cellStepY_ + line.second
		+ cellImageHeight_;

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
