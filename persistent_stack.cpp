#include <iostream>
#include <vector>
using namespace::std;

template<typename data_type>
struct PersistentStack {

    struct StackNode {
        data_type data;
        int size;
        StackNode *next;

        StackNode(const data_type &data, StackNode *next) : data(data), size(next == nullptr ? 1 : next -> size + 1), next(next) {}
    };

    vector<StackNode*> version_roots;

    PersistentStack() {
        version_roots.push_back(nullptr);
    }

    int push(int version, const data_type &data) {
        version_roots.push_back(new StackNode(data, version_roots[version]));
        return (int)version_roots.size() - 1;
    }

    int pop(int version) {
        version_roots.push_back(version_roots[version] -> next);
        return (int)version_roots.size() - 1;
    }

    const data_type& top(int version) const {
        return version_roots[version] -> data;
    }

    int size(int version) const {
        return version_roots[version] == nullptr ? 0 : version_roots[version] -> size;
    }

    bool empty(int version) const {
        return version_roots[version] == nullptr;
    }

    int versions() const {
        return version_roots.size();
    }

    void print(int version) const {
        for (StackNode *node = version_roots[version]; node != nullptr; node = node -> next) {
            cout << node -> data << ' ';
        }
        cout << '\n';
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int q;
    cin >> q;
    PersistentStack<long long> S;
    while (q--) {
        int type, v;
        cin >> type >> v;
        if (type == 1) {
            long long x;
            cin >> x;
            S.push(v, x);
        }
        else {
            cout << S.top(v) << '\n';
            S.pop(v);
        }
    }

    return 0;
}
