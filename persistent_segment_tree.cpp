#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <random>
using namespace::std;

template<typename data_type>
struct PersistentSegmentTree {

    struct Node {
        data_type value;
        int left, right;
    };

    int n;
    vector<Node> nodes;
    vector<int> version_roots;

    PersistentSegmentTree(int n) : n(n) {
        nodes.push_back({data_type(0), 0, 0});
        version_roots.push_back(0);
    }

    PersistentSegmentTree(const vector<data_type> &a) : PersistentSegmentTree((int)a.size()) {
        version_roots[0] = build(0, n - 1, a);
    }

    int new_node(const Node &node) {
        nodes.push_back(node);
        return (int)nodes.size() - 1;
    }

    int build(int l, int r, const vector<data_type> &a) {
        if (l == r) return new_node({a[l], 0, 0});
        int mi = (l + r) / 2;
        int left = build(l, mi, a);
        int right = build(mi + 1, r, a);
        return new_node({nodes[left].value + nodes[right].value, left, right});
    }

    int update(int last, int l, int r, int pos, const data_type &value, bool accumulate) {
        int curr = new_node(nodes[last]);
        if (l == r) {
            nodes[curr].value = accumulate ? nodes[curr].value + value : value;
            return curr;
        }
        int mi = (l + r) / 2;
        if (pos <= mi) {
            int child = update(nodes[last].left, l, mi, pos, value, accumulate);
            nodes[curr].left = child;
        }
        else {
            int child = update(nodes[last].right, mi + 1, r, pos, value, accumulate);
            nodes[curr].right = child;
        }
        nodes[curr].value = nodes[nodes[curr].left].value + nodes[nodes[curr].right].value;
        return curr;
    }

    int set(int version, int pos, const data_type &value) {
        version_roots.push_back(update(version_roots[version], 0, n - 1, pos, value, false));
        return (int)version_roots.size() - 1;
    }

    int add(int version, int pos, const data_type &value) {
        version_roots.push_back(update(version_roots[version], 0, n - 1, pos, value, true));
        return (int)version_roots.size() - 1;
    }

    data_type query(int root, int l, int r, int x, int y) const {
        if (root == 0 or y < l or r < x) return data_type(0);
        if (x <= l and r <= y) return nodes[root].value;
        int mi = (l + r) / 2;
        return query(nodes[root].left, l, mi, x, y) + query(nodes[root].right, mi + 1, r, x, y);
    }

    data_type query(int version, int x, int y) const {
        if (x > y) return data_type(0);
        return query(version_roots[version], 0, n - 1, x, y);
    }

    int kth(int old_version, int new_version, data_type k) const {
        int a = version_roots[old_version], b = version_roots[new_version];
        int l = 0, r = n - 1;
        while (l < r) {
            int mi = (l + r) / 2;
            data_type left_count = nodes[nodes[b].left].value - nodes[nodes[a].left].value;
            if (k <= left_count) {
                a = nodes[a].left;
                b = nodes[b].left;
                r = mi;
            }
            else {
                k -= left_count;
                a = nodes[a].right;
                b = nodes[b].right;
                l = mi + 1;
            }
        }
        return l;
    }

    int versions() const {
        return version_roots.size();
    }
};

int main() {
    mt19937 rng(3014);

    {
        int n = 40;
        vector<long long> a(n);
        for (auto &x : a) x = rng() % 100;
        PersistentSegmentTree<long long> T(a);
        vector<vector<long long>> brute = {a};
        for (int it = 0; it < 3000; ++it) {
            int v = rng() % T.versions();
            int pos = rng() % n;
            long long value = rng() % 100;
            brute.push_back(brute[v]);
            if (rng() % 2) {
                T.set(v, pos, value);
                brute.back()[pos] = value;
            }
            else {
                T.add(v, pos, value);
                brute.back()[pos] += value;
            }
        }
        for (int it = 0; it < 20000; ++it) {
            int v = rng() % T.versions();
            int x = rng() % n, y = rng() % n;
            if (x > y) swap(x, y);
            long long expected = 0;
            for (int i = x; i <= y; ++i) expected += brute[v][i];
            assert(T.query(v, x, y) == expected);
        }
    }

    {
        int n = 300;
        vector<int> a(n);
        for (int &x : a) x = rng() % 1000;
        vector<int> values = a;
        sort(values.begin(), values.end());
        values.erase(unique(values.begin(), values.end()), values.end());

        PersistentSegmentTree<int> T(values.size());
        for (int i = 0; i < n; ++i) {
            int pos = lower_bound(values.begin(), values.end(), a[i]) - values.begin();
            T.add(i, pos, 1);
        }
        for (int it = 0; it < 5000; ++it) {
            int l = rng() % n, r = rng() % n;
            if (l > r) swap(l, r);
            int k = rng() % (r - l + 1) + 1;
            vector<int> window(a.begin() + l, a.begin() + r + 1);
            nth_element(window.begin(), window.begin() + k - 1, window.end());
            assert(values[T.kth(l, r + 1, k)] == window[k - 1]);
        }
    }

    {
        int n = 300;
        vector<int> a(n);
        for (int &x : a) x = rng() % 30;
        PersistentSegmentTree<int> T(n);
        vector<int> last(30, -1), root_after(n);
        int v = 0;
        for (int i = 0; i < n; ++i) {
            if (last[a[i]] != -1) v = T.add(v, last[a[i]], -1);
            v = T.add(v, i, 1);
            root_after[i] = v;
            last[a[i]] = i;
        }
        for (int it = 0; it < 5000; ++it) {
            int l = rng() % n, r = rng() % n;
            if (l > r) swap(l, r);
            set<int> distinct(a.begin() + l, a.begin() + r + 1);
            assert(T.query(root_after[r], l, r) == (int)distinct.size());
        }
    }

    cout << "PersistentSegmentTree OK" << '\n';
    return 0;
}
