

#include "deque.hpp"
#include <iostream>
#include <cassert>

void test_basic() {
    sjtu::deque<int> d;
    assert(d.empty());
    assert(d.size() == 0);

    d.push_back(1);
    d.push_back(2);
    d.push_front(3);
    d.push_front(4);

    assert(d.size() == 4);
    assert(d[0] == 4);
    assert(d[1] == 3);
    assert(d[2] == 1);
    assert(d[3] == 2);

    assert(d.front() == 4);
    assert(d.back() == 2);

    d.pop_back();
    assert(d.size() == 3);
    assert(d.back() == 1);

    d.pop_front();
    assert(d.size() == 2);
    assert(d.front() == 3);

    d.clear();
    assert(d.empty());
    assert(d.size() == 0);
}

void test_iterator() {
    sjtu::deque<int> d;
    for (int i = 0; i < 10; ++i) d.push_back(i);

    int count = 0;
    for (sjtu::deque<int>::iterator it = d.begin(); it != d.end(); ++it) {
        assert(*it == count);
        count++;
    }
    assert(count == 10);

    sjtu::deque<int>::iterator it = d.begin() + 5;
    assert(*it == 5);
    assert(*(it - 2) == 3);
    assert(*(it + 2) == 7);
    assert(it - d.begin() == 5);
}

void test_insert_erase() {
    sjtu::deque<int> d;
    for (int i = 0; i < 5; ++i) d.push_back(i); // 0 1 2 3 4

    d.insert(d.begin() + 2, 10); // 0 1 10 2 3 4
    assert(d.size() == 6);
    assert(d[2] == 10);
    assert(d[3] == 2);

    d.erase(d.begin() + 2); // 0 1 2 3 4
    assert(d.size() == 5);
    assert(d[2] == 2);
}

void test_copy_assignment() {
    sjtu::deque<int> d1;
    for (int i = 0; i < 10; ++i) d1.push_back(i);

    sjtu::deque<int> d2(d1);
    assert(d2.size() == 10);
    for (int i = 0; i < 10; ++i) assert(d2[i] == i);

    sjtu::deque<int> d3;
    d3 = d1;
    assert(d3.size() == 10);
    for (int i = 0; i < 10; ++i) assert(d3[i] == i);

    d1.clear();
    assert(d2.size() == 10);
    assert(d3.size() == 10);
}

int main() {
    try {
        test_basic();
        test_iterator();
        test_insert_erase();
        test_copy_assignment();
        std::cout << "All local tests passed!" << std::endl;
    } catch (sjtu::exception &e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Caught unknown exception" << std::endl;
        return 1;
    }
    return 0;
}

