#ifndef LAYOUT_H
#define LAYOUT_H

#include "Math.h"
#include "Picture.h"

namespace piclist{

	/**
	 * AppWindowの中の画像の並び方を計算・保持するクラスです。
	 */
	class Layout
	{
		const int cellImageWidth_;
		const int cellImageHeight_;
		const int cellNameHeight_;
		const int cellWidth_;
		const int cellHeight_;
		const int lineSpace_;
		const int columnSpace_;
		const int cellStepX_;
		const int cellStepY_;

		std::size_t pictureCount_;
		unsigned int columns_;
		Size2i pageSize_;
	public:
		Layout();
		void update(const PictureContainer &pictures, const Size2i &clientSize);
		const Size2i getPageSize() const;

		Rect2i getImageRect(std::size_t index) const;
		Rect2i getNameRect(std::size_t index) const;
		int getImageWidth() const;
		int getImageHeight() const;
		int getNameHeight() const;
		int getCellStepY() const;
	};

}//namespace piclist
#endif
