#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
using namespace::std;

template<typename key_type>
struct PartiallyRetroactivePriorityQueue {

    enum SlotType { EMPTY, INSERT, DELETE_MIN };

    struct Best {
        bool valid;
        key_type key;
        int position;
    };

    struct SegmentNode {
        int sum, min_prefix;
        Best max_out, min_in;
    };

    int T;
    vector<SlotType> type;
    vector<key_type> key;
    vector<bool> in_now;
    vector<SegmentNode> tree;
    multiset<key_type> now;

    PartiallyRetroactivePriorityQueue(int T) : T(T), type(T, EMPTY), key(T), in_now(T, false), tree(4 * T) {
        build(1, 0, T - 1);
    }

    static Best better_max(const Best &a, const Best &b) {
        if (!a.valid) return b;
        if (!b.valid) return a;
        return a.key < b.key ? b : a;
    }

    static Best better_min(const Best &a, const Best &b) {
        if (!a.valid) return b;
        if (!b.valid) return a;
        return b.key < a.key ? b : a;
    }

    SegmentNode leaf(int t) const {
        SegmentNode node = {0, 0, {false, key_type(), t}, {false, key_type(), t}};
        if (type[t] == INSERT) {
            if (in_now[t]) node.min_in = {true, key[t], t};
            else {
                node.sum = node.min_prefix = 1;
                node.max_out = {true, key[t], t};
            }
        }
        else if (type[t] == DELETE_MIN) {
            node.sum = node.min_prefix = -1;
        }
        return node;
    }

    static SegmentNode combine(const SegmentNode &a, const SegmentNode &b) {
        return {a.sum + b.sum, min(a.min_prefix, a.sum + b.min_prefix), better_max(a.max_out, b.max_out), better_min(a.min_in, b.min_in)};
    }

    void build(int node, int l, int r) {
        if (l == r) {
            tree[node] = leaf(l);
            return;
        }
        int mi = (l + r) / 2;
        build(2 * node, l, mi);
        build(2 * node + 1, mi + 1, r);
        tree[node] = combine(tree[2 * node], tree[2 * node + 1]);
    }

    void refresh(int node, int l, int r, int t) {
        if (l == r) {
            tree[node] = leaf(t);
            return;
        }
        int mi = (l + r) / 2;
        if (t <= mi) refresh(2 * node, l, mi, t);
        else refresh(2 * node + 1, mi + 1, r, t);
        tree[node] = combine(tree[2 * node], tree[2 * node + 1]);
    }

    void refresh(int t) {
        refresh(1, 0, T - 1, t);
    }

    Best query_max_out(int node, int l, int r, int a, int b) const {
        if (b < l or r < a) return {false, key_type(), -1};
        if (a <= l and r <= b) return tree[node].max_out;
        int mi = (l + r) / 2;
        return better_max(query_max_out(2 * node, l, mi, a, b), query_max_out(2 * node + 1, mi + 1, r, a, b));
    }

    Best query_min_in(int node, int l, int r, int a, int b) const {
        if (b < l or r < a) return {false, key_type(), -1};
        if (a <= l and r <= b) return tree[node].min_in;
        int mi = (l + r) / 2;
        return better_min(query_min_in(2 * node, l, mi, a, b), query_min_in(2 * node + 1, mi + 1, r, a, b));
    }

    int find_last_zero(int node, int l, int r, int qr, int accumulated) const {
        if (l > qr or accumulated + tree[node].min_prefix > 0) return -1;
        if (l == r) return l;
        int mi = (l + r) / 2;
        int result = find_last_zero(2 * node + 1, mi + 1, r, qr, accumulated + tree[2 * node].sum);
        if (result != -1) return result;
        return find_last_zero(2 * node, l, mi, qr, accumulated);
    }

    int find_first_zero(int node, int l, int r, int ql, int accumulated) const {
        if (r < ql or accumulated + tree[node].min_prefix > 0) return -1;
        if (l == r) return l;
        int mi = (l + r) / 2;
        int result = find_first_zero(2 * node, l, mi, ql, accumulated);
        if (result != -1) return result;
        return find_first_zero(2 * node + 1, mi + 1, r, ql, accumulated + tree[2 * node].sum);
    }

    int last_bridge_at_or_before(int t) const {
        if (t == 0) return 0;
        return find_last_zero(1, 0, T - 1, t - 1, 0) + 1;
    }

    int first_bridge_after(int t) const {
        return find_first_zero(1, 0, T - 1, t, 0) + 1;
    }

    void mark(int t, bool value) {
        in_now[t] = value;
        if (value) now.insert(key[t]);
        else now.erase(now.find(key[t]));
        refresh(t);
    }

    void insert_push(int t, const key_type &k) {
        int bridge = last_bridge_at_or_before(t);
        Best best = bridge < T ? query_max_out(1, 0, T - 1, bridge, T - 1) : Best{false, key_type(), -1};
        type[t] = INSERT;
        key[t] = k;
        in_now[t] = false;
        if (best.valid and k < best.key) {
            refresh(t);
            mark(best.position, true);
        }
        else {
            mark(t, true);
        }
    }

    void insert_delete_min(int t) {
        int bridge = first_bridge_after(t);
        Best best = query_min_in(1, 0, T - 1, 0, bridge - 1);
        type[t] = DELETE_MIN;
        refresh(t);
        mark(best.position, false);
    }

    void erase_push(int t) {
        if (in_now[t]) {
            mark(t, false);
            type[t] = EMPTY;
            refresh(t);
            return;
        }
        int bridge = first_bridge_after(t);
        Best best = query_min_in(1, 0, T - 1, 0, bridge - 1);
        type[t] = EMPTY;
        refresh(t);
        mark(best.position, false);
    }

    void erase_delete_min(int t) {
        int bridge = last_bridge_at_or_before(t);
        Best best = query_max_out(1, 0, T - 1, bridge, T - 1);
        type[t] = EMPTY;
        refresh(t);
        mark(best.position, true);
    }

    const key_type& minimum() const {
        return *now.begin();
    }

    int size() const {
        return now.size();
    }

    vector<key_type> current() const {
        return vector<key_type>(now.begin(), now.end());
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T, q;
    cin >> T >> q;
    PartiallyRetroactivePriorityQueue<long long> Q(T);
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            int t;
            long long k;
            cin >> t >> k;
            Q.insert_push(t - 1, k);
        }
        else if (type == 2) {
            int t;
            cin >> t;
            Q.insert_delete_min(t - 1);
        }
        else if (type == 3) {
            int t;
            cin >> t;
            Q.erase_push(t - 1);
        }
        else if (type == 4) {
            int t;
            cin >> t;
            Q.erase_delete_min(t - 1);
        }
        else {
            cout << (Q.size() == 0 ? -1 : Q.minimum()) << '\n';
        }
    }

    return 0;
}
