#include <iostream>
#include <vector>
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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<long long> a(n);
    for (auto &x : a) cin >> x;
    PersistentArray<long long> A(a);
    int q;
    cin >> q;
    while (q--) {
        int type, v, i;
        cin >> type >> v >> i;
        if (type == 1) {
            long long x;
            cin >> x;
            A.set(v, i - 1, x);
        }
        else {
            cout << A.get(v, i - 1) << '\n';
        }
    }

    return 0;
}
