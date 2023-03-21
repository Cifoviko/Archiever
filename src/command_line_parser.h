#pragma once

#include "command_line_error.h"
#include "parser_setup.h"

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

class CommandLineParser {
public:
    CommandLineParser(int cnt, char** argumets);

    void PrintInfo();

    std::string GetParameter(size_t id) const;
    const std::vector<std::string>& GetParameters() const;
    std::string GetProgramName() const;
    std::string GetCommandString() const;
    parser_setup::Command GetCommand() const;
    size_t GetParametersCount() const;

private:
    std::string program_name_;
    std::string command_string_;
    parser_setup::Command command_;
    std::vector<std::string> parameters_;
};