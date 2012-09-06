#ifndef MENU_H
#define MENU_H

namespace piclist{


	class Menu
	{
		bool owner_;
		HMENU hMenu_;
	public:
		Menu();
		~Menu();

		void load(int resourceId);
		void setSubMenu(const Menu &menu, int nPos);
		void reset(HMENU hMenu, bool owner);

		void checkItem(int id, bool b);
		void popupMenu(int x, int y, HWND hParent);
	private:
		Menu(const Menu &);
		Menu &operator=(const Menu &);
	};

}//namespace piclist
#endif
