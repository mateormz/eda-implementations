#include <iostream>
#include <vector>
#include <cassert>
#include <random>
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
        assert(version_roots[version] != nullptr);
        version_roots.push_back(version_roots[version] -> next);
        return (int)version_roots.size() - 1;
    }

    const data_type& top(int version) const {
        assert(version_roots[version] != nullptr);
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
        cout << "(fondo)" << '\n';
    }
};

int main() {
    mt19937 rng(3014);

    PersistentStack<int> S;
    vector<vector<int>> brute = {{}};
    for (int it = 0; it < 5000; ++it) {
        int v = rng() % S.versions();
        if (brute[v].empty() or rng() % 2) {
            int x = rng() % 1000;
            S.push(v, x);
            brute.push_back(brute[v]);
            brute.back().push_back(x);
        }
        else {
            S.pop(v);
            brute.push_back(brute[v]);
            brute.back().pop_back();
        }
    }
    for (int v = 0; v < S.versions(); ++v) {
        assert(S.size(v) == (int)brute[v].size());
        if (!brute[v].empty()) assert(S.top(v) == brute[v].back());
    }

    int v1 = S.push(0, 1);
    int v2 = S.push(v1, 2);
    int v3 = S.push(v1, 3);
    S.print(v2);
    S.print(v3);

    cout << "PersistentStack OK" << '\n';
    return 0;
}
