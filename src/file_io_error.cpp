#include "file_io_error.h"

FileIOError::FileIOError(ErrorType error_type, std::string bad_file) : error_type_(error_type), bad_file_(bad_file) {
}

std::string FileIOError::GetFeedback() const {
    return (GetError() + " " + GetBadFile());
}

std::string FileIOError::GetError() const {
    switch (error_type_) {
        case FileIOError::CANT_OPEN_FILE:
            return "Can't open the file";
        case FileIOError::INVALID_FILE:
            return "File is broken";
        case FileIOError::NOT_OPEN_FILE:
            return "No file is open, when trying to interact";
        case FileIOError::READ_IN_WRITE_ONLY:
            return "Trying to read in file opened to write";
        case FileIOError::WRITE_IN_READ_ONLY:
            return "Trying to write in file opened to read";
        case FileIOError::TRYING_TO_OPEN_OPEN_FILE:
            return "Trying to open a new file, while old is still open";
        default:
            return "";
    }
}

std::string FileIOError::GetBadFile() const {
    return bad_file_;
}
