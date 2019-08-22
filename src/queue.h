#pragma once

#include <iostream>
#include <vector>

#include "triangle.h"

class Queue {
public:
    void Push(const std::shared_ptr<Triangle> &t) {
        const int i = m_Queue.size();
        t->SetQueueIndex(i);
        m_Queue.push_back(t);
        Up(i);
    }

    std::shared_ptr<Triangle> Pop() {
        const int n = m_Queue.size() - 1;
        Swap(0, n);
        Down(0, n);
        return PopBack();
    }

    std::shared_ptr<Triangle> Remove(const int i) {
        const int n = m_Queue.size() - 1;
        if (n != i) {
            Swap(i, n);
            if (!Down(i, n)) {
                Up(i);
            }
        }
        return PopBack();
    }

    std::shared_ptr<Triangle> Head() const {
        return m_Queue.front();
    }

    bool IsHeap() const {
        const int n = m_Queue.size();
        for (int i = 0; i < n; i++) {
            const int j1 = 2 * i + 1;
            const int j2 = 2 * i + 2;
            if (j1 < n && !LessEqual(i, j1)) {
                return false;
            }
            if (j2 < n && !LessEqual(i, j2)) {
                return false;
            }
        }
        return true;
    }

private:
    bool Less(const int i, const int j) const {
        return -m_Queue[i]->Error() < -m_Queue[j]->Error();
    }

    bool LessEqual(const int i, const int j) const {
        return -m_Queue[i]->Error() <= -m_Queue[j]->Error();
    }

    std::shared_ptr<Triangle> PopBack() {
        auto t = m_Queue.back();
        m_Queue.pop_back();
        t->SetQueueIndex(-1);
        return t;
    }

    void Swap(const int i, const int j) {
        const auto pi = m_Queue[i];
        const auto pj = m_Queue[j];
        m_Queue[i] = pj;
        m_Queue[j] = pi;
        pj->SetQueueIndex(i);
        pi->SetQueueIndex(j);
    }

    void Up(int j) {
        while (1) {
            int i = (j - 1) / 2;
            if (i == j || !Less(j, i)) {
                break;
            }
            Swap(i, j);
            j = i;
        }
    }

    bool Down(const int i0, const int n) {
        int i = i0;
        while (1) {
            const int j1 = 2 * i + 1;
            if (j1 >= n || j1 < 0) {
                break;
            }
            const int j2 = j1 + 1;
            int j = j1;
            if (j2 < n && Less(j2, j1)) {
                j = j2;
            }
            if (!Less(j, i)) {
                break;
            }
            Swap(i, j);
            i = j;
        }
        return i > i0;
    }

    std::vector<std::shared_ptr<Triangle>> m_Queue;
};
