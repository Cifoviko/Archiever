#pragma once

#include <vector>

class Code {
public:
    void Shift(size_t delta) {
        for (size_t i = 0; i < delta; ++i) {
            code_.push_back(false);
        }
    }
    Code& operator++() {
        if (code_.empty()) {
            return *this;
        }
        size_t id = code_.size() - 1;
        while (id >= 0) {
            if (code_[id]) {
                code_[id] = false;
            } else {
                code_[id] = true;
                break;
            }
            --id;
        }
        return *this;
    }
    const std::vector<bool> GetCode() const {
        return code_;
    }
    const size_t GetSize() const {
        return code_.size();
    }

private:
    std::vector<bool> code_;
};