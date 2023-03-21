#pragma once

#include "exeption.h"

class FileIOError : public Exeption {
public:
    enum ErrorType {
        CANT_OPEN_FILE,
        INVALID_FILE,
        NOT_OPEN_FILE,
        READ_IN_WRITE_ONLY,
        WRITE_IN_READ_ONLY,
        TRYING_TO_OPEN_OPEN_FILE
    };

    explicit FileIOError(ErrorType error_type, std::string bad_file = "");

    std::string GetFeedback() const override;
    std::string GetError() const override;

    std::string GetBadFile() const;

private:
    ErrorType error_type_;
    std::string bad_file_;
};