#include <iostream>
#include <vector>
#include <functional>
#include <set>
#include <cassert>
#include <random>
using namespace::std;

template<typename key_type, typename compare = less<key_type>>
struct FibonacciHeap {

    struct Node {
        key_type key;
        int degree;
        bool mark;
        Node *parent, *child;
        Node *left, *right;

        Node(const key_type &key) : key(key), degree(0), mark(false), parent(nullptr), child(nullptr) {
            left = right = this;
        }
    };

    Node *min_node = nullptr;
    int n = 0;
    compare cmp;

    FibonacciHeap() {}
    FibonacciHeap(const FibonacciHeap&) = delete;
    FibonacciHeap& operator=(const FibonacciHeap&) = delete;

    ~FibonacciHeap() {
        destroy(min_node);
    }

    void destroy(Node *list) {
        if (list == nullptr) return;
        Node *x = list;
        do {
            Node *next = x -> right;
            destroy(x -> child);
            delete x;
            x = next;
        } while (x != list);
    }

    static void splice(Node *a, Node *b) {
        Node *a_right = a -> right;
        Node *b_left = b -> left;
        a -> right = b;
        b -> left = a;
        b_left -> right = a_right;
        a_right -> left = b_left;
    }

    static void remove_from_list(Node *x) {
        x -> left -> right = x -> right;
        x -> right -> left = x -> left;
        x -> left = x -> right = x;
    }

    void add_root(Node *x) {
        x -> parent = nullptr;
        if (min_node == nullptr) {
            min_node = x;
            return;
        }
        splice(min_node, x);
        if (cmp(x -> key, min_node -> key)) min_node = x;
    }

    Node* insert(const key_type &key) {
        Node *x = new Node(key);
        add_root(x);
        ++n;
        return x;
    }

    const key_type& minimum() const {
        assert(min_node != nullptr);
        return min_node -> key;
    }

    void merge(FibonacciHeap &other) {
        if (other.min_node == nullptr) return;
        if (min_node == nullptr) min_node = other.min_node;
        else {
            splice(min_node, other.min_node);
            if (cmp(other.min_node -> key, min_node -> key)) min_node = other.min_node;
        }
        n += other.n;
        other.min_node = nullptr;
        other.n = 0;
    }

    void link(Node *y, Node *x) {
        y -> parent = x;
        y -> mark = false;
        if (x -> child == nullptr) x -> child = y;
        else splice(x -> child, y);
        ++x -> degree;
    }

    void consolidate() {
        vector<Node*> A(64, nullptr);
        vector<Node*> roots;
        Node *w = min_node;
        do {
            roots.push_back(w);
            w = w -> right;
        } while (w != min_node);

        for (Node *x : roots) {
            x -> left = x -> right = x;
            int d = x -> degree;
            while (A[d] != nullptr) {
                Node *y = A[d];
                if (cmp(y -> key, x -> key)) swap(x, y);
                link(y, x);
                A[d] = nullptr;
                ++d;
            }
            A[d] = x;
        }

        min_node = nullptr;
        for (Node *x : A) {
            if (x != nullptr) add_root(x);
        }
    }

    key_type extract_min() {
        Node *z = min_node;
        assert(z != nullptr);
        if (z -> child != nullptr) {
            Node *c = z -> child;
            do {
                c -> parent = nullptr;
                c = c -> right;
            } while (c != z -> child);
            splice(z, z -> child);
            z -> child = nullptr;
        }
        if (z -> right == z) {
            min_node = nullptr;
        }
        else {
            min_node = z -> right;
            remove_from_list(z);
            consolidate();
        }
        --n;
        key_type key = z -> key;
        delete z;
        return key;
    }

    void cut(Node *x, Node *y) {
        if (y -> child == x) y -> child = x -> right == x ? nullptr : x -> right;
        remove_from_list(x);
        --y -> degree;
        x -> mark = false;
        add_root(x);
    }

    void cascading_cut(Node *y) {
        Node *z = y -> parent;
        if (z == nullptr) return;
        if (!y -> mark) {
            y -> mark = true;
        }
        else {
            cut(y, z);
            cascading_cut(z);
        }
    }

    void decrease_key(Node *x, const key_type &key) {
        assert(!cmp(x -> key, key));
        x -> key = key;
        Node *y = x -> parent;
        if (y != nullptr and cmp(x -> key, y -> key)) {
            cut(x, y);
            cascading_cut(y);
        }
        if (cmp(x -> key, min_node -> key)) min_node = x;
    }

    void erase(Node *x) {
        Node *y = x -> parent;
        if (y != nullptr) {
            cut(x, y);
            cascading_cut(y);
        }
        min_node = x;
        extract_min();
    }

    int size() const { return n; }
    bool empty() const { return n == 0; }
};

int main() {
    mt19937 rng(3014);

    using key = pair<int, int>;
    FibonacciHeap<key> H;
    vector<FibonacciHeap<key>::Node*> handle;
    vector<bool> alive;
    set<key> brute;

    for (int it = 0; it < 100000; ++it) {
        int op = rng() % 10;
        if (brute.empty() or op < 4) {
            key k = {(int)(rng() % 1000), (int)handle.size()};
            handle.push_back(H.insert(k));
            alive.push_back(true);
            brute.insert(k);
        }
        else if (op < 6) {
            key k = H.extract_min();
            assert(k == *brute.begin());
            brute.erase(brute.begin());
            alive[k.second] = false;
        }
        else {
            int id = rng() % handle.size();
            if (!alive[id]) continue;
            key k = handle[id] -> key;
            brute.erase(k);
            if (op < 9) {
                key new_k = {k.first - (int)(rng() % 1000), id};
                H.decrease_key(handle[id], new_k);
                brute.insert(new_k);
            }
            else {
                H.erase(handle[id]);
                alive[id] = false;
            }
        }
        assert(H.size() == (int)brute.size());
        if (!brute.empty()) assert(H.minimum() == *brute.begin());
    }

    FibonacciHeap<int> A, B;
    for (int i = 0; i < 100; ++i) A.insert(2 * i);
    for (int i = 0; i < 100; ++i) B.insert(2 * i + 1);
    A.merge(B);
    assert(B.empty() and A.size() == 200);
    for (int i = 0; i < 200; ++i) assert(A.extract_min() == i);

    cout << "FibonacciHeap OK" << '\n';
    return 0;
}
