#include <iostream>
#include <vector>
#include <functional>
using namespace::std;

template<typename key_type, typename compare = less<key_type>>
struct BinomialHeap {

    struct Node;

    struct Handle {
        Node *node;
    };

    struct Node {
        key_type key;
        int degree;
        Node *parent, *child, *sibling;
        Handle *handle;

        Node(const key_type &key, Handle *handle) : key(key), degree(0), parent(nullptr), child(nullptr), sibling(nullptr), handle(handle) {}
    };

    Node *head = nullptr;
    int n = 0;
    compare cmp;

    BinomialHeap() {}
    BinomialHeap(const BinomialHeap&) = delete;
    BinomialHeap& operator=(const BinomialHeap&) = delete;

    ~BinomialHeap() {
        destroy(head);
    }

    void destroy(Node *x) {
        while (x != nullptr) {
            destroy(x -> child);
            Node *next = x -> sibling;
            delete x -> handle;
            delete x;
            x = next;
        }
    }

    static Node* merge_lists(Node *a, Node *b) {
        Node *result = nullptr;
        Node **tail = &result;
        while (a != nullptr and b != nullptr) {
            if (a -> degree <= b -> degree) {
                *tail = a;
                a = a -> sibling;
            }
            else {
                *tail = b;
                b = b -> sibling;
            }
            tail = &(*tail) -> sibling;
        }
        *tail = a != nullptr ? a : b;
        return result;
    }

    static void link(Node *y, Node *z) {
        y -> parent = z;
        y -> sibling = z -> child;
        z -> child = y;
        ++z -> degree;
    }

    Node* union_lists(Node *a, Node *b) {
        Node *result = merge_lists(a, b);
        if (result == nullptr) return nullptr;
        Node *prev = nullptr, *x = result, *next = x -> sibling;
        while (next != nullptr) {
            if (x -> degree != next -> degree or (next -> sibling != nullptr and next -> sibling -> degree == x -> degree)) {
                prev = x;
                x = next;
            }
            else if (!cmp(next -> key, x -> key)) {
                x -> sibling = next -> sibling;
                link(next, x);
            }
            else {
                if (prev == nullptr) result = next;
                else prev -> sibling = next;
                link(x, next);
                x = next;
            }
            next = x -> sibling;
        }
        return result;
    }

    Handle* insert(const key_type &key) {
        Handle *handle = new Handle();
        Node *x = new Node(key, handle);
        handle -> node = x;
        head = union_lists(head, x);
        ++n;
        return handle;
    }

    void merge(BinomialHeap &other) {
        head = union_lists(head, other.head);
        n += other.n;
        other.head = nullptr;
        other.n = 0;
    }

    Node* min_root(Node *&prev_of_min) const {
        Node *best = head, *prev = nullptr;
        prev_of_min = nullptr;
        for (Node *x = head; x != nullptr; prev = x, x = x -> sibling) {
            if (cmp(x -> key, best -> key)) {
                best = x;
                prev_of_min = prev;
            }
        }
        return best;
    }

    const key_type& minimum() const {
        Node *prev;
        return min_root(prev) -> key;
    }

    void remove_root(Node *root, Node *prev) {
        if (prev == nullptr) head = root -> sibling;
        else prev -> sibling = root -> sibling;
        Node *child = root -> child, *reversed = nullptr;
        while (child != nullptr) {
            Node *next = child -> sibling;
            child -> sibling = reversed;
            child -> parent = nullptr;
            reversed = child;
            child = next;
        }
        head = union_lists(head, reversed);
        delete root -> handle;
        delete root;
        --n;
    }

    key_type extract_min() {
        Node *prev;
        Node *root = min_root(prev);
        key_type key = root -> key;
        remove_root(root, prev);
        return key;
    }

    static void swap_with_parent(Node *x) {
        Node *p = x -> parent;
        swap(x -> key, p -> key);
        swap(x -> handle, p -> handle);
        x -> handle -> node = x;
        p -> handle -> node = p;
    }

    void decrease_key(Handle *handle, const key_type &key) {
        Node *x = handle -> node;
        x -> key = key;
        while (x -> parent != nullptr and cmp(x -> key, x -> parent -> key)) {
            swap_with_parent(x);
            x = x -> parent;
        }
    }

    void erase(Handle *handle) {
        Node *x = handle -> node;
        while (x -> parent != nullptr) {
            swap_with_parent(x);
            x = x -> parent;
        }
        Node *prev = nullptr;
        for (Node *y = head; y != x; y = y -> sibling) prev = y;
        remove_root(x, prev);
    }

    const key_type& key(Handle *handle) const {
        return handle -> node -> key;
    }

    int size() const { return n; }
    bool empty() const { return n == 0; }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    BinomialHeap<pair<long long, int>> H;
    vector<BinomialHeap<pair<long long, int>>::Handle*> handle(n + q + 1, nullptr);
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
