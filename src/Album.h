#ifndef PICTURE_H
#define PICTURE_H

#include <memory>
#include <deque>
#include "File.h"

namespace piclist{
	class AlbumItem;
	typedef std::shared_ptr<AlbumItem> AlbumItemPtr;
	typedef std::deque<AlbumItemPtr> AlbumItemContainer;

	/**
	 * ウィンドウに表示する項目を表すベースクラスです。
	 */
	class AlbumItem
	{
	public:
		virtual ~AlbumItem(){}

		enum Type
		{
			TYPE_LINE_BREAK,
			TYPE_PICTURE
		};
		virtual Type getType() const = 0;
		bool isLineBreak() const { return getType() == TYPE_LINE_BREAK;}

		template<typename R, typename F>
		R dispatch(F &f)
		{
			switch(getType()){
			case TYPE_LINE_BREAK: return f(*static_cast<AlbumLineBreak *>(this)); break;
			case TYPE_PICTURE: return f(*static_cast<AlbumPicture *>(this)); break;
			default: throw std::runtime_error("unknown album type.");
			}
		}
		template<typename R, typename F>
		R dispatch(F &f) const
		{
			switch(getType()){
			case TYPE_LINE_BREAK: return f(*static_cast<const AlbumLineBreak *>(this)); break;
			case TYPE_PICTURE: return f(*static_cast<const AlbumPicture *>(this)); break;
			default: throw std::runtime_error("unknown album type.");
			}
		}
	};

	/**
	 * 改行を意味する項目です。
	 */
	class AlbumLineBreak : public AlbumItem
	{
	public:
		virtual Type getType() const
		{
			return TYPE_LINE_BREAK;
		}
		static std::shared_ptr<AlbumLineBreak> create()
		{
			return std::shared_ptr<AlbumLineBreak>(new AlbumLineBreak());
		}
	};

	/**
	 * 絵のファイルを表示する項目です。
	 */
	class AlbumPicture : public AlbumItem
	{
		String filepath_;
	public:
		AlbumPicture(const String &filepath)
			: filepath_(filepath)
		{}
		virtual Type getType() const
		{
			return TYPE_PICTURE;
		}
		const String &getFilePath() const
		{
			return filepath_;
		}
		String getFileNameBase() const
		{
			return piclist::getFileNameBase(filepath_);
		}
		static std::shared_ptr<AlbumPicture> create(const String &filepath)
		{
			return std::shared_ptr<AlbumPicture>(new AlbumPicture(filepath));
		}
	};


}//namespace piclist
#endif
