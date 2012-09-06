#ifndef LAYOUT_H
#define LAYOUT_H

#include <map>
#include "Math.h"
#include "Album.h"

namespace piclist{

	/**
	 * AppWindowの中の画像の並び方を計算・保持するクラスです。
	 */
	class Layout
	{
	public:
		static const int IMAGE_HEIGHT_AUTO = -1;
	private:
		const int cellImageWidth_;
		const int cellImageHeight_;
		const int cellNameHeight_;
		const int cellWidth_;
		const int cellHeight_;
		const int lineSpace_;
		const int columnSpace_;
		const int cellStepX_;
		const int cellStepY_;

		std::size_t itemCount_;
		unsigned int columns_;
		Size2i pageSize_;

		typedef std::map<std::size_t, int> LineContainer;
		LineContainer lines_; //AlbumItemインデックス -> Y座標 マップ
	public:
		Layout();
		void update(const AlbumItemContainer &items, const Size2i &clientSize);
		const Size2i getPageSize() const;

		Rect2i getImageRect(std::size_t index) const;
		Rect2i getNameRect(std::size_t index) const;
		int getImageWidth() const;
		int getImageHeight() const;
		int getNameHeight() const;
		int getCellStepY() const;
	private:
		std::pair<std::size_t, int> findLine(std::size_t index) const;

	};

}//namespace piclist
#endif
