#include "Byte.h"

Byte::BitReference::BitReference(unsigned char* byte, unsigned char bitPos) : mByte(byte), mBitPos(bitPos) { }

Byte::BitReference& Byte::BitReference::operator=(unsigned char val) {
	if (val) {
		*this->mByte |= (1 << this->mBitPos);
	}
	else {
		*this->mByte &= ~(1 << this->mBitPos);
	}
	return *this;
}

//
//
//

Byte::Byte() { }

Byte::Byte(unsigned char byte) : mByte(byte) { }

Byte::BitReference Byte::operator[](unsigned char idx) {
	return Byte::BitReference(&this->mByte, (unsigned char)(idx & 7));
}

Byte& Byte::operator=(unsigned char byte) {
	this->mByte = byte;
	return *this;
}