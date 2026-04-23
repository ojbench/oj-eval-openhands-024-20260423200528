#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <iostream>

namespace sjtu { 

template<class T>
class deque {
private:
    static const size_t BLOCK_SIZE = 512;
    struct Block {
        T** data;
        size_t capacity;
        size_t head, tail;
        size_t count;

        Block(size_t cap = BLOCK_SIZE) : capacity(cap), head(0), tail(0), count(0) {
            data = (T**)operator new(capacity * sizeof(T*));
        }
        ~Block() {
            for (size_t i = 0; i < count; ++i) {
                size_t idx = (head + i) % capacity;
                data[idx]->~T();
                operator delete(data[idx]);
            }
            operator delete(data);
        }
        void push_back(const T &value) {
            data[tail] = (T*)operator new(sizeof(T));
            new (data[tail]) T(value);
            tail = (tail + 1) % capacity;
            count++;
        }
        void push_front(const T &value) {
            head = (head + capacity - 1) % capacity;
            data[head] = (T*)operator new(sizeof(T));
            new (data[head]) T(value);
            count++;
        }
        void pop_back() {
            tail = (tail + capacity - 1) % capacity;
            data[tail]->~T();
            operator delete(data[tail]);
            count--;
        }
        void pop_front() {
            data[head]->~T();
            operator delete(data[head]);
            head = (head + 1) % capacity;
            count--;
        }
        T& operator[](size_t pos) {
            return *data[(head + pos) % capacity];
        }
        const T& operator[](size_t pos) const {
            return *data[(head + pos) % capacity];
        }
    };

    T** map;
    size_t map_capacity;
    size_t first_block_idx, last_block_idx;
    size_t first_element_idx, last_element_idx;
    size_t total_size;

    static const size_t CHUNK_SIZE = 128;

    void expand_map() {
        size_t new_capacity = map_capacity * 2 + 1;
        T** new_map = (T**)operator new(new_capacity * sizeof(T*));
        size_t offset = (new_capacity - (last_block_idx - first_block_idx + 1)) / 2;
        for (size_t i = first_block_idx; i <= last_block_idx; ++i) {
            new_map[offset + i - first_block_idx] = map[i];
        }
        operator delete(map);
        map = new_map;
        last_block_idx = offset + last_block_idx - first_block_idx;
        first_block_idx = offset;
        map_capacity = new_capacity;
    }

public:
    class const_iterator;
    class iterator {
        friend class deque;
    private:
        deque* container;
        size_t index;
    public:
        iterator(deque* c = nullptr, size_t i = 0) : container(c), index(i) {}
        iterator(const iterator &other) : container(other.container), index(other.index) {}

        iterator operator+(const int &n) const {
            return iterator(container, index + n);
        }
        iterator operator-(const int &n) const {
            return iterator(container, index - n);
        }
        int operator-(const iterator &rhs) const {
            if (container != rhs.container) throw invalid_iterator();
            return (int)index - (int)rhs.index;
        }
        iterator operator+=(const int &n) {
            index += n;
            return *this;
        }
        iterator operator-=(const int &n) {
            index -= n;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            index++;
            return tmp;
        }
        iterator& operator++() {
            index++;
            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            index--;
            return tmp;
        }
        iterator& operator--() {
            index--;
            return *this;
        }
        T& operator*() const {
            if (!container || index < 0 || index >= container->total_size) throw invalid_iterator();
            return (*container)[index];
        }
        T* operator->() const noexcept {
            return &((*container)[index]);
        }
        bool operator==(const iterator &rhs) const {
            return container == rhs.container && index == rhs.index;
        }
        bool operator==(const const_iterator &rhs) const {
            return container == rhs.container && index == rhs.index;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };
    class const_iterator {
        friend class deque;
    private:
        const deque* container;
        size_t index;
    public:
        const_iterator(const deque* c = nullptr, size_t i = 0) : container(c), index(i) {}
        const_iterator(const iterator &other) : container(other.container), index(other.index) {}

