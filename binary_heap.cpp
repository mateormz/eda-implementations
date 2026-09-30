#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
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
        return heap[0];
    }

    void pop() {
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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    BinaryHeap<long long> H;
    string operation;
    while (cin >> operation) {
        if (operation == "insert") {
            long long x;
            cin >> x;
            H.push(x);
        }
        else if (operation == "extract") {
            cout << H.top() << '\n';
            H.pop();
        }
        else if (operation == "end") {
            break;
        }
    }

    return 0;
}
