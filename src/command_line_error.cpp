#include "command_line_error.h"

CommandLineError::CommandLineError(ErrorType error_type, std::string bad_input)
    : error_type_(error_type), bad_input_(bad_input) {
}

std::string CommandLineError::GetFeedback() const {
    return (GetError() + " " + GetBadInput());
}

std::string CommandLineError::GetError() const {
    switch (error_type_) {
        case CommandLineError::INVALID_COMMAND:
            return "Unknown command";
        case CommandLineError::NO_COMMAND:
            return "No command";
        case CommandLineError::NOT_ENOUGH_PARAMETERS:
            return "Not enough paramerters";
        case CommandLineError::TO_MANY_PARAMETERS:
            return "To many paramerters";
        case CommandLineError::INVALID_PARAMETER:
            return "Invalid paramerters";
        default:
            return "";
    }
}

std::string CommandLineError::GetBadInput() const {
    return bad_input_;
}
