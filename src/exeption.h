#pragma once

#include <string>

class Exeption {
public:
    virtual std::string GetError() const = 0;
    virtual std::string GetFeedback() const = 0;
};