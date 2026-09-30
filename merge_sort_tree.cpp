#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <random>
using namespace::std;

template<typename data_type>
struct MergeSortTree {
    int n;
    vector<vector<data_type>> tree;

    MergeSortTree(const vector<data_type> &a) : n(a.size()), tree(4 * max((int)a.size(), 1)) {
        if (n > 0) build(1, 0, n - 1, a);
    }

    void build(int node, int l, int r, const vector<data_type> &a) {
        if (l == r) {
            tree[node].push_back(a[l]);
            return;
        }
        int mi = (l + r) / 2;
        build(2 * node, l, mi, a);
        build(2 * node + 1, mi + 1, r, a);
        merge(tree[2 * node].begin(), tree[2 * node].end(),
              tree[2 * node + 1].begin(), tree[2 * node + 1].end(),
              back_inserter(tree[node]));
    }

    int count(int node, int l, int r, int x, int y, const data_type &lo, const data_type &hi) const {
        if (y < l or r < x) return 0;
        if (x <= l and r <= y) {
            const vector<data_type> &v = tree[node];
            return upper_bound(v.begin(), v.end(), hi) - lower_bound(v.begin(), v.end(), lo);
        }
        int mi = (l + r) / 2;
        return count(2 * node, l, mi, x, y, lo, hi) + count(2 * node + 1, mi + 1, r, x, y, lo, hi);
    }

    int count(int x, int y, const data_type &lo, const data_type &hi) const {
        x = max(x, 0);
        y = min(y, n - 1);
        if (x > y or hi < lo) return 0;
        return count(1, 0, n - 1, x, y, lo, hi);
    }

    int count_less_equal(int x, int y, const data_type &k) const {
        x = max(x, 0);
        y = min(y, n - 1);
        if (x > y) return 0;
        return count_leq(1, 0, n - 1, x, y, k);
    }

    int count_greater(int x, int y, const data_type &k) const {
        x = max(x, 0);
        y = min(y, n - 1);
        if (x > y) return 0;
        return (y - x + 1) - count_less_equal(x, y, k);
    }

    int count_leq(int node, int l, int r, int x, int y, const data_type &k) const {
        if (y < l or r < x) return 0;
        if (x <= l and r <= y) return upper_bound(tree[node].begin(), tree[node].end(), k) - tree[node].begin();
        int mi = (l + r) / 2;
        return count_leq(2 * node, l, mi, x, y, k) + count_leq(2 * node + 1, mi + 1, r, x, y, k);
    }
};

int main() {
    mt19937 rng(3014);

    int n = 500;
    vector<int> a(n);
    for (int &x : a) x = rng() % 1000;
    MergeSortTree<int> T(a);

    for (int it = 0; it < 20000; ++it) {
        int x = rng() % n, y = rng() % n;
        if (x > y) swap(x, y);
        int lo = rng() % 1000, hi = rng() % 1000;
        if (lo > hi) swap(lo, hi);
        int k = rng() % 1000;

        int in_range = 0, leq = 0, greater_than = 0;
        for (int i = x; i <= y; ++i) {
            in_range += lo <= a[i] and a[i] <= hi;
            leq += a[i] <= k;
            greater_than += a[i] > k;
        }
        assert(T.count(x, y, lo, hi) == in_range);
        assert(T.count_less_equal(x, y, k) == leq);
        assert(T.count_greater(x, y, k) == greater_than);
    }

    cout << "MergeSortTree OK" << '\n';
    return 0;
}
