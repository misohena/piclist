#include <algorithm>
#include <numeric>
#include "Layout.h"

namespace piclist{

Layout::Layout()
	: imageWidth_(96)
	, imageHeight_(96)
	, nameHeight_(10)
	, lineSpace_(0)
	, columnSpace_(1)
	, itemCount_()
	, columns_(1)
	, pageSize_()
{
	updateCellLayout();
}

void Layout::updateCellLayout()
{
	cellWidth_ = imageWidth_;
	cellHeight_ = (isImageHeightAuto() ? imageWidth_ : imageHeight_) + nameHeight_;
	cellStepX_ = cellWidth_ + columnSpace_;
	cellStepY_ = cellHeight_ + lineSpace_;
}

void Layout::update(const AlbumItemContainer &items, const Size2i &clientSize, ImageCache &imageCache)
{
	updateCellLayout();

	lines_.clear();
	itemCount_ = items.size();
	columns_ = std::max(1, (clientSize.w + columnSpace_) / cellStepX_);

	typedef AlbumItemContainer::const_iterator ItemIt;

	class LineDivider
	{
		const unsigned int columns_;
		const ItemIt beg_;
		const ItemIt end_;
		ItemIt curr_;
	public:
		LineDivider(unsigned int columns, const ItemIt &beg, const ItemIt &end)
			: columns_(columns), beg_(beg), end_(end), curr_(beg) {}

		bool isTerminated() const { return curr_ == end_;}
		const ItemIt &getCurrent() const { return curr_;}
		ItemIt getLineEnd()
		{
			unsigned int count = 0;
			for(;;){
				if(curr_ == end_){
					return curr_;
				}
				if((*curr_)->isLineBreak()){
					const ItemIt lineEnd = curr_;
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

	LineDivider lineDivider(columns_, items.begin(), items.end());

	int lineTop = 0;
	while(!lineDivider.isTerminated()){
		const ItemIt lineBeg = lineDivider.getCurrent();
		const ItemIt lineEnd = lineDivider.getLineEnd();

		// 画像の高さがAUTOの場合、行内の最大の画像の高さを求める。
		if(isImageHeightAuto()){
			const int maxImageHeight = std::accumulate(lineBeg, lineEnd, 0, [&](int maxImageHeight, const AlbumItemPtr &item) -> int {
				switch(item->getType()){
				case AlbumItem::TYPE_LINE_BREAK:
					break;
				case AlbumItem::TYPE_PICTURE:
					if(const AlbumPicture *pic = dynamic_cast<const AlbumPicture *>(item.get())){
						if(const ImagePtr im = imageCache.getImage(pic->getFilePath(), Size2i(getImageWidth(), getImageHeight()))){
							return std::max(maxImageHeight, im->getHeight());
						}
					}
					break;
				}
				return maxImageHeight;
			});
			lines_.insert(LineContainer::value_type(lineBeg - items.begin(), LineInfo(lineTop, maxImageHeight)));
			lineTop += maxImageHeight + nameHeight_ + lineSpace_;
		}
		else{
			lines_.insert(LineContainer::value_type(lineBeg - items.begin(), LineInfo(lineTop, imageHeight_)));
			lineTop += cellStepY_;
		}

	}

	pageSize_.set(
		columns_ * cellStepX_ - columnSpace_,
		lineTop ? lineTop - lineSpace_ : 0);
}

const Size2i Layout::getPageSize() const
{
	return pageSize_;
}

std::pair<std::size_t, Layout::LineInfo> Layout::findLine(std::size_t index) const
{
	if(lines_.empty()){
		return std::pair<std::size_t, LineInfo>(0, LineInfo(0, 0));
	}

	LineContainer::const_iterator it = lines_.lower_bound(index);
	if(it == lines_.end()){
		return *lines_.rbegin();
	}
	if(it->first == index){
		return *it;
	}
	if(it == lines_.begin()){
		return std::pair<std::size_t, LineInfo>(0, LineInfo(0, 0));
	}
	--it;
	return *it;
}

Rect2i Layout::getImageRect(std::size_t index) const
{
	const std::pair<std::size_t, LineInfo> line = findLine(index);
	const int x = (index - line.first) % columns_ * cellStepX_;
	const int y = (index - line.first) / columns_ * cellStepY_ + line.second.cellTop;

	return Rect2i(x, y, x + imageWidth_, y + line.second.imageHeight);
}

Rect2i Layout::getNameRect(std::size_t index) const
{
	const std::pair<std::size_t, LineInfo> line = findLine(index);
	const int x = (index - line.first) % columns_ * cellStepX_;
	const int y = (index - line.first) / columns_ * cellStepY_ + line.second.cellTop
		+ line.second.imageHeight;

	return Rect2i(x, y, x + cellWidth_, y + nameHeight_);
}

int Layout::getImageWidth() const
{
	return imageWidth_;
}
int Layout::getImageHeight() const
{
	return imageHeight_;
}

bool Layout::isImageHeightAuto() const
{
	return imageHeight_ < 0;
}

int Layout::getImageHeightMax() const
{
	return imageHeight_ < 0 ? -imageHeight_ : imageHeight_;
}

int Layout::getNameHeight(void) const
{
	return nameHeight_;
}

int Layout::getCellStepY(void) const
{
	return cellStepY_;
}


String Layout::getLayoutParamId(LayoutParamType lpt)
{
	switch(lpt){
	case LP_IMAGE_WIDTH: return STRING_LIT("ImageWidth");
	case LP_IMAGE_HEIGHT: return STRING_LIT("ImageHeight");
	case LP_NAME_HEIGHT: return STRING_LIT("NameHeight");
	case LP_COLUMN_SPACE: return STRING_LIT("ColumnSpace");
	case LP_LINE_SPACE: return STRING_LIT("LineSpace");
	default: return String();
	}
}

String Layout::getLayoutParamName(LayoutParamType lpt)
{
	switch(lpt){
	case LP_IMAGE_WIDTH: return STRING_LIT("画像の幅");
	case LP_IMAGE_HEIGHT: return STRING_LIT("画像の高さ(負の時、-高さ以内で自動)");
	case LP_NAME_HEIGHT: return STRING_LIT("ファイル名の高さ");
	case LP_COLUMN_SPACE: return STRING_LIT("桁間スペース");
	case LP_LINE_SPACE: return STRING_LIT("行間スペース");
	default: return String();
	}
}

int Layout::getLayoutParam(LayoutParamType lpt)
{
	switch(lpt){
	case LP_IMAGE_WIDTH: return imageWidth_;
	case LP_IMAGE_HEIGHT: return imageHeight_;
	case LP_NAME_HEIGHT: return nameHeight_;
	case LP_COLUMN_SPACE: return columnSpace_;
	case LP_LINE_SPACE: return lineSpace_;
	default: return 0;
	}
}

void Layout::setLayoutParam(LayoutParamType lpt, int value)
{
	switch(lpt){
	case LP_IMAGE_WIDTH: imageWidth_ = value; break;
	case LP_IMAGE_HEIGHT: imageHeight_ = value; break;
	case LP_NAME_HEIGHT: nameHeight_ = value; break;
	case LP_COLUMN_SPACE: columnSpace_ = value; break;
	case LP_LINE_SPACE: lineSpace_ = value; break;
	}
}




}//namespace piclist
