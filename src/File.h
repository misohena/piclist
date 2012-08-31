#ifndef FILE_H
#define FILE_H

#include <windows.h>
#include <tchar.h>
#include "String.h"

namespace piclist{

	class FileEnumerator
	{
		String directory_;
		HANDLE handle_;
		WIN32_FIND_DATA data_; //valid()のときのみ有効。
	public:
		explicit FileEnumerator(const String &searchPath)
			: handle_(INVALID_HANDLE_VALUE)
		{
			TCHAR fullpath[MAX_PATH];
			LPTSTR filepart = NULL;
			if(::GetFullPathName(searchPath.c_str(), MAX_PATH, fullpath, &filepart) > MAX_PATH){
				return;
			}
			directory_.assign(fullpath, filepart);

			handle_ = ::FindFirstFile(searchPath.c_str(), &data_);
			makeEntry();
		}

		~FileEnumerator()
		{
			close();
		}

		bool valid() const
		{
			return handle_ != INVALID_HANDLE_VALUE;
		}

		void increment()
		{
			next();
			makeEntry();
		}

		String getEntryFileName(void) const
		{
			return valid() ? String(data_.cFileName) : String();
		}

		String getEntryFilePath(void) const
		{
			return valid() ? directory_ + data_.cFileName : String();
		}

	private:
		void close(void)
		{
			if(valid()){
				::FindClose(handle_);
				handle_ = INVALID_HANDLE_VALUE;
			}
		}

		void next(void)
		{
			if(valid()){
				if(!::FindNextFile(handle_, &data_)){
					close(); //!valid()になる。
				}
			}
		}

		void makeEntry()
		{
			//validな . や .. は除外する。
			while(valid() && (lstrcmp(data_.cFileName, _T(".")) == 0 || lstrcmp(data_.cFileName, _T("..")) == 0)){
				next();
			}
		}
	};

}//namespace piclist
#endif
