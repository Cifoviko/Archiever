#pragma once

#include <vector>
#include <algorithm>
#include <functional>

template <typename Element, typename Compare = std::less<Element>>
class PriorityQueue {
public:
    PriorityQueue() = default;

    Element GetTop() const {
        return data_[0];
    }
    bool IsEmpty() const {
        return data_.empty();
    }
    size_t GetSize() const {
        return data_.size();
    }
    void Push(const Element& element) {
        data_.push_back(element);
        PushHeap();
    }
    void Pop() {
        PopHeap();
        data_.pop_back();
    }
    void PrintData() const {
        for (const auto& element : data_) {
            std::cout << element << ' ';
        }
        std::cout << std::endl;
    }

private:
    void PushHeap() {
        size_t id = data_.size() - 1;
        while (id > 0 && Compare()(data_[(id - 1) / 2], data_[id])) {
            std::swap(data_[id], data_[(id - 1) / 2]);
            if (id % 2 == 0) {
                std::swap(data_[id - 1], data_[id]);
            }
            id = (id - 1) / 2;
        }
        if (id != 0 && id % 2 == 0 && Compare()(data_[id - 1], data_[id])) {
            std::swap(data_[id - 1], data_[id]);
        }
    }

    void PopHeap() {
        std::swap(data_[0], data_[data_.size() - 1]);
        size_t id = 0;
        while (2 * id + 1 < data_.size() - 1) {
            size_t left = 2 * id + 1;
            size_t right = 2 * id + 2;

            if (Compare()(data_[id], data_[left])) {
                std::swap(data_[id], data_[left]);
                if (right < data_.size() - 1 && Compare()(data_[left], data_[right])) {
                    std::swap(data_[left], data_[right]);
                    id = right;
                } else {
                    id = left;
                }
            } else {
                break;
            }
        }
    }

    std::vector<Element> data_;
};