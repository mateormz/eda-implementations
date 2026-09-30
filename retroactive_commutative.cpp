#include <iostream>
#include <vector>
#include <map>
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
        timeline[time] = {position, delta};
        current[position] += delta;
    }

    void erase(long long time) {
        auto it = timeline.find(time);
        current[it -> second.position] -= it -> second.delta;
        timeline.erase(it);
    }

    const data_type& query(int position) const {
        return current[position];
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;
    PartiallyRetroactiveArray<long long> A(n);
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            long long t, d;
            int i;
            cin >> t >> i >> d;
            A.insert(t, i - 1, d);
        }
        else if (type == 2) {
            long long t;
            cin >> t;
            A.erase(t);
        }
        else {
            int i;
            cin >> i;
            cout << A.query(i - 1) << '\n';
        }
    }

    return 0;
}
