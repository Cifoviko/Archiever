#pragma once

#include "command_line_error.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace parser_setup {
enum Command { ARCHIVE, UNARRCHIVE, HELP, BIT_VIEW, Unknown };  // Command list

Command GetCommand(std::string command);  // Get COMMAND by it's string
void ValidateCommand(Command command,
                     const std::vector<std::string> parameters);  // Check if there is rigth amount of parameters
}  // namespace parser_setup