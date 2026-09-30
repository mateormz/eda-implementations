#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <cassert>
#include <random>
using namespace::std;

template<typename data_type, typename compare = less<data_type>>
struct BinaryHeap {
    vector<data_type> heap;
    compare cmp;

    BinaryHeap() {}

    BinaryHeap(const vector<data_type> &a) : heap(a) {
        for (int i = (int)heap.size() / 2 - 1; i >= 0; --i) sift_down(i);
    }

    static int parent(int i) { return (i - 1) / 2; }
    static int left(int i) { return 2 * i + 1; }
    static int right(int i) { return 2 * i + 2; }

    bool higher(const data_type &a, const data_type &b) const {
        return cmp(b, a);
    }

    void sift_up(int i) {
        while (i > 0 and higher(heap[i], heap[parent(i)])) {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    void sift_down(int i) {
        int n = heap.size();
        while (true) {
            int best = i;
            if (left(i) < n and higher(heap[left(i)], heap[best])) best = left(i);
            if (right(i) < n and higher(heap[right(i)], heap[best])) best = right(i);
            if (best == i) return;
            swap(heap[i], heap[best]);
            i = best;
        }
    }

    void push(const data_type &x) {
        heap.push_back(x);
        sift_up((int)heap.size() - 1);
    }

    const data_type& top() const {
        assert(!heap.empty());
        return heap[0];
    }

    void pop() {
        assert(!heap.empty());
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) sift_down(0);
    }

    void update(int i, const data_type &value) {
        bool goes_up = higher(value, heap[i]);
        heap[i] = value;
        if (goes_up) sift_up(i);
        else sift_down(i);
    }

    int size() const { return heap.size(); }
    bool empty() const { return heap.empty(); }
};

template<typename data_type>
void heap_sort(vector<data_type> &a) {
    BinaryHeap<data_type> H(a);
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        a[i] = H.top();
        H.pop();
    }
}

int main() {
    mt19937 rng(3014);

    BinaryHeap<int> max_heap;
    vector<int> brute;
    for (int it = 0; it < 20000; ++it) {
        if (brute.empty() or rng() % 3) {
            int x = rng() % 1000;
            max_heap.push(x);
            brute.push_back(x);
        }
        else {
            auto it_max = max_element(brute.begin(), brute.end());
            assert(max_heap.top() == *it_max);
            brute.erase(it_max);
            max_heap.pop();
        }
    }

    vector<int> a(1000);
    for (int &x : a) x = rng() % 100;
    BinaryHeap<int, greater<int>> min_heap(a);
    vector<int> sorted_a = a;
    sort(sorted_a.begin(), sorted_a.end());
    for (int x : sorted_a) {
        assert(min_heap.top() == x);
        min_heap.pop();
    }

    vector<int> b = a;
    heap_sort(b);
    assert(b == sorted_a);

    cout << "BinaryHeap OK" << '\n';
    return 0;
}
