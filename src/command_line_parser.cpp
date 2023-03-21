#include "command_line_error.h"
#include "command_line_parser.h"

CommandLineParser::CommandLineParser(int cnt, char** argumets) {
    program_name_ = argumets[0];
    if (cnt == 1) {
        throw(CommandLineError(CommandLineError::ErrorType::NO_COMMAND));
    }
    command_string_ = argumets[1];
    command_ = parser_setup::GetCommand(command_string_);
    if (command_ == parser_setup::Unknown) {
        throw(CommandLineError(CommandLineError::ErrorType::INVALID_COMMAND, command_string_));
    }
    int id = 2;
    while (id < cnt) {
        parameters_.push_back(argumets[id++]);
    }
    parser_setup::ValidateCommand(command_, parameters_);
}

void CommandLineParser::PrintInfo() {
    std::cout << "name: " << program_name_ << std::endl;
    std::cout << "command: " << command_ << std::endl;
    std::cout << "parameters: ";
    for (const auto& parameter : parameters_) {
        std::cout << parameter << ' ';
    }
    std::cout << std::endl;
}

std::string CommandLineParser::GetParameter(size_t id) const {
    return parameters_[id];
}

const std::vector<std::string>& CommandLineParser::GetParameters() const {
    return parameters_;
}

std::string CommandLineParser::GetProgramName() const {
    return program_name_;
}

parser_setup::Command CommandLineParser::GetCommand() const {
    return command_;
}

size_t CommandLineParser::GetParametersCount() const {
    return parameters_.size();
}

std::string CommandLineParser::GetCommandString() const {
    return command_string_;
}