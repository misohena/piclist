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

}//namespace piclist
#endif
