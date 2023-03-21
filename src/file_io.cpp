#include "file_io.h"

#include <climits>

BitFileIO::BitFileIO(std::string file_name, IOMode mode) : file_name_(file_name) {
    OpenFile(file_name, mode);
}

BitFileIO::~BitFileIO() {
    CloseFile();
}

void BitFileIO::OpenFile(std::string file_name, IOMode mode) {
    // [begin] Check if correct call
    if (mode == IO_CLOSED) {
        return;
    } else if (io_mode_ != IO_CLOSED) {
        throw FileIOError(FileIOError::TRYING_TO_OPEN_OPEN_FILE);
    }
    // [end] Check if correct call

    file_name_ = file_name;
    io_mode_ = mode;
    buffer_ = 0;
    position_ = 0;
    if (io_mode_ == IO_READ) {
        stream_.open(file_name, std::ios::in | std::ios::binary);
    } else {
        stream_.open(file_name, std::ios::out | std::ios::binary);
    }
    if (!stream_.is_open()) {
        throw FileIOError(FileIOError::CANT_OPEN_FILE, file_name);
    }
}

void BitFileIO::CloseFile() {
    if (io_mode_ == IO_CLOSED) {
        return;
    }

    if ((io_mode_ == IO_WRITE) && (position_ > 0)) {  // Dumping buffer if left
        char v = static_cast<char>(buffer_);
        stream_.write(&v, sizeof(v));
    }
    file_name_ = "";
    stream_.close();
    io_mode_ = IO_CLOSED;
}

void BitFileIO::BufferDump() {  // Buffer dumping
    char v = static_cast<char>(buffer_);
    stream_.write(&v, sizeof(v));
    position_ = 0;
    buffer_ = 0;
}

void BitFileIO::WriteWord(uint32_t value, uint8_t n_bit) {
    // [begin] Check if correct call
    if (io_mode_ == IO_CLOSED) {
        throw FileIOError(FileIOError::NOT_OPEN_FILE);
    } else if (io_mode_ == IO_READ) {
        throw FileIOError(FileIOError::WRITE_IN_READ_ONLY, file_name_);
    }
    // [end] Check if correct call

    for (size_t i = 0; i < n_bit; ++i) {
        buffer_ += (((value >> (n_bit - 1 - i)) & 0x01) << (CHAR_BIT - 1 - position_++));
        if (position_ >= CHAR_BIT) {  // Check if buffer is filled
            BufferDump();             // Dumping buffer
        }
    }

    if (stream_.bad()) {
        throw FileIOError(FileIOError::INVALID_FILE, file_name_);
    }
}

void BitFileIO::WriteWord(const std::vector<bool>& value) {
    // [begin] Check if correct call
    if (io_mode_ == IO_CLOSED) {
        throw FileIOError(FileIOError::NOT_OPEN_FILE);
    } else if (io_mode_ == IO_READ) {
        throw FileIOError(FileIOError::WRITE_IN_READ_ONLY, file_name_);
    }
    // [end] Check if correct call

    size_t n_bit = value.size();
    for (size_t i = 0; i < n_bit; ++i) {
        buffer_ += ((value[i] ? 1 : 0) << (CHAR_BIT - 1 - position_++));
        if (position_ >= CHAR_BIT) {  // Check if buffer is filled
            BufferDump();             // Dumping buffer
        }
    }

    // [begin] Check if correct execution
    if (stream_.bad()) {
        throw FileIOError(FileIOError::INVALID_FILE, file_name_);
    }
    // [end] Check if correct execution
}

bool BitFileIO::BufferRefill() {  // Buffer refilling
    unsigned char v = 0;
    stream_.read(reinterpret_cast<char*>(&v), sizeof(v));
    if (stream_.eof()) {  // Check on file end
        return false;
    }
    position_ = 0;
    buffer_ = static_cast<uint8_t>(v);
    return true;
}

int32_t BitFileIO::ReadWord(uint8_t n_bit) {
    // [begin] Check if correct call
    if (io_mode_ == IO_CLOSED) {
        throw FileIOError(FileIOError::NOT_OPEN_FILE);
    } else if (io_mode_ == IO_WRITE) {
        throw FileIOError(FileIOError::READ_IN_WRITE_ONLY, file_name_);
    }
    // [end] Check if correct call

    int32_t result = 0;
    for (size_t i = 0; i < n_bit; ++i) {
        if (position_ == 0) {       // Check if buffer is empty
            if (!BufferRefill()) {  // Check if file ended
                return -1;
            }
        }
        result += ((buffer_ >> (CHAR_BIT - 1 - position_++)) & 0x01) << (n_bit - 1 - i);
        position_ = (position_ >= CHAR_BIT) ? 0 : position_;
    }

    return result;
}