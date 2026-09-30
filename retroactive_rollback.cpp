#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <random>
using namespace::std;

template<typename structure_type, typename operation_type>
struct RollbackRetroactive {
    structure_type structure;
    vector<pair<long long, operation_type>> log;
    long long work = 0;

    int position_of(long long time) const {
        return lower_bound(log.begin(), log.end(), time, [](const pair<long long, operation_type> &e, long long t) { return e.first < t; }) - log.begin();
    }

    void rollback(int from) {
        for (int i = (int)log.size() - 1; i >= from; --i) {
            log[i].second.undo(structure);
            ++work;
        }
    }

    void redo(int from) {
        for (int i = from; i < (int)log.size(); ++i) {
            log[i].second.apply(structure);
            ++work;
        }
    }

    void insert(long long time, const operation_type &operation) {
        int i = position_of(time);
        assert(i == (int)log.size() or log[i].first != time);
        rollback(i);
        log.insert(log.begin() + i, {time, operation});
        redo(i);
    }

    void erase(long long time) {
        int i = position_of(time);
        assert(i < (int)log.size() and log[i].first == time);
        rollback(i);
        log.erase(log.begin() + i);
        redo(i);
    }

    const structure_type& now() const {
        return structure;
    }
};

struct StackOperation {
    bool is_push;
    int value;
    bool popped;
    int popped_value;

    static StackOperation push(int value) { return {true, value, false, 0}; }
    static StackOperation pop() { return {false, 0, false, 0}; }

    void apply(vector<int> &stack) {
        if (is_push) {
            stack.push_back(value);
        }
        else {
            popped = !stack.empty();
            if (popped) {
                popped_value = stack.back();
                stack.pop_back();
            }
        }
    }

    void undo(vector<int> &stack) {
        if (is_push) stack.pop_back();
        else if (popped) stack.push_back(popped_value);
    }
};

int main() {
    mt19937 rng(3014);

    RollbackRetroactive<vector<int>, StackOperation> R;
    vector<pair<long long, StackOperation>> brute;
    for (int it = 0; it < 3000; ++it) {
        if (brute.empty() or rng() % 3) {
            long long time = rng() % 100000;
            bool used = false;
            for (auto &e : brute) used = used or e.first == time;
            if (used) continue;
            StackOperation op = rng() % 2 ? StackOperation::push(rng() % 1000) : StackOperation::pop();
            R.insert(time, op);
            brute.push_back({time, op});
        }
        else {
            int id = rng() % brute.size();
            R.erase(brute[id].first);
            brute.erase(brute.begin() + id);
        }
        sort(brute.begin(), brute.end(), [](auto &a, auto &b) { return a.first < b.first; });
        vector<int> expected;
        for (auto e : brute) e.second.apply(expected);
        assert(R.now() == expected);
    }
    cout << "operaciones rehechas en total: " << R.work << '\n';

    cout << "RollbackRetroactive OK" << '\n';
    return 0;
}
