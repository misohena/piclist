#ifndef MATH_H
#define MATH_H

namespace piclist{

	/**
	 * �񎟌����s���W�n�ɂ����鎲�ɉ�������`�̃T�C�Y�ł��B
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
	 * �񎟌����s���W�n�ɂ����鎲�ɉ�������`��\���N���X�ł��B
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
