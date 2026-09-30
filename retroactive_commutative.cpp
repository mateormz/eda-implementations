#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <random>
using namespace::std;

template<typename data_type>
struct PartiallyRetroactiveArray {

    struct Operation {
        int position;
        data_type delta;
    };

    vector<data_type> current;
    map<long long, Operation> timeline;

    PartiallyRetroactiveArray(int n) : current(n, data_type(0)) {}

    void insert(long long time, int position, const data_type &delta) {
        assert(!timeline.count(time));
        timeline[time] = {position, delta};
        current[position] += delta;
    }

    void erase(long long time) {
        auto it = timeline.find(time);
        assert(it != timeline.end());
        current[it -> second.position] -= it -> second.delta;
        timeline.erase(it);
    }

    const data_type& query(int position) const {
        return current[position];
    }
};

int main() {
    mt19937 rng(3014);

    int n = 20;
    PartiallyRetroactiveArray<long long> A(n);
    map<long long, pair<int, long long>> brute;
    for (int it = 0; it < 5000; ++it) {
        if (brute.empty() or rng() % 3) {
            long long time = rng() % 100000;
            if (brute.count(time)) continue;
            int position = rng() % n;
            long long delta = (long long)(rng() % 2001) - 1000;
            A.insert(time, position, delta);
            brute[time] = {position, delta};
        }
        else {
            auto it = brute.begin();
            advance(it, rng() % brute.size());
            A.erase(it -> first);
            brute.erase(it);
        }
        if (it % 50 == 0) {
            vector<long long> expected(n, 0);
            for (auto &[time, op] : brute) expected[op.first] += op.second;
            for (int i = 0; i < n; ++i) assert(A.query(i) == expected[i]);
        }
    }

    cout << "PartiallyRetroactiveArray OK" << '\n';
    return 0;
}
