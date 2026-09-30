#include <iostream>
#include <vector>
#include <cassert>
#include <random>
using namespace::std;

template<typename data_type>
struct PersistentArray {

    struct Node {
        data_type value;
        Node *left, *right;

        Node(const data_type &value = data_type(), Node *left = nullptr, Node *right = nullptr) : value(value), left(left), right(right) {}
    };

    int n;
    vector<Node*> version_roots;

    PersistentArray(const vector<data_type> &a) : n(a.size()) {
        assert(n > 0);
        version_roots.push_back(build(0, n - 1, a));
    }

    PersistentArray(int n, const data_type &initial = data_type()) : PersistentArray(vector<data_type>(n, initial)) {}

    Node* build(int l, int r, const vector<data_type> &a) {
        if (l == r) return new Node(a[l]);
        int mi = (l + r) / 2;
        return new Node(data_type(), build(l, mi, a), build(mi + 1, r, a));
    }

    Node* set(Node *old_root, int l, int r, int pos, const data_type &value) {
        Node *new_root = new Node(*old_root);
        if (l == r) {
            new_root -> value = value;
            return new_root;
        }
        int mi = (l + r) / 2;
        if (pos <= mi) new_root -> left = set(old_root -> left, l, mi, pos, value);
        else new_root -> right = set(old_root -> right, mi + 1, r, pos, value);
        return new_root;
    }

    int set(int version, int pos, const data_type &value) {
        version_roots.push_back(set(version_roots[version], 0, n - 1, pos, value));
        return (int)version_roots.size() - 1;
    }

    const data_type& get(int version, int pos) const {
        Node *node = version_roots[version];
        int l = 0, r = n - 1;
        while (l < r) {
            int mi = (l + r) / 2;
            if (pos <= mi) {
                node = node -> left;
                r = mi;
            }
            else {
                node = node -> right;
                l = mi + 1;
            }
        }
        return node -> value;
    }

    int versions() const {
        return version_roots.size();
    }
};

int main() {
    mt19937 rng(3014);

    int n = 50;
    vector<int> a(n);
    for (int &x : a) x = rng() % 100;

    PersistentArray<int> A(a);
    vector<vector<int>> brute = {a};
    for (int it = 0; it < 5000; ++it) {
        int v = rng() % A.versions();
        int pos = rng() % n;
        int value = rng() % 100;
        A.set(v, pos, value);
        brute.push_back(brute[v]);
        brute.back()[pos] = value;
    }
    for (int v = 0; v < A.versions(); ++v) {
        for (int i = 0; i < n; ++i) assert(A.get(v, i) == brute[v][i]);
    }

    cout << "PersistentArray OK" << '\n';
    return 0;
}
