#include "parser_setup.h"
#include <iostream>

namespace parser_setup {
const std::unordered_map<std::string, Command> COMMANDS(
    {{"-c", ARCHIVE}, {"-d", UNARRCHIVE}, {"-h", HELP}, {"-b", BIT_VIEW}});

Command GetCommand(std::string command) {  // Get COMMAND by it's string
    if (COMMANDS.count(command) == 0) {
        return Unknown;
    }
    return (COMMANDS.find(command))->second;
}

void ValidateCommand(Command command,
                     const std::vector<std::string> parameters) {  // Check if there is rigth amount of parameters
    switch (command) {
        case parser_setup::ARCHIVE:
            if (parameters.size() < 2) {
                throw(CommandLineError(CommandLineError::ErrorType::NOT_ENOUGH_PARAMETERS));
            }
            break;
        case parser_setup::UNARRCHIVE:
            if (parameters.size() > 1) {
                throw(CommandLineError(CommandLineError::ErrorType::TO_MANY_PARAMETERS));
            } else if (parameters.empty()) {
                throw(CommandLineError(CommandLineError::ErrorType::NOT_ENOUGH_PARAMETERS));
            }
            break;
        case parser_setup::HELP:
            if (!parameters.empty()) {
                throw(CommandLineError(CommandLineError::ErrorType::TO_MANY_PARAMETERS));
            }
            break;
        case parser_setup::BIT_VIEW:
            if (parameters.size() < 2) {
                throw(CommandLineError(CommandLineError::ErrorType::NOT_ENOUGH_PARAMETERS));
            } else if (parameters.size() > 2) {
                throw(CommandLineError(CommandLineError::ErrorType::TO_MANY_PARAMETERS));
            }
            break;
        case parser_setup::Unknown:
            throw(CommandLineError(CommandLineError::ErrorType::NO_COMMAND));
            break;
        default:
            break;
    }
}
}  // namespace parser_setup