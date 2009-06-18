
#include "Buffer.hpp"

#include <iostream>
#include <iomanip>

using namespace backlot;

int main(int argc, char **argv)
{
	// Simple, byte-aligned
	std::cout << "Simple, aligned:" << std::endl;
	BufferPointer buffer = new Buffer();
	buffer->write8(0xC2);
	buffer->write32(0xDEADC0DE);
	buffer->write16(0xCAFE);
	if (buffer->getSize() != 7)
		std::cout << "Wrong buffer size (7): " << buffer->getSize() << std::endl;
	if (buffer->getPosition() != 56)
		std::cout << "Wrong buffer position (56): " << buffer->getSize() << std::endl;
	buffer->setPosition(0);
	if (buffer->getPosition() != 0)
		std::cout << "Wrong buffer position (0): " << buffer->getSize() << std::endl;
	std::cout << std::hex;
	uint8_t u8 = buffer->read8();
	if (u8 != 0xC2)
		std::cout << "Wrong data (0xC2): " << u8 << std::endl;
	uint32_t u32 = buffer->read32();
	if (u32 != 0xDEADC0DE)
		std::cout << "Wrong data (0xDEADC0DE): " << u32 << std::endl;
	uint16_t u16 = buffer->read16();
	if (u16 != 0xCAFE)
		std::cout << "Wrong data (0xCAFE): " << u16 << std::endl;
	// Unaligned
	std::cout << "Unaligned:" << std::endl;
	buffer = new Buffer();
	buffer->writeInt(0, 3);
	buffer->write8(0xC2);
	buffer->write32(0xDEADC0DE);
	buffer->write16(0xCAFE);
	if (buffer->getSize() != 8)
		std::cout << "Wrong buffer size (8): " << buffer->getSize() << std::endl;
	if (buffer->getPosition() != 59)
		std::cout << "Wrong buffer position (59): " << buffer->getSize() << std::endl;
	buffer->setPosition(0);
	buffer->readInt(3);
	u8 = buffer->read8();
	if (u8 != 0xC2)
		std::cout << "Wrong data (0xC2): " << u8 << std::endl;
	u32 = buffer->read32();
	if (u32 != 0xDEADC0DE)
		std::cout << "Wrong data (0xDEADC0DE): " << u32 << std::endl;
	u16 = buffer->read16();
	if (u16 != 0xCAFE)
		std::cout << "Wrong data (0xCAFE): " << u16 << std::endl;
	// Other sizes/corner cases
	std::cout << "Other sizes:" << std::endl;
	buffer = new Buffer();
	buffer->writeUnsignedInt(0x2BAD, 14);
	buffer->writeInt(0x2BAD, 14);
	buffer->writeInt(0xBADCAFE, 28);
	buffer->writeInt(0x1, 1);
	if (buffer->getSize() != 8)
		std::cout << "Wrong buffer size (8): " << buffer->getSize() << std::endl;
	if (buffer->getPosition() != 57)
		std::cout << "Wrong buffer position (57): " << buffer->getSize() << std::endl;
	buffer->setPosition(0);
	u16 = buffer->readUnsignedInt(14);
	if (u16 != 0x2BAD)
		std::cout << "Wrong data (0x2BAD): " << u16 << std::endl;
	std::cout << std::dec;
	int s32 = buffer->readInt(14);
	if (s32 != -1)
		std::cout << "Wrong data (-1): " << s32 << std::endl;
	std::cout << std::hex;
	u32 = buffer->readUnsignedInt(28);
	if (u32 != 0xBADCAFE)
		std::cout << "Wrong data (0xBADCAFE): " << u32 << std::endl;
	u8 = buffer->readUnsignedInt(1);
	if (u8 != 1)
		std::cout << "Wrong data (1): " << u8 << std::endl;
	return 0;
}