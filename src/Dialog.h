#ifndef DIALOG_H
#define DIALOG_H

#include <vector>
#include <string>

namespace piclist{

	/**
	 * Win32ダイアログボックスを実装しやすくするためのベースクラスです。
	 */
	class Dialog
	{
	    HWND hwnd_;
	public:
		Dialog();
		~Dialog();

		HWND getWindowHandle() const { return hwnd_;}

		bool openModal(HWND parent, LPDLGTEMPLATE temp);
		bool openModal(HWND parent, LPCWSTR lpTemplateName);

		void endDialog(INT_PTR result);
	private:
		static INT_PTR CALLBACK dlgprocStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		INT_PTR CALLBACK dlgproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	protected:
		virtual void onInitDialog();
		virtual bool onCommand(int notificationCode, int id, HWND hWndControl);
	};


	/**
	 * ダイアログテンプレートを作成するためのクラスです。
	 * 作成したテンプレートはDialogBoxIndirectやCreateDialogIndirectで使えます。
	 */
	class DialogTemplate
	{
		std::vector<WORD> temp_;
	public:
		DialogTemplate(const std::wstring &title, int x, int y, int cx, int cy, int numberOfItems)
		{
			genDlgTemplate(temp_, title, x, y, cx, cy, numberOfItems);
		}

		LPDLGTEMPLATE getDlgTemplate()
		{
			return reinterpret_cast<LPDLGTEMPLATE>(&temp_[0]);
		}

		void addItem(const std::wstring &classId, const std::wstring &text, DWORD id, int x, int y, int cx, int cy, DWORD style)
		{
			genDlgItem(temp_, classId, text, id, x, y, cx, cy, style);
		}

		void addButton(const std::wstring &text, DWORD id, int x, int y, int cx, int cy)
		{
			addItem(L"BUTTON", text, id, x, y, cx, cy, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | WS_TABSTOP);
		}
		void addTextLabel(const std::wstring &text, DWORD id, int x, int y, int cx, int cy)
		{
			addItem(L"STATIC", text, id, x, y, cx, cy, WS_VISIBLE);
		}
		void addEditBox(const std::wstring &text, DWORD id, int x, int y, int cx, int cy)
		{
			addItem(L"EDIT", text, id, x, y, cx, cy, WS_VISIBLE);
		}

	private:
		//
		// see: http://support.microsoft.com/kb/141201
		//
		static void genDlgTemplate(std::vector<WORD> &dst, const std::wstring &title, int x, int y, int cx, int cy, int numberOfItems)
		{
			const DWORD lStyle =
				DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS |
				WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
			dst.push_back(1);      // DlgVer
			dst.push_back(0xFFFF); // Signature
			dst.push_back(0);      // LOWORD HelpID
			dst.push_back(0);      // HIWORD HelpID
			dst.push_back(0);      // LOWORD (lExtendedStyle)
			dst.push_back(0);      // HIWORD (lExtendedStyle)
			dst.push_back(LOWORD(lStyle));
			dst.push_back(HIWORD(lStyle));
			dst.push_back(numberOfItems); // NumberOfItems
			dst.push_back(x);      // x
			dst.push_back(y);      // y
			dst.push_back(cx);     // cx
			dst.push_back(cy);     // cy
			dst.push_back(0);      // Menu
			dst.push_back(0);      // Class

			// text
			dst.insert(dst.end(), title.begin(), title.end());
			dst.push_back(L'\0');

			// Font information because of DS_SETFONT.
			dst.push_back(8);  // Point size
			dst.push_back(FW_DONTCARE);  // Weight
			dst.push_back(MAKEWORD( FALSE, DEFAULT_CHARSET ));  // italic flag and charset.

			const std::wstring fontName = L"MS Shell Dlg";
			dst.insert(dst.end(), fontName.begin(), fontName.end());
			dst.push_back(L'\0');

			// Make sure the first item starts on a DWORD boundary.
			if(dst.size() & 1){
				dst.push_back(0);
			}
		}

		static void genDlgItem(std::vector<WORD> &dst, const std::wstring &classId, const std::wstring &text, DWORD id, int x, int y, int cx, int cy, DWORD style)
		{
			dst.push_back(0);         // LOWORD (lHelpID)
			dst.push_back(0);         // HIWORD (lHelpID)
			dst.push_back(0);         // LOWORD (lExtendedStyle)
			dst.push_back(0);         // HIWORD (lExtendedStyle)
			dst.push_back(LOWORD(style));
			dst.push_back(HIWORD(style));
			dst.push_back(x);         // x
			dst.push_back(y);         // y
			dst.push_back(cx);        // cx
			dst.push_back(cy);        // cy
			dst.push_back(LOWORD(id));  // LOWORD (Control ID)
			dst.push_back(HIWORD(id));  // HOWORD (Control ID)

			dst.insert(dst.end(), classId.begin(), classId.end());
			dst.push_back(L'\0');

			// Copy the text of the item.
			dst.insert(dst.end(), text.begin(), text.end());
			dst.push_back(L'\0');

			///@todo extra data support
			dst.push_back(0);// Advance pointer over nExtraStuff WORD.

			// make sure the next item starts on a DWORD boundary.
			if(dst.size() & 1){
				dst.push_back(0);
			}
		}
	};


}//namespace piclist
#endif
