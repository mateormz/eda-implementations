#include <iostream>
#include <vector>
#include <algorithm>
using namespace::std;

template<typename structure_type, typename operation_type>
struct RollbackRetroactive {
    structure_type structure;
    vector<pair<long long, operation_type>> log;

    int position_of(long long time) const {
        return lower_bound(log.begin(), log.end(), time, [](const pair<long long, operation_type> &e, long long t) { return e.first < t; }) - log.begin();
    }

    void rollback(int from) {
        for (int i = (int)log.size() - 1; i >= from; --i) {
            log[i].second.undo(structure);
        }
    }

    void redo(int from) {
        for (int i = from; i < (int)log.size(); ++i) {
            log[i].second.apply(structure);
        }
    }

    void insert(long long time, const operation_type &operation) {
        int i = position_of(time);
        rollback(i);
        log.insert(log.begin() + i, {time, operation});
        redo(i);
    }

    void erase(long long time) {
        int i = position_of(time);
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
    long long value;
    bool popped;
    long long popped_value;

    static StackOperation push(long long value) { return {true, value, false, 0}; }
    static StackOperation pop() { return {false, 0, false, 0}; }

    void apply(vector<long long> &stack) {
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

    void undo(vector<long long> &stack) {
        if (is_push) stack.pop_back();
        else if (popped) stack.push_back(popped_value);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int q;
    cin >> q;
    RollbackRetroactive<vector<long long>, StackOperation> R;
    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            long long t, x;
            cin >> t >> x;
            R.insert(t, StackOperation::push(x));
        }
        else if (type == 2) {
            long long t;
            cin >> t;
            R.insert(t, StackOperation::pop());
        }
        else if (type == 3) {
            long long t;
            cin >> t;
            R.erase(t);
        }
        else {
            cout << (R.now().empty() ? -1 : R.now().back()) << '\n';
        }
    }

    return 0;
}
