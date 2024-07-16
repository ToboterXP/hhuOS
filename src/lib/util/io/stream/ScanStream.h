#ifndef HHUOS_SCANSTREAM_H
#define HHUOS_SCANSTREAM_H


#include <cstdint>

#include "InputStream.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/ArrayList.h"

namespace Util::Io {

class ScanStream : public InputStream {

public:
    explicit ScanStream(InputStream &stream);

    ScanStream(const ScanStream &copy) = delete;

    ScanStream &operator=(const ScanStream &copy) = delete;

    ~ScanStream() override = default;

    int16_t read() override;
	
	int16_t peek() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;
	
	uint32_t getReadBytes();
	
	void setReadLimit(uint32_t limit); //limit is compared to readBytes, -1 = no limit
	
	long long readLong(int base=0);
	
	int32_t readInt(int base=0);
	
	uint32_t readUint(int base=0);
	
	double readDouble();

	

private:
	uint32_t readChars = 0;
	
	uint32_t readLimit = -1; //-1 = no limit 
	
    InputStream &stream;
};

}


#endif