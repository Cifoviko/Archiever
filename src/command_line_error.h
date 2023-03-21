#pragma once

#include <string>

#include "exeption.h"

class CommandLineError : public Exeption {
public:
    enum ErrorType { INVALID_COMMAND, NO_COMMAND, NOT_ENOUGH_PARAMETERS, TO_MANY_PARAMETERS, INVALID_PARAMETER };

    explicit CommandLineError(ErrorType error_type, std::string bad_input = "");

    std::string GetFeedback() const override;
    std::string GetError() const override;

    std::string GetBadInput() const;

private:
    ErrorType error_type_;
    std::string bad_input_;
};