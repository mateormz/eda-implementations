#include <iostream>
#include <vector>
#include <set>
#include <optional>
using namespace::std;

template<typename data_type>
struct FullyRetroactivePredecessor {

    struct Element {
        data_type value;
        int birth, death;
        bool alive;
    };

    int T;
    vector<multiset<data_type>> tree;
    vector<Element> elements;

    FullyRetroactivePredecessor(int T) : T(T), tree(4 * T) {}

    void update(int node, int l, int r, int a, int b, const data_type &value, bool add) {
        if (b < l or r < a) return;
        if (a <= l and r <= b) {
            if (add) tree[node].insert(value);
            else tree[node].erase(tree[node].find(value));
            return;
        }
        int mi = (l + r) / 2;
        update(2 * node, l, mi, a, b, value, add);
        update(2 * node + 1, mi + 1, r, a, b, value, add);
    }

    void place(const Element &e, bool add) {
        if (e.birth < e.death) update(1, 0, T - 1, e.birth, e.death - 1, e.value, add);
    }

    int insert_operation(int time, const data_type &value) {
        elements.push_back({value, time, T, true});
        place(elements.back(), true);
        return (int)elements.size() - 1;
    }

    void delete_insert_operation(int id) {
        place(elements[id], false);
        elements[id].alive = false;
    }

    void insert_erase_operation(int id, int time) {
        place(elements[id], false);
        elements[id].death = time;
        place(elements[id], true);
    }

    void delete_erase_operation(int id) {
        place(elements[id], false);
        elements[id].death = T;
        place(elements[id], true);
    }

    optional<data_type> predecessor(int time, const data_type &x) const {
        optional<data_type> best;
        int node = 1, l = 0, r = T - 1;
        while (true) {
            const multiset<data_type> &s = tree[node];
            auto it = s.upper_bound(x);
            if (it != s.begin()) {
                --it;
                if (!best or *best < *it) best = *it;
            }
            if (l == r) break;
            int mi = (l + r) / 2;
            if (time <= mi) {
                node = 2 * node;
                r = mi;
            }
            else {
                node = 2 * node + 1;
                l = mi + 1;
            }
        }
        return best;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T, q;
    cin >> T >> q;
    FullyRetroactivePredecessor<long long> P(T);
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            int t;
            long long x;
            cin >> t >> x;
            P.insert_operation(t - 1, x);
        }
        else if (type == 2) {
            int id;
            cin >> id;
            P.delete_insert_operation(id - 1);
        }
        else if (type == 3) {
            int id, t;
            cin >> id >> t;
            P.insert_erase_operation(id - 1, t - 1);
        }
        else if (type == 4) {
            int id;
            cin >> id;
            P.delete_erase_operation(id - 1);
        }
        else {
            int t;
            long long x;
            cin >> t >> x;
            optional<long long> result = P.predecessor(t - 1, x);
            cout << (result ? *result : -1) << '\n';
        }
    }

    return 0;
}
