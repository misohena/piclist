#ifndef VALUEINPUTDIALOG_H
#define VALUEINPUTDIALOG_H

#include "Dialog.h"
#include "String.h"

namespace piclist{

	/**
	 * 一つの値(整数、浮動小数点数、文字列)を入力するダイアログボックスです。
	 */
	class ValueInputDialog : public Dialog
	{
		static const int ID_PROMPT = 1000;
		static const int ID_EDITBOX = 1001;
		String prompt_;
		String value_;

		enum Mode{
			MODE_STRING,
			MODE_INT,
			MODE_DOUBLE
		};
		Mode mode_;

		String resultString_;
		int resultInt_;
		double resultDouble_;

	public:
		ValueInputDialog(const String &prompt, const String &value);
		ValueInputDialog(const String &prompt, int value);
		ValueInputDialog(const String &prompt, double value);

		const String &getResultString(void) const { return resultString_;}
		int getResultInt(void) const { return resultInt_;}
		double getResultDouble(void) const { return resultDouble_;}

		bool doModal(HWND parent);
	protected:
		virtual bool onCommand(int notificationCode, int id, HWND hWndControl);
	};

}//namespace piclist
#endif
