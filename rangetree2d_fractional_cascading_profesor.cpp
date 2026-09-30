#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

using namespace std;

template<typename data_type, typename build_type, typename index_type, int K>
struct FractionalCascadingRangeTree {
    using coordinate_type = typename build_type::value_type;

    struct FractionalCascadingRangeTreeNode {
        build_type minimum, maximum;
        vector<build_type> sorted_by_next_dimension;
        vector<index_type> left_count, right_count;
        FractionalCascadingRangeTreeNode *left;
        FractionalCascadingRangeTreeNode *right;

        FractionalCascadingRangeTreeNode(build_type minimum, build_type maximum)
            : minimum(minimum), maximum(maximum), left(nullptr), right(nullptr) {}

        ~FractionalCascadingRangeTreeNode() {
            delete left;
            delete right;
        }
    };

    FractionalCascadingRangeTreeNode *root;

    FractionalCascadingRangeTree(vector<build_type> &a) {
        root = a.empty() ? nullptr : build_range_tree_from_indices(0, (index_type)a.size() - 1, a);
    }

    ~FractionalCascadingRangeTree() {
        delete root;
    }

    FractionalCascadingRangeTree(const FractionalCascadingRangeTree &) = delete;
    FractionalCascadingRangeTree &operator=(const FractionalCascadingRangeTree &) = delete;

    FractionalCascadingRangeTreeNode* build_range_tree_from_indices(
        index_type l, index_type r, vector<build_type> &a) {
        auto *node = new FractionalCascadingRangeTreeNode(a[l], a[r]);
        if (l == r) {
            node->sorted_by_next_dimension.emplace_back(a[l]);
            return node;
        }

        index_type mi = (l + r) / 2;
        node->left = build_range_tree_from_indices(l, mi, a);
        node->right = build_range_tree_from_indices(mi + 1, r, a);

        const auto &L = node->left->sorted_by_next_dimension;
        const auto &R = node->right->sorted_by_next_dimension;
        auto &sorted = node->sorted_by_next_dimension;
        sorted.reserve(L.size() + R.size());
        node->left_count.reserve(L.size() + R.size() + 1);
        node->right_count.reserve(L.size() + R.size() + 1);
        node->left_count.emplace_back(0);
        node->right_count.emplace_back(0);

        index_type i = 0, j = 0;
        while (i < (index_type)L.size() or j < (index_type)R.size()) {
            if (j == (index_type)R.size() or
                (i < (index_type)L.size() and L[i][K + 1] <= R[j][K + 1])) {
                sorted.emplace_back(L[i++]);
            } else {
                sorted.emplace_back(R[j++]);
            }
            node->left_count.emplace_back(i);
            node->right_count.emplace_back(j);
        }
        return node;
    }

    data_type query_with_positions(FractionalCascadingRangeTreeNode *node,
                                   coordinate_type l, coordinate_type r,
                                   index_type low, index_type high) {
        if (node == nullptr or low == high or
            r < node->minimum[K] or node->maximum[K] < l) return data_type(0);

        if (l <= node->minimum[K] and node->maximum[K] <= r) {
            return data_type(high - low);
        }

        return query_with_positions(node->left, l, r,
                                    node->left_count[low], node->left_count[high]) +
               query_with_positions(node->right, l, r,
                                    node->right_count[low], node->right_count[high]);
    }

    data_type query(coordinate_type l, coordinate_type r,
                    coordinate_type d, coordinate_type u) {
        if (root == nullptr or l > r or d > u) return data_type(0);

        const auto &sorted = root->sorted_by_next_dimension;
        index_type low = lower_bound(sorted.begin(), sorted.end(), d,
            [](const build_type &point, coordinate_type value) {
                return point[K + 1] < value;
            }) - sorted.begin();
        index_type high = upper_bound(sorted.begin(), sorted.end(), u,
            [](coordinate_type value, const build_type &point) {
                return value < point[K + 1];
            }) - sorted.begin();

        return query_with_positions(root, l, r, low, high);
    }
};

int main() {
    cin.tie(0)->sync_with_stdio(false);

    int n, q;
    if (!(cin >> n >> q)) return 0;

    vector<array<long long, 2>> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i][0] >> a[i][1];
    }
    sort(a.begin(), a.end());

    FractionalCascadingRangeTree<long long, array<long long, 2>, int, 0> Solver(a);
    while (q--) {
        long long l, r, d, u;
        cin >> l >> r >> d >> u;
        cout << Solver.query(l, r, d, u) << '\n';
    }
    return 0;
}
