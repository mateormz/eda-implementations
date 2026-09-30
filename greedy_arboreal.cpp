#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <random>
using namespace::std;

struct GreedyArboreal {
    int n;
    vector<int> last_touch;
    vector<vector<int>> rows;
    long long cost;

    GreedyArboreal(int n) : n(n), last_touch(n, -1), cost(0) {}

    const vector<int>& access(int x) {
        int time = rows.size();
        vector<int> touched = {x};
        int best = last_touch[x];
        for (int y = x + 1; y < n; ++y) {
            if (last_touch[y] > best) {
                touched.push_back(y);
                best = last_touch[y];
            }
        }
        best = last_touch[x];
        for (int y = x - 1; y >= 0; --y) {
            if (last_touch[y] > best) {
                touched.push_back(y);
                best = last_touch[y];
            }
        }
        for (int y : touched) last_touch[y] = time;
        sort(touched.begin(), touched.end());
        cost += touched.size();
        rows.push_back(touched);
        return rows.back();
    }

    long long run(const vector<int> &sequence) {
        for (int x : sequence) access(x);
        return cost;
    }
};

bool is_arborally_satisfied(int n, const vector<vector<int>> &rows) {
    int m = rows.size();
    vector<vector<int>> grid(m, vector<int>(n, 0));
    for (int t = 0; t < m; ++t) {
        for (int x : rows[t]) grid[t][x] = 1;
    }
    vector<vector<int>> prefix(m + 1, vector<int>(n + 1, 0));
    for (int t = 0; t < m; ++t) {
        for (int x = 0; x < n; ++x) {
            prefix[t + 1][x + 1] = grid[t][x] + prefix[t][x + 1] + prefix[t + 1][x] - prefix[t][x];
        }
    }
    auto points_in = [&](int t1, int t2, int x1, int x2) {
        return prefix[t2 + 1][x2 + 1] - prefix[t1][x2 + 1] - prefix[t2 + 1][x1] + prefix[t1][x1];
    };
    vector<pair<int, int>> points;
    for (int t = 0; t < m; ++t) {
        for (int x : rows[t]) points.push_back({t, x});
    }
    for (auto &[t1, x1] : points) {
        for (auto &[t2, x2] : points) {
            if (t1 >= t2 or x1 == x2) continue;
            if (points_in(t1, t2, min(x1, x2), max(x1, x2)) < 3) return false;
        }
    }
    return true;
}

int main() {
    mt19937 rng(3014);

    for (int test = 0; test < 300; ++test) {
        int n = rng() % 12 + 1, m = rng() % 30 + 1;
        vector<int> sequence(m);
        for (int &x : sequence) x = rng() % n;
        GreedyArboreal G(n);
        G.run(sequence);
        for (int t = 0; t < m; ++t) {
            assert(binary_search(G.rows[t].begin(), G.rows[t].end(), sequence[t]));
        }
        assert(is_arborally_satisfied(n, G.rows));
    }

    assert(!is_arborally_satisfied(3, {{0}, {2}}));
    assert(is_arborally_satisfied(3, {{0}, {0, 2}}));

    int n = 1 << 12;
    vector<int> sequential(n), bit_reversal(n);
    for (int i = 0; i < n; ++i) {
        sequential[i] = i;
        int r = 0;
        for (int b = 0; b < 12; ++b) if (i >> b & 1) r |= 1 << (11 - b);
        bit_reversal[i] = r;
    }
    GreedyArboreal A(n), B(n);
    cout << "secuencial: " << (double)A.run(sequential) / n << " puntos por acceso" << '\n';
    cout << "bit-reversal: " << (double)B.run(bit_reversal) / n << " puntos por acceso" << '\n';

    cout << "GreedyArboreal OK" << '\n';
    return 0;
}
