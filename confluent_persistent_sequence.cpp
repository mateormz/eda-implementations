#include <iostream>
#include <vector>
#include <cassert>
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
    mt19937 rng(3014);

    ConfluentPersistentSequence<int> S;
    vector<vector<int>> brute = {{}};
    for (int i = 0; i < 5; ++i) {
        vector<int> a(rng() % 20 + 1);
        for (int &x : a) x = rng() % 1000;
        S.create(a);
        brute.push_back(a);
    }
    for (int it = 0; it < 3000; ++it) {
        int op = rng() % 3;
        int v = rng() % S.versions();
        if (op == 0) {
            int w = rng() % S.versions();
            if (brute[v].size() + brute[w].size() > 3000) continue;
            S.concat(v, w);
            vector<int> c = brute[v];
            c.insert(c.end(), brute[w].begin(), brute[w].end());
            brute.push_back(c);
        }
        else if (op == 1) {
            int len = brute[v].size();
            int l = rng() % (len + 1), r = rng() % (len + 1);
            if (l > r) swap(l, r);
            S.substring(v, l, r);
            brute.push_back(vector<int>(brute[v].begin() + l, brute[v].begin() + r));
        }
        else {
            if (brute[v].empty()) continue;
            int pos = rng() % brute[v].size();
            int value = rng() % 1000;
            S.set(v, pos, value);
            brute.push_back(brute[v]);
            brute.back()[pos] = value;
        }
    }
    for (int v = 0; v < S.versions(); ++v) assert(S.to_vector(v) == brute[v]);

    int v = S.create({1, 2, 3});
    for (int i = 0; i < 40; ++i) v = S.concat(v, v);
    assert(S.length(v) == 3LL << 40);
    for (int it = 0; it < 1000; ++it) {
        long long pos = (long long)(rng() % 3) + 3 * (long long)(((unsigned long long)rng() << 32 | rng()) % (1LL << 40));
        assert(S.get(v, pos) == pos % 3 + 1);
    }

    cout << "ConfluentPersistentSequence OK" << '\n';
    return 0;
}
