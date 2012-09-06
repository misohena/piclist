#include <Windows.h>
#include "Menu.h"

namespace piclist{

Menu::Menu()
	: owner_(false)
	, hMenu_(NULL)
{}

Menu::~Menu()
{
	reset(NULL, false);
}

void Menu::load(int resourceId)
{
	reset(::LoadMenu(::GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId)), true);
}

void Menu::setSubMenu(const Menu &menu, int nPos)
{
	reset(::GetSubMenu(menu.hMenu_, nPos), false);
}

void Menu::reset(HMENU hMenu, bool owner)
{
	if(owner_ && hMenu_){
		::DestroyMenu(hMenu_);
	}
	hMenu_ = hMenu;
	owner_ = hMenu && owner;
}

void Menu::checkItem(int id, bool b)
{
	if(hMenu_){
		::CheckMenuItem(hMenu_, id, b ? MF_CHECKED : 0);
	}
}

void Menu::popupMenu(int x, int y, HWND hParent)
{
	if(hMenu_){
		::TrackPopupMenu(hMenu_, 0, x, y, 0, hParent, NULL);
	}
}


}//namespace piclist
