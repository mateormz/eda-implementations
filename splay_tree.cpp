#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <random>
using namespace::std;

template<typename key_type>
struct SplayTree {

    struct Node {
        key_type key;
        Node *left, *right, *parent;

        Node(const key_type &key) : key(key), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    Node *root = nullptr;
    long long rotations = 0;

    SplayTree() {}
    SplayTree(const SplayTree&) = delete;
    SplayTree& operator=(const SplayTree&) = delete;

    SplayTree(SplayTree &&other) : root(other.root), rotations(other.rotations) {
        other.root = nullptr;
    }

    ~SplayTree() {
        vector<Node*> stack;
        if (root != nullptr) stack.push_back(root);
        while (!stack.empty()) {
            Node *x = stack.back();
            stack.pop_back();
            if (x -> left != nullptr) stack.push_back(x -> left);
            if (x -> right != nullptr) stack.push_back(x -> right);
            delete x;
        }
    }

    void rotate(Node *x) {
        Node *p = x -> parent, *g = p -> parent;
        if (p -> left == x) {
            p -> left = x -> right;
            if (x -> right != nullptr) x -> right -> parent = p;
            x -> right = p;
        }
        else {
            p -> right = x -> left;
            if (x -> left != nullptr) x -> left -> parent = p;
            x -> left = p;
        }
        p -> parent = x;
        x -> parent = g;
        if (g != nullptr) {
            if (g -> left == p) g -> left = x;
            else g -> right = x;
        }
        ++rotations;
    }

    void splay(Node *x) {
        while (x -> parent != nullptr) {
            Node *p = x -> parent, *g = p -> parent;
            if (g != nullptr) {
                if ((g -> left == p) == (p -> left == x)) rotate(p);
                else rotate(x);
            }
            rotate(x);
        }
        root = x;
    }

    void access(const key_type &key) {
        Node *x = root, *last = nullptr;
        while (x != nullptr) {
            last = x;
            if (key < x -> key) x = x -> left;
            else if (x -> key < key) x = x -> right;
            else break;
        }
        if (last != nullptr) splay(last);
    }

    bool contains(const key_type &key) {
        access(key);
        return root != nullptr and !(root -> key < key) and !(key < root -> key);
    }

    bool insert(const key_type &key) {
        if (contains(key)) return false;
        Node *x = new Node(key);
        if (root != nullptr) {
            if (root -> key < key) {
                x -> left = root;
                x -> right = root -> right;
                root -> right = nullptr;
            }
            else {
                x -> right = root;
                x -> left = root -> left;
                root -> left = nullptr;
            }
            if (x -> left != nullptr) x -> left -> parent = x;
            if (x -> right != nullptr) x -> right -> parent = x;
        }
        root = x;
        return true;
    }

    Node* join_nodes(Node *a, Node *b) {
        if (a == nullptr) {
            root = b;
            return b;
        }
        Node *m = a;
        while (m -> right != nullptr) m = m -> right;
        splay(m);
        m -> right = b;
        if (b != nullptr) b -> parent = m;
        return m;
    }

    bool erase(const key_type &key) {
        if (!contains(key)) return false;
        Node *r = root;
        Node *a = r -> left, *b = r -> right;
        if (a != nullptr) a -> parent = nullptr;
        if (b != nullptr) b -> parent = nullptr;
        delete r;
        root = nullptr;
        join_nodes(a, b);
        return true;
    }

    SplayTree split(const key_type &key) {
        SplayTree other;
        if (root == nullptr) return other;
        access(key);
        if (key < root -> key) {
            other.root = root;
            root = root -> left;
            other.root -> left = nullptr;
            if (root != nullptr) root -> parent = nullptr;
        }
        else {
            other.root = root -> right;
            root -> right = nullptr;
            if (other.root != nullptr) other.root -> parent = nullptr;
        }
        return other;
    }

    void join(SplayTree &other) {
        Node *b = other.root;
        other.root = nullptr;
        join_nodes(root, b);
    }

    const key_type& minimum() {
        assert(root != nullptr);
        Node *x = root;
        while (x -> left != nullptr) x = x -> left;
        splay(x);
        return x -> key;
    }

    const key_type& maximum() {
        assert(root != nullptr);
        Node *x = root;
        while (x -> right != nullptr) x = x -> right;
        splay(x);
        return x -> key;
    }

    bool empty() const {
        return root == nullptr;
    }

    void inorder(vector<key_type> &out) const {
        vector<Node*> stack;
        Node *x = root;
        while (x != nullptr or !stack.empty()) {
            while (x != nullptr) {
                stack.push_back(x);
                x = x -> left;
            }
            x = stack.back();
            stack.pop_back();
            out.push_back(x -> key);
            x = x -> right;
        }
    }
};

int main() {
    mt19937 rng(3014);

    SplayTree<int> T;
    set<int> brute;
    for (int it = 0; it < 200000; ++it) {
        int op = rng() % 3;
        int key = rng() % 5000;
        if (op == 0) assert(T.insert(key) == brute.insert(key).second);
        else if (op == 1) assert(T.erase(key) == (brute.erase(key) == 1));
        else assert(T.contains(key) == (brute.count(key) == 1));
        if (it % 20000 == 0 and !brute.empty()) {
            assert(T.minimum() == *brute.begin());
            assert(T.maximum() == *brute.rbegin());
        }
    }
    vector<int> keys;
    T.inorder(keys);
    assert(keys == vector<int>(brute.begin(), brute.end()));

    for (int it = 0; it < 100; ++it) {
        int key = rng() % 5000;
        SplayTree<int> R = T.split(key);
        vector<int> left_keys, right_keys;
        T.inorder(left_keys);
        R.inorder(right_keys);
        for (int x : left_keys) assert(x <= key);
        for (int x : right_keys) assert(x > key);
        assert(left_keys.size() + right_keys.size() == brute.size());
        T.join(R);
    }

    SplayTree<int> S;
    int n = 100000;
    for (int i = 0; i < n; ++i) S.insert(i);
    S.rotations = 0;
    for (int i = 0; i < n; ++i) assert(S.contains(i));
    cout << "acceso secuencial: " << (double)S.rotations / n << " rotaciones por acceso" << '\n';

    cout << "SplayTree OK" << '\n';
    return 0;
}
