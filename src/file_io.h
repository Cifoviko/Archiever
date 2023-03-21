#pragma once

#include "file_io_error.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

enum IOMode { IO_READ, IO_WRITE, IO_CLOSED };

class BitFileIO {  // Little-endian format
public:
    explicit BitFileIO(std::string file_name = "", IOMode mode = IOMode::IO_CLOSED);
    ~BitFileIO();

    void OpenFile(std::string file_name, IOMode mode);
    void CloseFile();

    void WriteWord(uint32_t value, uint8_t n_bit);   // Writing up to 256 bits
    void WriteWord(const std::vector<bool>& value);  // Writing up to size_t bits
    int32_t ReadWord(uint8_t n_bit);                 // Reading up to 256 bits

private:
    void BufferDump();    // Buffer dumping
    bool BufferRefill();  // Buffer refilling

    uint8_t buffer_;
    uint8_t position_;
    IOMode io_mode_ = IO_CLOSED;
    std::fstream stream_;
    std::string file_name_;
};