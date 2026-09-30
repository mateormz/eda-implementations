#include <iostream>
#include <vector>
#include <set>
#include <optional>
#include <cassert>
#include <random>
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
        assert(elements[id].death == T and elements[id].birth <= time);
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
    mt19937 rng(3014);

    int T = 200;
    FullyRetroactivePredecessor<int> P(T);
    struct Brute { int value, birth, death; bool alive; };
    vector<Brute> brute;

    for (int it = 0; it < 5000; ++it) {
        int op = rng() % 4;
        if (brute.empty() or op == 0) {
            int time = rng() % T, value = rng() % 1000;
            P.insert_operation(time, value);
            brute.push_back({value, time, T, true});
        }
        else {
            int id = rng() % brute.size();
            if (!brute[id].alive) continue;
            if (op == 1) {
                P.delete_insert_operation(id);
                brute[id].alive = false;
            }
            else if (op == 2 and brute[id].death == T) {
                int time = brute[id].birth + rng() % (T - brute[id].birth + 1);
                P.insert_erase_operation(id, time);
                brute[id].death = time;
            }
            else if (op == 3 and brute[id].death != T) {
                P.delete_erase_operation(id);
                brute[id].death = T;
            }
        }
        for (int q = 0; q < 5; ++q) {
            int time = rng() % T, x = rng() % 1000;
            optional<int> expected;
            for (auto &e : brute) {
                if (e.alive and e.birth <= time and time < e.death and e.value <= x) {
                    if (!expected or *expected < e.value) expected = e.value;
                }
            }
            assert(P.predecessor(time, x) == expected);
        }
    }

    cout << "FullyRetroactivePredecessor OK" << '\n';
    return 0;
}
