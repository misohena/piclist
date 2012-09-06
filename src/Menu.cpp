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

void Menu::reset(HMENU hMenu, bool owner)
{
	if(owner_ && hMenu_){
		::DestroyMenu(hMenu_);
	}
	hMenu_ = hMenu;
	owner_ = hMenu && owner;
}

void Menu::popupSubMenu(int nPos, int x, int y, HWND hParent)
{
	if(!hMenu_){
		return;
	}
	if(HMENU hSubMenu = ::GetSubMenu(hMenu_, 0)){
		::TrackPopupMenu(hSubMenu, 0, x, y, 0, hParent, NULL);
	}
}


}//namespace piclist
