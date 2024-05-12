#include "ifstream12.h"
#include <iostream>
#include <cassert>
void ifstream12::reset() {
    /* for (size_t i = 0; i < fBufferSize; i++)
         fBuffer[i] &= std::byte{ 0 };*/
    fByteCount = 0;
    fByteIndex = 0;
    fBitIndex = 7;
}
void ifstream12::fetch_data() {
    fIStream.read(reinterpret_cast<char*>(fBuffer), fBufferSize);
    fByteCount = fIStream.gcount();
    fByteIndex = 0;
}
ifstream12::ifstream12(const char* aFileName, size_t aBufferSize) :
    fBuffer(new std::byte[aBufferSize]),
    fBufferSize(aBufferSize),

    fByteIndex(0),
    fBitIndex(7) {
    reset();
    open(aFileName);
}
ifstream12::~ifstream12() {
    close();
    delete[] fBuffer;

}
void ifstream12::open(const char* aFileName) {
   /* assert(!isOpen());*/
    if (aFileName) {
        fIStream.open(aFileName, std::ifstream::binary);
    }

}
void ifstream12::close() {
    if (isOpen()) {
        fIStream.close();
    }
    /* reset();*/
}
bool ifstream12::isOpen() const {
    return fIStream.is_open();
}

bool ifstream12::good() const {
    return fIStream.good();
}

bool ifstream12::eof() const {
    return (fByteCount == 0) && fIStream.eof();
}
std::optional<size_t> ifstream12::readBit() {
    if (fByteCount == 0) {
        fetch_data();
        if (fByteCount == 0)
            return std::nullopt; // End of file
    }

    std::byte lByte = fBuffer[fByteIndex] & (std::byte{ 1 } << fBitIndex);
    size_t result = std::to_integer<size_t>(lByte);
    fBitIndex--;

    if (fBitIndex < 0) {
        fByteIndex++;
        fBitIndex = 7;
        fByteCount--;

        if (eof())
            return std::nullopt; // End of file
    }

    return result;
}
ifstream12& ifstream12:: operator>>(size_t& aValue) {
    aValue = 0;
    for (int i = 11; i >= 0; --i) {
        auto bit = readBit();
        if (!bit.has_value()) {
            break; // End of file
        }
        aValue |= static_cast<size_t>(*bit) << i;
    }
   
    return *this;
}