#include <iostream>
#include <vector>
#include <random>
using namespace::std;

template<typename data_type>
struct ConfluentPersistentSequence {

    struct Node {
        data_type value;
        long long size;
        Node *left, *right;
    };

    vector<Node*> version_roots;
    mt19937_64 rng;

    ConfluentPersistentSequence() : rng(3014) {
        version_roots.push_back(nullptr);
    }

    static long long size(Node *t) {
        return t == nullptr ? 0 : t -> size;
    }

    static Node* make(const data_type &value, Node *left, Node *right) {
        return new Node{value, 1 + size(left) + size(right), left, right};
    }

    Node* merge(Node *a, Node *b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;
        if ((long long)(rng() % (unsigned long long)(size(a) + size(b))) < size(a)) {
            return make(a -> value, a -> left, merge(a -> right, b));
        }
        return make(b -> value, merge(a, b -> left), b -> right);
    }

    void split(Node *t, long long k, Node *&a, Node *&b) {
        if (t == nullptr) {
            a = b = nullptr;
            return;
        }
        if (size(t -> left) < k) {
            Node *r;
            split(t -> right, k - size(t -> left) - 1, r, b);
            a = make(t -> value, t -> left, r);
        }
        else {
            Node *l;
            split(t -> left, k, a, l);
            b = make(t -> value, l, t -> right);
        }
    }

    Node* build(const vector<data_type> &a, int l, int r) {
        if (l > r) return nullptr;
        int mi = (l + r) / 2;
        return make(a[mi], build(a, l, mi - 1), build(a, mi + 1, r));
    }

    int new_version(Node *root) {
        version_roots.push_back(root);
        return (int)version_roots.size() - 1;
    }

    int create(const vector<data_type> &a) {
        return new_version(build(a, 0, (int)a.size() - 1));
    }

    int concat(int first, int second) {
        return new_version(merge(version_roots[first], version_roots[second]));
    }

    int substring(int version, long long l, long long r) {
        Node *a, *b, *c, *d;
        split(version_roots[version], r, a, b);
        split(a, l, c, d);
        return new_version(d);
    }

    Node* set(Node *t, long long pos, const data_type &value) {
        long long left_size = size(t -> left);
        if (pos < left_size) return make(t -> value, set(t -> left, pos, value), t -> right);
        if (pos > left_size) return make(t -> value, t -> left, set(t -> right, pos - left_size - 1, value));
        return make(value, t -> left, t -> right);
    }

    int set(int version, long long pos, const data_type &value) {
        return new_version(set(version_roots[version], pos, value));
    }

    const data_type& get(int version, long long pos) const {
        Node *t = version_roots[version];
        while (true) {
            long long left_size = size(t -> left);
            if (pos < left_size) t = t -> left;
            else if (pos > left_size) {
                pos -= left_size + 1;
                t = t -> right;
            }
            else return t -> value;
        }
    }

    long long length(int version) const {
        return size(version_roots[version]);
    }

    void to_vector(Node *t, vector<data_type> &out) const {
        if (t == nullptr) return;
        to_vector(t -> left, out);
        out.push_back(t -> value);
        to_vector(t -> right, out);
    }

    vector<data_type> to_vector(int version) const {
        vector<data_type> out;
        to_vector(version_roots[version], out);
        return out;
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
    ConfluentPersistentSequence<long long> S;
    S.create(a);
    int q;
    cin >> q;
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            int u, v;
            cin >> u >> v;
            S.concat(u, v);
        }
        else if (type == 2) {
            int v;
            long long l, r;
            cin >> v >> l >> r;
            S.substring(v, l - 1, r);
        }
        else if (type == 3) {
            int v;
            long long i, x;
            cin >> v >> i >> x;
            S.set(v, i - 1, x);
        }
        else {
            int v;
            long long i;
            cin >> v >> i;
            cout << S.get(v, i - 1) << '\n';
        }
    }

    return 0;
}
