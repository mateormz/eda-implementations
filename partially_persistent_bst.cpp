#include <iostream>
#include <vector>
#include <climits>
using namespace::std;

template<typename key_type>
struct PartiallyPersistentBST {

    struct Node;

    struct Modification {
        int field;
        Node *value;
        int version;
    };

    static const int max_modifications = 2;

    struct Node {
        key_type key;
        Node *child[2];
        Modification modifications[max_modifications];
        int used;

        Node(const key_type &key, Node *left = nullptr, Node *right = nullptr) : key(key), used(0) {
            child[0] = left;
            child[1] = right;
        }
    };

    vector<Node*> version_roots;

    PartiallyPersistentBST() {
        version_roots.push_back(nullptr);
    }

    int current() const {
        return (int)version_roots.size() - 1;
    }

    Node* get(Node *node, int field, int version) const {
        Node *result = node -> child[field];
        for (int i = 0; i < node -> used; ++i) {
            if (node -> modifications[i].field == field and node -> modifications[i].version <= version) {
                result = node -> modifications[i].value;
            }
        }
        return result;
    }

    Node* new_node(const key_type &key, Node *left = nullptr, Node *right = nullptr) {
        return new Node(key, left, right);
    }

    void set_field(vector<Node*> &path, vector<int> &dirs, int index, int field, Node *value) {
        Node *x = path[index];
        if (x -> used < max_modifications) {
            x -> modifications[x -> used++] = {field, value, current()};
            return;
        }
        Node *copy = new_node(x -> key, get(x, 0, INT_MAX), get(x, 1, INT_MAX));
        copy -> child[field] = value;
        if (index == 0) version_roots[current()] = copy;
        else set_field(path, dirs, index - 1, dirs[index - 1], copy);
    }

    int insert(const key_type &key) {
        version_roots.push_back(version_roots.back());
        if (version_roots[current()] == nullptr) {
            version_roots[current()] = new_node(key);
            return current();
        }
        vector<Node*> path;
        vector<int> dirs;
        Node *x = version_roots[current()];
        while (x != nullptr) {
            if (!(key < x -> key) and !(x -> key < key)) return current();
            int dir = x -> key < key ? 1 : 0;
            path.push_back(x);
            dirs.push_back(dir);
            x = get(x, dir, current());
        }
        set_field(path, dirs, (int)path.size() - 1, dirs.back(), new_node(key));
        return current();
    }

    bool contains(int version, const key_type &key) const {
        Node *x = version_roots[version];
        while (x != nullptr) {
            if (key < x -> key) x = get(x, 0, version);
            else if (x -> key < key) x = get(x, 1, version);
            else return true;
        }
        return false;
    }

    void inorder(Node *x, int version, vector<key_type> &out) const {
        if (x == nullptr) return;
        inorder(get(x, 0, version), version, out);
        out.push_back(x -> key);
        inorder(get(x, 1, version), version, out);
    }

    vector<key_type> inorder(int version) const {
        vector<key_type> out;
        inorder(version_roots[version], version, out);
        return out;
    }

    int versions() const {
        return version_roots.size();
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int q;
    cin >> q;
    PartiallyPersistentBST<long long> T;
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            long long x;
            cin >> x;
            T.insert(x);
        }
        else {
            int v;
            long long x;
            cin >> v >> x;
            cout << (T.contains(v, x) ? "YES" : "NO") << '\n';
        }
    }

    return 0;
}
