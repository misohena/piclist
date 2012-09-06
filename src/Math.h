#ifndef MATH_H
#define MATH_H

namespace piclist{

	/**
	 * 二次元直行座標系における軸に沿った矩形のサイズです。
	 */
	class Size2i
	{
	public:
		int w;
		int h;
		Size2i() : w(0), h(0){}
		Size2i(int w, int h) : w(w), h(h){}
		void set(int w_, int h_)
		{
			w = w_;
			h = h_;
		}
	};
	inline bool operator==(const Size2i &lhs, const Size2i &rhs){ return lhs.w == rhs.w && lhs.h == rhs.h;}
	inline bool operator!=(const Size2i &lhs, const Size2i &rhs){ return lhs.w != rhs.w || lhs.h != rhs.h;}

	/**
	 * 二次元直行座標系における軸に沿った矩形を表すクラスです。
	 */
	class Rect2i
	{
	public:
		int left;
		int top;
		int right;
		int bottom;
		Rect2i(int left, int top, int right, int bottom)
			: left(left), top(top), right(right), bottom(bottom)
		{}
		int getWidth() const { return right - left;}
		int getHeight() const { return bottom - top;}
	};

	/**
	 * 二次元座標系における点を表すクラスです。
	 */
	class Point2i
	{
	public:
		int x;
		int y;
		Point2i(int x, int y)
			: x(x), y(y)
		{}
		Point2i()
			: x(0), y(0)
		{}
	};
}//namespace piclist
#endif
