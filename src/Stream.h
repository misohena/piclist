#ifndef STREAM_H
#define STREAM_H

#include <vector>
#include "String.h"

namespace piclist{

class VectorOutputStream
{
	std::vector<unsigned char> buffer_;
public:
	std::vector<unsigned char> &getBuffer() { return buffer_;}

	void writeUInt32(unsigned int v)
	{
		buffer_.push_back(v & 0xff);
		buffer_.push_back((v>>8) & 0xff);
		buffer_.push_back((v>>16) & 0xff);
		buffer_.push_back((v>>24) & 0xff);
	}
	void writeString(const String &s)
	{
		writeUInt32(s.size() * sizeof(String::value_type));
		if(!s.empty()){
			buffer_.insert(
				buffer_.end(),
				reinterpret_cast<const unsigned char *>(&s[0]),
				reinterpret_cast<const unsigned char *>(&s[0] + s.size()));
		}
	}
};

class MemoryInputStream
{
	bool error_;
	unsigned char *p_;
	unsigned char *end_;
public:
	MemoryInputStream(unsigned char *p, unsigned char *end)
		: error_(false), p_(p), end_(end)
	{}
	void setError()
	{
		error_ = true;
	}
	bool hasError() const
	{
		return error_;
	}
	unsigned int readUInt8()
	{
		if(p_ != end_){
			return *(p_++);
		}
		else{
			setError();
			return 0;
		}
	}
	unsigned int readUInt32()
	{
		const unsigned int a = readUInt8();
		const unsigned int b = readUInt8();
		const unsigned int c = readUInt8();
		const unsigned int d = readUInt8();
		return a | (b<<8) | (c<<16) | (d<<24);
	}
	String readString()
	{
		const unsigned int bytes = readUInt32();
		if(bytes == 0){
			return String();
		}

		std::vector<String::value_type> chars((bytes + sizeof(String::value_type) - 1) / sizeof(String::value_type));

		if(static_cast<std::size_t>(end_ - p_) < bytes){
			setError();
			return String();
		}

		std::memcpy(&chars[0], p_, bytes);
		p_ += bytes;
		return String(chars.begin(), chars.end());
	}
};

}//namespace piclist
#endif
