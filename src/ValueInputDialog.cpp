#include <vector>
#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "ValueInputDialog.h"


namespace piclist{

// --------------------------------------------------------------------------
// ValueInputDialog クラスの実装
// --------------------------------------------------------------------------

ValueInputDialog::ValueInputDialog(const String &prompt, const String &value)
	: prompt_(prompt), mode_(MODE_STRING), value_(value)
{}

ValueInputDialog::ValueInputDialog(const String &prompt, int value)
	: prompt_(prompt), mode_(MODE_INT), value_(std::to_wstring(static_cast<long long>(value)))
{}

ValueInputDialog::ValueInputDialog(const String &prompt, double value)
	: prompt_(prompt), mode_(MODE_DOUBLE), value_(std::to_wstring(static_cast<long double>(value)))
{}

bool ValueInputDialog::doModal(HWND parent)
{
	DialogTemplate dt(L"値の入力", 0, 0, 141, 60, 4);
	dt.addTextLabel(prompt_, ID_PROMPT, 7, 7, 127, 8);
	dt.addEditBox(value_, ID_EDITBOX, 7, 20, 127, 14);
	dt.addButton(L"OK", IDOK, 29, 39, 50, 14);
	dt.addButton(L"キャンセル", IDCANCEL, 84, 39, 50, 14);

	return openModal(parent, dt.getDlgTemplate()) == IDOK;
}


bool ValueInputDialog::onCommand(int notificationCode, int id, HWND hWndControl)
{
	switch(id){
	case IDOK:
		{
			TCHAR str[256] = {};
			::GetDlgItemText(getWindowHandle(), ID_EDITBOX, str, sizeof(str)/sizeof(str[0])-1);

			try{
				switch(mode_){
				case MODE_STRING:
					resultString_.assign(str);
					break;
				case MODE_INT:
					resultInt_ = std::stoi(str);
					break;
				case MODE_DOUBLE:
					resultDouble_ = std::stod(str);
					break;
				}
			}
			catch(const std::invalid_argument &){
				::MessageBox(getWindowHandle(), _T("書式が合いません。"), _T("エラー"), MB_OK);
				return TRUE;
			}
			catch(const std::out_of_range &){
				::MessageBox(getWindowHandle(), _T("値の範囲が不正です。"), _T("エラー"), MB_OK);
				return TRUE;
			}
			endDialog(IDOK);
		}
		return TRUE;
	case IDCANCEL:
		endDialog(IDCANCEL);
		return TRUE;
	}
	return FALSE;
}

}//namespace piclist
