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
		enum LayoutParamType
		{
			LP_IMAGE_WIDTH,
			LP_IMAGE_HEIGHT,
			LP_NAME_HEIGHT,
			LP_COLUMN_SPACE,
			LP_LINE_SPACE,
			LP_COUNT
		};
	private:
		int imageWidth_;
		int imageHeight_;
		int nameHeight_;
		int lineSpace_;
		int columnSpace_;
		int cellWidth_;
		int cellHeight_;
		int cellStepX_;
		int cellStepY_;

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

		static String getLayoutParamId(LayoutParamType lpt);
		static String getLayoutParamName(LayoutParamType lpt);
		int getLayoutParam(LayoutParamType lpt);
		void setLayoutParam(LayoutParamType lpt, int value);
	private:
		void updateCellLayout();
		std::pair<std::size_t, int> findLine(std::size_t index) const;

	};

}//namespace piclist
#endif
