#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <vector>
#include "String.h"
#include "Album.h"

namespace piclist{

class CommandLineParser
{
	String windowName_;
	AlbumItemContainer albumItems_;
	bool noClearFlag_;
public:
	CommandLineParser();
	void parse(const String::value_type *cmdLineStr);

	const String &getWindowName() const { return windowName_;}
	AlbumItemContainer getAlbum() const { return albumItems_;}
	bool getNoClearFlag() const { return noClearFlag_;}
};

}//namespace piclist
#endif
