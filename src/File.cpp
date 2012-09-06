#include <vector>
#include <cstdlib>
#include <windows.h>
#include <tchar.h>
#include "File.h"

namespace piclist{

// ----------------------------------------------------------------------
// Current Directory
// ----------------------------------------------------------------------

String getCurrentDirectory()
{
	const DWORD size = ::GetCurrentDirectory(0, NULL);
	if(size == 0){
		return String();
	}
	std::vector<TCHAR> buffer(size);
	const DWORD size2 = ::GetCurrentDirectory(size, &buffer[0]);
	if(size2 == 0 || size2 >= size){
		return String();
	}
	return &buffer[0];
}

bool setCurrentDirectory(const String &dir)
{
	return !!::SetCurrentDirectory(dir.c_str());
}

// ----------------------------------------------------------------------
// filename component
// ----------------------------------------------------------------------

String getFileNameBase(const String &filepath)
{
	String::value_type base[2048];
	if(_wsplitpath_s(filepath.c_str(), NULL, 0, NULL, 0, base, sizeof(base)/sizeof(base[0]), NULL, 0)){
		return String();
	}
	return base;
}

// ----------------------------------------------------------------------
// File Enumerator
// ----------------------------------------------------------------------

struct FileEnumerator::Impl
{
	String directory_;
	HANDLE handle_;
	WIN32_FIND_DATA data_; //valid()のときのみ有効。
	Impl()
		: handle_(INVALID_HANDLE_VALUE)
	{}
};

FileEnumerator::FileEnumerator(const String &searchPath)
	: impl_(new Impl())
{
	TCHAR fullpath[MAX_PATH];
	LPTSTR filepart = NULL;
	if(::GetFullPathName(searchPath.c_str(), MAX_PATH, fullpath, &filepart) > MAX_PATH){
		return;
	}
	impl_->directory_.assign(fullpath, filepart);

	impl_->handle_ = ::FindFirstFile(searchPath.c_str(), &(impl_->data_));
	makeEntry();
}

FileEnumerator::~FileEnumerator()
{
	close();
}

bool FileEnumerator::valid() const
{
	return impl_->handle_ != INVALID_HANDLE_VALUE;
}

void FileEnumerator::increment()
{
	next();
	makeEntry();
}

String FileEnumerator::getEntryFileName(void) const
{
	return valid() ? String(impl_->data_.cFileName) : String();
}

String FileEnumerator::getEntryFilePath(void) const
{
	return valid() ? impl_->directory_ + impl_->data_.cFileName : String();
}

void FileEnumerator::close(void)
{
	if(valid()){
		::FindClose(impl_->handle_);
		impl_->handle_ = INVALID_HANDLE_VALUE;
	}
}

void FileEnumerator::next(void)
{
	if(valid()){
		if(!::FindNextFile(impl_->handle_, &(impl_->data_))){
			close(); //!valid()になる。
		}
	}
}

void FileEnumerator::makeEntry()
{
	//validな . や .. は除外する。
	while(valid() && (lstrcmp(impl_->data_.cFileName, _T(".")) == 0 || lstrcmp(impl_->data_.cFileName, _T("..")) == 0)){
		next();
	}
}



}//namespace piclist

