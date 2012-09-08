#ifndef FILE_H
#define FILE_H

#include "String.h"

namespace piclist{

	// ----------------------------------------------------------------------
	// Current Directory
	// ----------------------------------------------------------------------

	String getCurrentDirectory();
	bool setCurrentDirectory(const String &dir);

	class ScopedCurrentDirectory
	{
		String oldDir_;
	public:
		explicit ScopedCurrentDirectory(const String &newDir)
			: oldDir_(getCurrentDirectory())
		{
			setCurrentDirectory(newDir);
		}
		~ScopedCurrentDirectory()
		{
			setCurrentDirectory(oldDir_);
		}
	private:
		ScopedCurrentDirectory(const ScopedCurrentDirectory &);
		ScopedCurrentDirectory &operator=(const ScopedCurrentDirectory &);
	};

	// ----------------------------------------------------------------------
	// filename component
	// ----------------------------------------------------------------------

	String getFullPathName(const String &filepath);

	String getFileNameBase(const String &filepath);
	String getFileNameExtension(const String &filepath);
	bool hasSupportedImageFileExtension(const String &filepath);

	// ----------------------------------------------------------------------
	// file type
	// ----------------------------------------------------------------------

	bool isExistingDirectory(const String &filepath);
	bool isExistingRegularFile(const String &filepath);

	// ----------------------------------------------------------------------
	// File Enumerator
	// ----------------------------------------------------------------------
	class FileEnumerator
	{
		struct Impl;
		std::unique_ptr<Impl> impl_;
	public:
		explicit FileEnumerator(const String &searchPath);
		~FileEnumerator();
		bool valid() const;
		void increment();
		String getEntryFileName() const;
		String getEntryFilePath() const;
	private:
		void close();
		void next();
		void makeEntry();
	};

}//namespace piclist
#endif
