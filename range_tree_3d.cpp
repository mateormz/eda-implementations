#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
using namespace::std;

template<typename coordinate_type>
struct LayeredRangeTree2D {
    using point_type = array<coordinate_type, 3>;

    int n;
    vector<point_type> by_y;
    vector<vector<point_type>> by_z;
    vector<vector<int>> to_left;

    LayeredRangeTree2D(vector<point_type> points) : n(points.size()), by_y(points) {
        sort(by_y.begin(), by_y.end(), [](const point_type &a, const point_type &b) { return a[1] < b[1]; });
        by_z.resize(4 * max(n, 1));
        to_left.resize(4 * max(n, 1));
        if (n > 0) build(1, 0, n - 1);
    }

    void build(int node, int l, int r) {
        if (l == r) {
            by_z[node] = {by_y[l]};
            to_left[node] = {0, 0};
            return;
        }
        int mi = (l + r) / 2;
        build(2 * node, l, mi);
        build(2 * node + 1, mi + 1, r);
        const vector<point_type> &L = by_z[2 * node], &R = by_z[2 * node + 1];
        vector<point_type> &M = by_z[node];
        vector<int> &T = to_left[node];
        T.push_back(0);
        int a = 0, b = 0;
        while (a < (int)L.size() or b < (int)R.size()) {
            bool take_left = b == (int)R.size() or (a < (int)L.size() and !(R[b][2] < L[a][2]));
            M.push_back(take_left ? L[a++] : R[b++]);
            T.push_back(T.back() + take_left);
        }
    }

    template<typename callback_type>
    void query(int node, int l, int r, int a, int b, int lo, int hi, callback_type &callback) const {
        if (b < l or r < a or lo >= hi) return;
        if (a <= l and r <= b) {
            callback(by_z[node], lo, hi);
            return;
        }
        int mi = (l + r) / 2;
        const vector<int> &T = to_left[node];
        query(2 * node, l, mi, a, b, T[lo], T[hi], callback);
        query(2 * node + 1, mi + 1, r, a, b, lo - T[lo], hi - T[hi], callback);
    }

    template<typename callback_type>
    void query(coordinate_type y1, coordinate_type y2, coordinate_type z1, coordinate_type z2, callback_type &callback) const {
        if (n == 0) return;
        int a = lower_bound(by_y.begin(), by_y.end(), y1, [](const point_type &p, coordinate_type v) { return p[1] < v; }) - by_y.begin();
        int b = upper_bound(by_y.begin(), by_y.end(), y2, [](coordinate_type v, const point_type &p) { return v < p[1]; }) - by_y.begin() - 1;
        if (a > b) return;
        const vector<point_type> &root = by_z[1];
        int lo = lower_bound(root.begin(), root.end(), z1, [](const point_type &p, coordinate_type v) { return p[2] < v; }) - root.begin();
        int hi = upper_bound(root.begin(), root.end(), z2, [](coordinate_type v, const point_type &p) { return v < p[2]; }) - root.begin();
        query(1, 0, n - 1, a, b, lo, hi, callback);
    }
};

template<typename coordinate_type>
struct RangeTree3D {
    using point_type = array<coordinate_type, 3>;

    int n;
    vector<point_type> by_x;
    vector<LayeredRangeTree2D<coordinate_type>> secondary;

    RangeTree3D(vector<point_type> points) : n(points.size()), by_x(points) {
        sort(by_x.begin(), by_x.end(), [](const point_type &a, const point_type &b) { return a[0] < b[0]; });
        secondary.assign(4 * max(n, 1), LayeredRangeTree2D<coordinate_type>({}));
        if (n > 0) build(1, 0, n - 1);
    }

    void build(int node, int l, int r) {
        secondary[node] = LayeredRangeTree2D<coordinate_type>(vector<point_type>(by_x.begin() + l, by_x.begin() + r + 1));
        if (l == r) return;
        int mi = (l + r) / 2;
        build(2 * node, l, mi);
        build(2 * node + 1, mi + 1, r);
    }

    template<typename callback_type>
    void query(int node, int l, int r, int a, int b, const array<coordinate_type, 4> &yz, callback_type &callback) const {
        if (b < l or r < a) return;
        if (a <= l and r <= b) {
            secondary[node].query(yz[0], yz[1], yz[2], yz[3], callback);
            return;
        }
        int mi = (l + r) / 2;
        query(2 * node, l, mi, a, b, yz, callback);
        query(2 * node + 1, mi + 1, r, a, b, yz, callback);
    }

    template<typename callback_type>
    void query(const point_type &low, const point_type &high, callback_type &callback) const {
        if (n == 0) return;
        int a = lower_bound(by_x.begin(), by_x.end(), low[0], [](const point_type &p, coordinate_type v) { return p[0] < v; }) - by_x.begin();
        int b = upper_bound(by_x.begin(), by_x.end(), high[0], [](coordinate_type v, const point_type &p) { return v < p[0]; }) - by_x.begin() - 1;
        if (a > b) return;
        query(1, 0, n - 1, a, b, array<coordinate_type, 4>{low[1], high[1], low[2], high[2]}, callback);
    }

    long long count(const point_type &low, const point_type &high) const {
        long long total = 0;
        auto callback = [&](const vector<point_type> &, int lo, int hi) { total += hi - lo; };
        query(low, high, callback);
        return total;
    }

    vector<point_type> report(const point_type &low, const point_type &high) const {
        vector<point_type> result;
        auto callback = [&](const vector<point_type> &v, int lo, int hi) {
            for (int i = lo; i < hi; ++i) result.push_back(v[i]);
        };
        query(low, high, callback);
        return result;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    vector<array<long long, 3>> points(n);
    for (auto &p : points) cin >> p[0] >> p[1] >> p[2];
    RangeTree3D<long long> T(points);
    while (q--) {
        array<long long, 3> low, high;
        cin >> low[0] >> low[1] >> low[2] >> high[0] >> high[1] >> high[2];
        cout << T.count(low, high) << '\n';
    }

    return 0;
}
