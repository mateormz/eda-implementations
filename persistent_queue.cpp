#include <iostream>
#include <vector>
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
        versions_list.push_back({set(v.root, 0, capacity - 1, v.back, value), v.front, v.back + 1});
        return (int)versions_list.size() - 1;
    }

    int pop(int version) {
        Version v = versions_list[version];
        versions_list.push_back({v.root, v.front + 1, v.back});
        return (int)versions_list.size() - 1;
    }

    const data_type& front(int version) const {
        const Version &v = versions_list[version];
        return get(v.root, v.front);
    }

    const data_type& back(int version) const {
        const Version &v = versions_list[version];
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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int q;
    cin >> q;
    PersistentQueue<long long> Q(q + 1);
    while (q--) {
        int type, v;
        cin >> type >> v;
        if (type == 1) {
            long long x;
            cin >> x;
            Q.push(v, x);
        }
        else {
            cout << Q.front(v) << '\n';
            Q.pop(v);
        }
    }

    return 0;
}
