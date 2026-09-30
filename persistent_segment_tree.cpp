#include <iostream>
#include <vector>
#include <algorithm>
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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<long long> a(n);
    for (auto &x : a) cin >> x;
    PersistentSegmentTree<long long> T(a);
    int q;
    cin >> q;
    while (q--) {
        int type, v;
        cin >> type >> v;
        if (type == 1) {
            int i;
            long long x;
            cin >> i >> x;
            T.set(v, i - 1, x);
        }
        else if (type == 2) {
            int i;
            long long x;
            cin >> i >> x;
            T.add(v, i - 1, x);
        }
        else {
            int l, r;
            cin >> l >> r;
            cout << T.query(v, l - 1, r - 1) << '\n';
        }
    }

    return 0;
}
