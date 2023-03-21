#pragma once

#include <compare>

template <typename Symbol>
class Node {
public:
    explicit Node(Symbol value = 0) : value_(value) {
    }
    explicit Node(Symbol value, Node<Symbol>* left_child, Node<Symbol>* right_child)
        : value_(value), left_child_(left_child), right_child_(right_child) {
    }

    ~Node() {
        delete left_child_;
        delete right_child_;
    };

    std::strong_ordering operator<=>(const Node<Symbol>* node) const {
        if (value_ < node->value_) {
            return std::strong_ordering::less;
        } else if (value_ > node->value_) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }

    Node<Symbol>* operator[](size_t id) {
        if (id == 0) {
            return left_child_;
        } else {
            return right_child_;
        }
        return nullptr;
    }

    Symbol value_;
    Node<Symbol>* left_child_ = nullptr;
    Node<Symbol>* right_child_ = nullptr;
};