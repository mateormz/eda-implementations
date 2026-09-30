#include <iostream>
#include <vector>
#include <deque>
#include <cassert>
#include <random>
using namespace::std;

template<typename data_type>
struct PersistentQueue {

    struct Node {
        data_type value;
        Node *left, *right;

        Node(const data_type &value = data_type(), Node *left = nullptr, Node *right = nullptr) : value(value), left(left), right(right) {}
    };

    struct Version {
        Node *root;
        int front, back;
    };

    int capacity;
    vector<Version> versions_list;

    PersistentQueue(int capacity) : capacity(capacity) {
        assert(capacity > 0);
        versions_list.push_back({build(0, capacity - 1), 0, 0});
    }

    Node* build(int l, int r) {
        if (l == r) return new Node();
        int mi = (l + r) / 2;
        return new Node(data_type(), build(l, mi), build(mi + 1, r));
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

    const data_type& get(Node *node, int pos) const {
        int l = 0, r = capacity - 1;
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

    int push(int version, const data_type &value) {
        Version v = versions_list[version];
        assert(v.back < capacity);
        versions_list.push_back({set(v.root, 0, capacity - 1, v.back, value), v.front, v.back + 1});
        return (int)versions_list.size() - 1;
    }

    int pop(int version) {
        Version v = versions_list[version];
        assert(v.front < v.back);
        versions_list.push_back({v.root, v.front + 1, v.back});
        return (int)versions_list.size() - 1;
    }

    const data_type& front(int version) const {
        const Version &v = versions_list[version];
        assert(v.front < v.back);
        return get(v.root, v.front);
    }

    const data_type& back(int version) const {
        const Version &v = versions_list[version];
        assert(v.front < v.back);
        return get(v.root, v.back - 1);
    }

    int size(int version) const {
        return versions_list[version].back - versions_list[version].front;
    }

    bool empty(int version) const {
        return size(version) == 0;
    }

    int versions() const {
        return versions_list.size();
    }
};

int main() {
    mt19937 rng(3014);

    int operations = 5000;
    PersistentQueue<int> Q(operations);
    vector<deque<int>> brute = {{}};
    for (int it = 0; it < operations; ++it) {
        int v = rng() % Q.versions();
        if (brute[v].empty() or rng() % 2) {
            int x = rng() % 1000;
            Q.push(v, x);
            brute.push_back(brute[v]);
            brute.back().push_back(x);
        }
        else {
            Q.pop(v);
            brute.push_back(brute[v]);
            brute.back().pop_front();
        }
    }
    for (int v = 0; v < Q.versions(); ++v) {
        assert(Q.size(v) == (int)brute[v].size());
        if (!brute[v].empty()) {
            assert(Q.front(v) == brute[v].front());
            assert(Q.back(v) == brute[v].back());
        }
    }

    cout << "PersistentQueue OK" << '\n';
    return 0;
}
