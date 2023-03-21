#pragma once

#include "priority_queue.h"
#include "node.h"
#include "code.h"

#include <algorithm>
#include <queue>
#include <vector>
#include <map>

template <typename Symbol>
class SizeTNodePtrGreater {  // Comparator for std::pair<size_t, Node<Symbol>*>
public:
    bool operator()(const std::pair<size_t, Node<Symbol>*>& a, const std::pair<size_t, Node<Symbol>*>& b) const {
        if (a.first == b.first) {
            return a.second->value_ > b.second->value_;
        } else {
            return a.first > b.first;
        }
    }
};

template <typename Symbol>
class Coder {
public:
    using EncodedList = std::map<Symbol, size_t>;

    ~Coder() {
        delete huffman_tree_;
    }

    void AddSymbol(Symbol key) {  // Add symbol(key) frequency
        ++symbol_count_[key];
    }
    void Encode() {  // Generate coded lengths codes using frequency table
        CreateEncodedTree();
        CreateEncodedList();
    }
    const EncodedList& GetEncodedList() const {
        return encoded_list_;
    }
    void Reset() {
        encoded_list_.clear();
        symbol_count_.clear();
        delete huffman_tree_;
    }
    void ChangeEncodedList(const EncodedList& encoded_list) {
        Reset();
        encoded_list_ = encoded_list;
    }
    void ChangeEncodedList(EncodedList&& encoded_list) {
        Reset();
        std::swap(encoded_list_, encoded_list);
    }
    void CreateCanonHuffmanTree(const std::vector<std::pair<size_t, Symbol>>&
                                    encoded_table) {  // Generate canonical codes using !sorted lengths(encoded_table)
        delete huffman_tree_;
        huffman_tree_ = new Node<Symbol>;
        Code code;
        for (const auto& [length, symbol] : encoded_table) {
            ++code;
            code.Shift(length - code.GetSize());
            symbol_code_[symbol] = code.GetCode();
            AddCode(code.GetCode(), symbol);
        }
        now_node_ = huffman_tree_;
    }
    void CreateCanonHuffmanTree() {  // Generate canonical codes using internal lengths(encoded_list_ -> encoded_table)
        delete huffman_tree_;
        huffman_tree_ = new Node<Symbol>;

        // [begin] Generate sorted lengths(encoded_table)
        std::vector<std::pair<size_t, Symbol>> encoded_table;
        for (const auto& [symbol, length] : encoded_list_) {
            encoded_table.push_back(std::make_pair(length, symbol));
        }
        sort(encoded_table.begin(), encoded_table.end());
        // [end] Generate sorted lengths(encoded_table)

        // [begin] Generate canonical codes
        Code code;
        for (const auto& [length, symbol] : encoded_table) {
            ++code;
            code.Shift(length - code.GetSize());
            symbol_code_[symbol] = code.GetCode();
            AddCode(code.GetCode(), symbol);
        }
        // [end] Generate canonical codes

        now_node_ = huffman_tree_;
    }

    bool GoToNode(size_t x) {  // Descending in internal bor (return true if terminal)
        if (x == 0) {
            now_node_ = now_node_->left_child_;
        } else {
            now_node_ = now_node_->right_child_;
        }
        return (now_node_->left_child_ == nullptr && now_node_->right_child_ == nullptr);
    }
    Symbol GetNodeValue() const {  // Get value of current node in internal bor
        return now_node_->value_;
    }
    void ResetNode() {  // Reset current node in internal bor to root
        now_node_ = huffman_tree_;
    }

    const std::vector<bool>& GetCode(Symbol symbol) {  // Get canonical code of encoded symbol
        return symbol_code_[symbol];
    }

private:
    void CreateEncodedTree() {  // Generating NOT canonical codes
        // [begin] Initializing tree leaves
        std::priority_queue<std::pair<size_t, Node<Symbol>*>, std::vector<std::pair<size_t, Node<Symbol>*>>,
                            SizeTNodePtrGreater<Symbol>>
            nodes;
        for (const auto& [symbol, cnt] : symbol_count_) {
            Node<Symbol>* new_node = new Node<Symbol>(symbol);
            nodes.push(std::make_pair(cnt, new_node));
        }
        // [end] Initializing tree leaves

        // [begin] Building tree
        while (nodes.size() > 1) {
            auto [cnt1, node1] = nodes.top();
            nodes.pop();
            auto [cnt2, node2] = nodes.top();
            nodes.pop();

            Node<Symbol>* new_node = new Node<Symbol>(std::min(node1->value_, node2->value_), node1, node2);
            nodes.push(std::make_pair(cnt1 + cnt2, new_node));
        }
        // [end] Building tree

        huffman_tree_ = nodes.top().second;  // Initializing root
    }
    void VisitNode(Node<Symbol>* node, size_t h = 0) {  // Calculate lengths of NOT canonical codes, decending in tree
        if (node->left_child_ == nullptr && node->right_child_ == nullptr) {
            encoded_list_[node->value_] = h;
            return;
        }
        if (node->left_child_ != nullptr) {
            VisitNode(node->left_child_, h + 1);
        }
        if (node->right_child_ != nullptr) {
            VisitNode(node->right_child_, h + 1);
        }
    }
    void CreateEncodedList() {  // Calculate lengths of NOT canonical codes
        VisitNode(huffman_tree_);
    }
    void AddCode(const std::vector<bool>& code, Symbol symbol) {  // Add code of symbol to internal bor
        Node<Symbol>* node = huffman_tree_;
        for (const auto& element : code) {
            if (element) {
                if (node->right_child_ == nullptr) {
                    node->right_child_ = new Node<Symbol>;
                }
                node = node->right_child_;
            } else {
                if (node->left_child_ == nullptr) {
                    node->left_child_ = new Node<Symbol>;
                }
                node = node->left_child_;
            }
        }
        node->value_ = symbol;
    }

    EncodedList encoded_list_;
    std::map<Symbol, size_t> symbol_count_;
    Node<Symbol>* huffman_tree_ = nullptr;
    std::map<Symbol, std::vector<bool>> symbol_code_;
    Node<Symbol>* now_node_ = nullptr;
};