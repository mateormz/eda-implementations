#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <random>
using namespace::std;

template<typename data_type>
struct FractionalCascading {
    int k;
    vector<vector<data_type>> augmented;
    vector<vector<int>> own_count;
    vector<vector<int>> bridge;

    FractionalCascading(const vector<vector<data_type>> &lists) : k(lists.size()), augmented(k), own_count(k), bridge(k) {
        for (int i = k - 1; i >= 0; --i) {
            const vector<data_type> &L = lists[i];
            vector<pair<data_type, int>> promoted;
            if (i + 1 < k) {
                for (int j = 1; j < (int)augmented[i + 1].size(); j += 2) {
                    promoted.push_back({augmented[i + 1][j], j});
                }
            }
            vector<data_type> &M = augmented[i];
            vector<int> origin;
            int a = 0, b = 0;
            while (a < (int)L.size() or b < (int)promoted.size()) {
                if (b == (int)promoted.size() or (a < (int)L.size() and !(promoted[b].first < L[a]))) {
                    M.push_back(L[a++]);
                    origin.push_back(-1);
                }
                else {
                    M.push_back(promoted[b].first);
                    origin.push_back(promoted[b].second);
                    ++b;
                }
            }
            int m = M.size();
            own_count[i].assign(m + 1, 0);
            for (int p = 0; p < m; ++p) own_count[i][p + 1] = own_count[i][p] + (origin[p] == -1);
            bridge[i].assign(m + 1, i + 1 < k ? (int)augmented[i + 1].size() : 0);
            for (int p = m - 1; p >= 0; --p) bridge[i][p] = origin[p] != -1 ? origin[p] : bridge[i][p + 1];
        }
    }

    vector<int> lower_bounds(const data_type &x) const {
        vector<int> result(k);
        if (k == 0) return result;
        int pos = lower_bound(augmented[0].begin(), augmented[0].end(), x) - augmented[0].begin();
        for (int i = 0; i < k; ++i) {
            result[i] = own_count[i][pos];
            if (i + 1 < k) {
                int q = bridge[i][pos];
                while (q > 0 and !(augmented[i + 1][q - 1] < x)) --q;
                pos = q;
            }
        }
        return result;
    }

    long long total_size() const {
        long long total = 0;
        for (auto &M : augmented) total += M.size();
        return total;
    }
};

int main() {
    mt19937 rng(3014);

    for (int test = 0; test < 200; ++test) {
        int k = rng() % 20 + 1;
        vector<vector<int>> lists(k);
        for (auto &L : lists) {
            L.resize(rng() % 50);
            for (int &x : L) x = rng() % 200;
            sort(L.begin(), L.end());
        }
        FractionalCascading<int> F(lists);
        for (int it = 0; it < 300; ++it) {
            int x = (int)(rng() % 220) - 10;
            vector<int> result = F.lower_bounds(x);
            for (int i = 0; i < k; ++i) {
                assert(result[i] == lower_bound(lists[i].begin(), lists[i].end(), x) - lists[i].begin());
            }
        }
    }

    int k = 100, n = 10000;
    vector<vector<int>> lists(k, vector<int>(n));
    for (auto &L : lists) {
        for (int &x : L) x = rng();
        sort(L.begin(), L.end());
    }
    FractionalCascading<int> F(lists);
    cout << "tamaño total: " << F.total_size() << " (original " << (long long)k * n << ")" << '\n';

    cout << "FractionalCascading OK" << '\n';
    return 0;
}
