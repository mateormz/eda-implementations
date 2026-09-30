#include <iostream>
#include <vector>
#include <algorithm>
using namespace::std;

template<typename key_type>
struct SplayTree {

    struct Node {
        key_type key;
        Node *left, *right, *parent;

        Node(const key_type &key) : key(key), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    Node *root = nullptr;

    SplayTree() {}
    SplayTree(const SplayTree&) = delete;
    SplayTree& operator=(const SplayTree&) = delete;

    SplayTree(SplayTree &&other) : root(other.root) {
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
        Node *x = root;
        while (x -> left != nullptr) x = x -> left;
        splay(x);
        return x -> key;
    }

    const key_type& maximum() {
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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int q;
    cin >> q;
    SplayTree<long long> T;
    while (q--) {
        int type;
        long long x;
        cin >> type >> x;
        if (type == 1) T.insert(x);
        else if (type == 2) T.erase(x);
        else cout << (T.contains(x) ? "YES" : "NO") << '\n';
    }

    return 0;
}
