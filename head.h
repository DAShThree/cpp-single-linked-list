#pragma once
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include <algorithm>

template <typename T>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const T& val, Node* next_node) : value(val), next_node(next_node){}
        T value{};
        Node* next_node = nullptr;
    };

public:
    template <typename U>
    class BasicIterator {
        template <typename V> friend class BasicIterator;
        friend class SingleLinkedList<T>;

        Node* node_ = nullptr;
        explicit BasicIterator(Node* node) noexcept : node_(node) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = U*;
        using reference = U&;

        BasicIterator() = default;
        BasicIterator(const BasicIterator<T>& other) noexcept : node_(other.node_) {}
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<T>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator!=(const BasicIterator<T>& rhs) const noexcept {
            return !(*this == rhs);
        }
        [[nodiscard]] bool operator==(const BasicIterator<const T>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator!=(const BasicIterator<const T>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }
        BasicIterator operator++(int) noexcept {
            BasicIterator old_node(*this);
            ++(*this);
            return old_node;
        }
        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }
        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &node_->value;
        }
    };

public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<T>;
    using ConstIterator = BasicIterator<const T>;

    SingleLinkedList() = default;
    
    ~SingleLinkedList() {
        Clear();
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const T& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* node_to_delete = head_.next_node;
            head_.next_node = node_to_delete->next_node;
            delete node_to_delete;
        }
        size_ = 0;
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return const_cast<SingleLinkedList*>(this)->before_begin();
    }
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return before_begin();
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }
    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }
    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    SingleLinkedList& operator=(const SingleLinkedList& other) {
        if (this != &other) {
            SingleLinkedList temp(other);
            swap(temp);
        }
        return *this;
    }

    void PushBack(const T& value) {
        Node* new_node = new Node(value, nullptr);
        Node* current = &head_;
        while (current->next_node != nullptr) {
            current = current->next_node;
        }
        current->next_node = new_node;
        ++size_;
    }

    SingleLinkedList(std::initializer_list<T> values) {
        try {
            for (auto it = values.end(); it != values.begin(); ) {
                --it;
                PushFront(*it);
            }
        } catch (...) {
            Clear();
            throw;
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        try {
            SingleLinkedList temp;
            Node* current = &temp.head_;
            for (const T& value : other) {
                current->next_node = new Node(value, nullptr);
                current = current->next_node;
                ++temp.size_;
            }
            swap(temp);
        } catch (...) {
            Clear();
            throw;
        }
    }
    bool operator==(const SingleLinkedList& other) const {
        return std::equal(begin(), end(), other.begin(), other.end());
    }
    bool operator!=(const SingleLinkedList& other) const {
        return !(*this == other);
    }
    bool operator<(const SingleLinkedList& other) const {
    return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }
    bool operator>(const SingleLinkedList& other) const { return other < *this; }
    bool operator<=(const SingleLinkedList& other) const { return !(other < *this); }
    bool operator>=(const SingleLinkedList& other) const { return !(*this < other); }

    void PopFront() noexcept {
        assert(head_.next_node != nullptr);
        Node* node_to_delete = head_.next_node;
        head_.next_node = node_to_delete->next_node;
        delete node_to_delete;
        --size_;
    }

    Iterator InsertAfter(ConstIterator pos, const T& value) {
        assert(pos.node_ != nullptr);
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator{new_node};
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr);
        assert(pos.node_->next_node != nullptr);
        Node* node_to_delete = pos.node_->next_node;
        pos.node_->next_node = node_to_delete->next_node;
        delete node_to_delete;
        --size_;
        return Iterator{pos.node_->next_node};
    }

private:
    Node head_{};
    size_t size_ = 0;
};

template <typename T>
void swap(SingleLinkedList<T>& lhs, SingleLinkedList<T>& rhs) noexcept {
    lhs.swap(rhs);
}