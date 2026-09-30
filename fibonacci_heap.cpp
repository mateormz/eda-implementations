#include <iostream>
#include <vector>
#include <functional>
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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    FibonacciHeap<pair<long long, int>> H;
    vector<FibonacciHeap<pair<long long, int>>::Node*> handle(n + q + 1, nullptr);
    for (int i = 1; i <= n; ++i) {
        long long a;
        cin >> a;
        handle[i] = H.insert({a, i});
    }
    int next_id = n + 1;
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            long long v;
            cin >> v;
            handle[next_id] = H.insert({v, next_id});
            ++next_id;
        }
        else if (type == 2) {
            cout << H.extract_min().second << '\n';
        }
        else {
            int id;
            long long v;
            cin >> id >> v;
            H.decrease_key(handle[id], {v, id});
        }
    }

    return 0;
}