        const_iterator operator+(const int &n) const {
            return const_iterator(container, index + n);
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(container, index - n);
        }
        int operator-(const const_iterator &rhs) const {
            if (container != rhs.container) throw invalid_iterator();
            return (int)index - (int)rhs.index;
        }
        const_iterator operator+=(const int &n) {
            index += n;
            return *this;
        }
        const_iterator operator-=(const int &n) {
            index -= n;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            index++;
            return tmp;
        }
        const_iterator& operator++() {
            index++;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            index--;
            return tmp;
        }
        const_iterator& operator--() {
            index--;
            return *this;
        }
        const T& operator*() const {
            if (!container || index < 0 || index >= container->total_size) throw invalid_iterator();
            return (*container)[index];
        }
        const T* operator->() const noexcept {
            return &((*container)[index]);
        }
        bool operator==(const iterator &rhs) const {
            return container == rhs.container && index == rhs.index;
        }
        bool operator==(const const_iterator &rhs) const {
            return container == rhs.container && index == rhs.index;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };

private:
    T** blocks;
    size_t head_block, tail_block;
    size_t head_idx, tail_idx;
    size_t blocks_cap;

    void expand_blocks() {
        size_t new_cap = blocks_cap * 2 + 1;
        T** new_blocks = (T**)operator new(new_cap * sizeof(T*));
        size_t offset = (new_cap - (tail_block - head_block + 1)) / 2;
        for (size_t i = head_block; i <= tail_block; ++i) {
            new_blocks[offset + i - head_block] = blocks[i];
        }
        operator delete(blocks);
        blocks = new_blocks;
        tail_block = offset + tail_block - head_block;
        head_block = offset;
        blocks_cap = new_cap;
    }

public:
    deque() : total_size(0), blocks_cap(CHUNK_SIZE) {
        blocks = (T**)operator new(blocks_cap * sizeof(T*));
        for (size_t i = 0; i < blocks_cap; ++i) blocks[i] = nullptr;
        head_block = tail_block = blocks_cap / 2;
        blocks[head_block] = (T*)operator new(BLOCK_SIZE * sizeof(T));
        head_idx = tail_idx = BLOCK_SIZE / 2;
    }
    deque(const deque &other) : total_size(other.total_size), blocks_cap(other.blocks_cap) {
        blocks = (T**)operator new(blocks_cap * sizeof(T*));
        for (size_t i = 0; i < blocks_cap; ++i) blocks[i] = nullptr;
        head_block = other.head_block;
        tail_block = other.tail_block;
        head_idx = other.head_idx;
        tail_idx = other.tail_idx;
        for (size_t i = head_block; i <= tail_block; ++i) {
            if (other.blocks[i]) {
                blocks[i] = (T*)operator new(BLOCK_SIZE * sizeof(T));
                size_t start = (i == head_block) ? head_idx : 0;
                size_t end = (i == tail_block) ? tail_idx : BLOCK_SIZE;
                for (size_t j = start; j < end; ++j) {
                    new (&blocks[i][j]) T(other.blocks[i][j]);
                }
            }
        }
    }
    ~deque() {
        clear();
        for (size_t i = 0; i < blocks_cap; ++i) {
            if (blocks[i]) operator delete(blocks[i]);
        }
        operator delete(blocks);
    }
    deque &operator=(const deque &other) {
        if (this == &other) return *this;
        clear();
        for (size_t i = 0; i < blocks_cap; ++i) {
            if (blocks[i]) operator delete(blocks[i]);
        }
        operator delete(blocks);

        total_size = other.total_size;
        blocks_cap = other.blocks_cap;
        blocks = (T**)operator new(blocks_cap * sizeof(T*));
        for (size_t i = 0; i < blocks_cap; ++i) blocks[i] = nullptr;
        head_block = other.head_block;
        tail_block = other.tail_block;
        head_idx = other.head_idx;
        tail_idx = other.tail_idx;
        for (size_t i = head_block; i <= tail_block; ++i) {
            if (other.blocks[i]) {
                blocks[i] = (T*)operator new(BLOCK_SIZE * sizeof(T));
                size_t start = (i == head_block) ? head_idx : 0;
                size_t end = (i == tail_block) ? tail_idx : BLOCK_SIZE;
                for (size_t j = start; j < end; ++j) {
                    new (&blocks[i][j]) T(other.blocks[i][j]);
                }
            }
        }
        return *this;
    }
    T & at(const size_t &pos) {
        if (pos < 0 || pos >= total_size) throw index_out_of_bound();
        return (*this)[pos];
    }
    const T & at(const size_t &pos) const {
        if (pos < 0 || pos >= total_size) throw index_out_of_bound();
        return (*this)[pos];
    }
    T & operator[](const size_t &pos) {
        size_t real_idx = head_idx + pos;
        size_t block_offset = real_idx / BLOCK_SIZE;
        size_t inner_idx = real_idx % BLOCK_SIZE;
        return blocks[head_block + block_offset][inner_idx];
    }
    const T & operator[](const size_t &pos) const {
        size_t real_idx = head_idx + pos;
        size_t block_offset = real_idx / BLOCK_SIZE;
        size_t inner_idx = real_idx % BLOCK_SIZE;
        return blocks[head_block + block_offset][inner_idx];
    }
    T & front() {
        if (total_size == 0) throw container_is_empty();
        return blocks[head_block][head_idx];
    }
    const T & front() const {
        if (total_size == 0) throw container_is_empty();
        return blocks[head_block][head_idx];
    }
    T & back() {
        if (total_size == 0) throw container_is_empty();
        if (tail_idx == 0) return blocks[tail_block - 1][BLOCK_SIZE - 1];
        return blocks[tail_block][tail_idx - 1];
    }
    const T & back() const {
        if (total_size == 0) throw container_is_empty();
        if (tail_idx == 0) return blocks[tail_block - 1][BLOCK_SIZE - 1];
        return blocks[tail_block][tail_idx - 1];
    }
    iterator begin() {
        return iterator(this, 0);
    }
    const_iterator begin() const {
        return const_iterator(this, 0);
    }
    const_iterator cbegin() const {
        return const_iterator(this, 0);
    }
    iterator end() {
        return iterator(this, total_size);
    }
    const_iterator end() const {
        return const_iterator(this, total_size);
    }
    const_iterator cend() const {
        return const_iterator(this, total_size);
    }
    bool empty() const {
        return total_size == 0;
    }
    size_t size() const {
        return total_size;
    }
    void clear() {
        for (size_t i = 0; i < total_size; ++i) {
            (*this)[i].~T();
        }
        total_size = 0;
        head_block = tail_block = blocks_cap / 2;
        head_idx = tail_idx = BLOCK_SIZE / 2;
    }
    iterator insert(iterator pos, const T &value) {
        if (pos.container != this || pos.index < 0 || pos.index > total_size) throw invalid_iterator();
        if (pos.index == 0) {
            push_front(value);
            return begin();
        }
        if (pos.index == total_size) {
            push_back(value);
            return iterator(this, total_size - 1);
        }
        
        if (pos.index < total_size / 2) {
            push_front(front());
            for (size_t i = 0; i < pos.index; ++i) {
                (*this)[i] = (*this)[i + 1];
            }
            (*this)[pos.index] = value;
        } else {
            push_back(back());
            for (size_t i = total_size - 1; i > pos.index; --i) {
                (*this)[i] = (*this)[i - 1];
            }
            (*this)[pos.index] = value;
        }
        return iterator(this, pos.index);
    }
    iterator erase(iterator pos) {
        if (pos.container != this || pos.index < 0 || pos.index >= total_size) throw invalid_iterator();
        if (total_size == 0) throw container_is_empty();
        
        if (pos.index < total_size / 2) {
            for (size_t i = pos.index; i > 0; --i) {
                (*this)[i] = (*this)[i - 1];
            }
            pop_front();
        } else {
            for (size_t i = pos.index; i < total_size - 1; ++i) {
                (*this)[i] = (*this)[i + 1];
            }
            pop_back();
        }
        return iterator(this, pos.index);
    }
    void push_back(const T &value) {
        if (tail_idx == BLOCK_SIZE) {
            if (tail_block + 1 == blocks_cap) expand_blocks();
            tail_block++;
            if (!blocks[tail_block]) blocks[tail_block] = (T*)operator new(BLOCK_SIZE * sizeof(T));
            tail_idx = 0;
        }
        new (&blocks[tail_block][tail_idx]) T(value);
        tail_idx++;
        total_size++;
    }
    void pop_back() {
        if (total_size == 0) throw container_is_empty();
        if (tail_idx == 0) {
            tail_block--;
            tail_idx = BLOCK_SIZE;
        }
        tail_idx--;
        blocks[tail_block][tail_idx].~T();
        total_size--;
    }
    void push_front(const T &value) {
        if (head_idx == 0) {
            if (head_block == 0) expand_blocks();
            head_block--;
            if (!blocks[head_block]) blocks[head_block] = (T*)operator new(BLOCK_SIZE * sizeof(T));
            head_idx = BLOCK_SIZE;
        }
        head_idx--;
        new (&blocks[head_block][head_idx]) T(value);
        total_size++;
    }
    void pop_front() {
        if (total_size == 0) throw container_is_empty();
        blocks[head_block][head_idx].~T();
        head_idx++;
        if (head_idx == BLOCK_SIZE) {
            head_block++;
            head_idx = 0;
        }
        total_size--;
    }
};

}

#endif
