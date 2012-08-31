#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <vector>
#include "String.h"
#include "Picture.h"

namespace piclist{

class CommandLineParser
{
	String windowName_;
	std::vector<String> files_;
public:
	CommandLineParser();
	void parse(const String::value_type *cmdLineStr);

	const String &getWindowName() const { return windowName_;}
	PictureContainer getPictures() const;
};

}//namespace piclist
#endif
