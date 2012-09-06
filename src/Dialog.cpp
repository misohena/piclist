#include <Windows.h>
#include "Dialog.h"

namespace piclist{


Dialog::Dialog()
	: hwnd_(NULL)
{}

Dialog::~Dialog()
{
}

bool Dialog::openModal(HWND parent, LPDLGTEMPLATE temp)
{
	HINSTANCE inst = GetModuleHandle(NULL);
	return IDOK == DialogBoxIndirectParam(inst, temp, parent,
		dlgprocStatic,
		reinterpret_cast<LPARAM>(this));
}

bool Dialog::openModal(HWND parent, LPCWSTR lpTemplateName)
{
	HINSTANCE inst = GetModuleHandle(NULL);
	return IDOK == DialogBoxParam(inst, lpTemplateName, parent,
		dlgprocStatic,
		reinterpret_cast<LPARAM>(this));
}

void Dialog::endDialog(INT_PTR result)
{
	if(hwnd_){
		::EndDialog(hwnd_, IDOK);
	}
}

INT_PTR CALLBACK Dialog::dlgprocStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_INITDIALOG){
		SetWindowLong(hwnd, GWL_USERDATA, lParam);
	}

	LONG ptr = GetWindowLong(hwnd, GWL_USERDATA);
	if(ptr != 0){
		Dialog *dlg = reinterpret_cast<Dialog*>(ptr);
		dlg->hwnd_ = hwnd;
		return dlg->dlgproc(hwnd, msg, wParam, lParam);
	}
	else{
		return 0;
	}
}

INT_PTR CALLBACK Dialog::dlgproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		onInitDialog();
		return TRUE;
	case WM_COMMAND:
		return onCommand(HIWORD(wparam), LOWORD(wparam), (HWND)lparam) ? TRUE : FALSE;
	}
	return FALSE;
}

void Dialog::onInitDialog(){}
bool Dialog::onCommand(int notificationCode, int id, HWND hWndControl){ return false;}

}//namespace piclist
