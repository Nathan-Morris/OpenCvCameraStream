#include <functional>

#pragma once

class Byte
{
private:
	static class BitReference {
	private:
		unsigned char* mByte;
		unsigned char mBitPos;

	public:
		BitReference(unsigned char* byte, unsigned char bitPos);

		operator unsigned char() const {
			return (*this->mByte >> this->mBitPos) & 1;
		}

		BitReference& operator=(unsigned char val);
	};

private:
	unsigned char mByte = 0;

public:
	Byte();
	Byte(unsigned char byte);

	operator unsigned char() const {
		return this->mByte;
	}

	BitReference operator[](unsigned char idx);

	Byte& operator=(unsigned char byte);
};