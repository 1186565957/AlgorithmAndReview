/**
 * @brief A simple memeory-pool structure, which is without the memalign and the memory uncontiguous
 *  just a single demo  wrote in my university
 *
 */

/*
	环形缓冲区，用以克服TCP读取和接受写入的速度问题。

*/

#pragma once

#include <memory>
#include <mutex>
#include <stdint.h>
#include <cstring>
#include <algorithm>

namespace circle_buffer
{
	class CircleBuffer
	{
	public:
		enum
		{
			enmDefaultBufferLen = 72560,
		};

	public:
		CircleBuffer(const uint32_t size = enmDefaultBufferLen);
		~CircleBuffer();

		uint32_t Put(const char *buff, const uint32_t buffLen);
		uint32_t Get(char buf[], const uint32_t maxBuffLen);
		uint32_t Size()
		{
			return size;
		}
		uint32_t Abandoned(const uint32_t buffLen);

		uint32_t EmptySize();
		uint32_t UsedSize();
		const char *Buffer()
		{
			return buffer;
		}

		bool isEmpty()
		{
			return out >= in ? true : false;
		}

	private:
		uint32_t size;

		//两个缓冲区标识符量
		volatile uint32_t in;
		volatile uint32_t out;
		char *buffer;
	};

	CircleBuffer::CircleBuffer(const uint32_t size)
		: in(0), out(0), size(0)
	{
		buffer = new (std::nothrow) char[size];
		if (!buffer)
		{
			return;
		}
		memset(buffer, 0, size);
		this->size = size;
	}

	CircleBuffer::~CircleBuffer()
	{
		if (buffer)
		{
			delete[] buffer;
		}
	}

	uint32_t CircleBuffer::Put(const char *buff, const uint32_t bufferLen)
	{
		uint32_t lengthToPut = std::min(bufferLen, EmptySize());

		// Starting puts data from labol of in
		uint32_t len = std::min(lengthToPut, size - (in % size));
		memcpy(buffer + (in % size), buff, len);

		// Moveing the rest data to the beginning of the buffer
		memcpy(buffer, buff + len, lengthToPut - len);
		in += lengthToPut;
		return lengthToPut;
	}
	uint32_t CircleBuffer::Get(char buf[], const uint32_t maxBuffLen)
	{
		uint32_t lengthToGet = std::min(maxBuffLen, UsedSize());

		uint32_t len = std::min(lengthToGet, size - (out % size));
		memcpy(buf, buffer + out % size, len);

		memcpy(buf + len, buffer, lengthToGet - len);
		out += lengthToGet;
		return lengthToGet;
	}

	uint32_t CircleBuffer::Abandoned(const uint32_t bufferLen)
	{

		char tempBuff[65535] = {0};

		uint32_t lengthToAbandon = std::min(bufferLen, UsedSize());
		uint32_t len = std::min(lengthToAbandon, size - (out % size));
		memcpy(tempBuff, buffer + out % size, len);
		memcpy(tempBuff + len, buffer, lengthToAbandon - len);
		out += lengthToAbandon;
		return lengthToAbandon;
	}

	uint32_t CircleBuffer::EmptySize()
	{
		return size - in + out;
	}
	uint32_t CircleBuffer::UsedSize()
	{
		return in - out;
	}
}