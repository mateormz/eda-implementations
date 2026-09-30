#include <iostream>
#include <vector>
#include <algorithm>
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
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;
    vector<int> sequence(m);
    for (int &x : sequence) {
        cin >> x;
        --x;
    }
    GreedyArboreal G(n);
    cout << G.run(sequence) << '\n';

    return 0;
}
