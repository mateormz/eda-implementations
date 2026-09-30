#include<iostream>
#include<vector>
#include<array>
#include<algorithm>
#include <stack>

using namespace::std;

template<typename build_type>
void merge(vector<build_type> &l, vector<build_type> &r, vector<build_type> &res, int dim) {
    int at = 0;
    for(auto &e : l) {
        while (at < (int)r.size() and r[at][dim] < e[dim]) {
            res.emplace_back(r[at++]);
        }
        res.emplace_back(e);
    }
    while (at < (int)r.size()) {
        res.emplace_back(r[at++]);
    }
}

template<typename build_type, typename node_type>
vector<build_type> compute_sorted_values(int dim, node_type *L, node_type *R) {
    vector<build_type> l = L->next->get_leaves(), r = R->next->get_leaves();
    vector<build_type> res;
    merge(l, r, res, dim);
    return res;
}

template<typename data_type, typename build_type, typename index_type, int K>
struct LastLevelRangeTree {
    struct LastLevelRangeTreeNode {
        data_type sum;
        build_type minimum, maximum;
        LastLevelRangeTreeNode *left;
        LastLevelRangeTreeNode *right;

        LastLevelRangeTreeNode(build_type single_value)
            : sum(single_value[K + 1]), minimum(single_value), maximum(single_value) {
                left = right = nullptr;
            }
        LastLevelRangeTreeNode(data_type sum)
            : sum(sum), minimum(), maximum() {
                left = right = nullptr;
            }
    };

    LastLevelRangeTreeNode* root;

    LastLevelRangeTree(vector<build_type> &a) {
        root = build_range_tree_from_indices(index_type(0), (index_type)a.size() - 1, a);
    }

    LastLevelRangeTreeNode* build_range_tree_from_indices(index_type l, index_type r, vector<build_type> &a) {
        if (l == r) {
            LastLevelRangeTreeNode *node = new LastLevelRangeTreeNode(a[l]);
            return node;
        }
        index_type mi = (l + r) / 2;
        LastLevelRangeTreeNode *left = build_range_tree_from_indices(l, mi, a);
        LastLevelRangeTreeNode *right = build_range_tree_from_indices(mi + 1, r, a);
        LastLevelRangeTreeNode *node = new LastLevelRangeTreeNode(left->sum + right->sum);
        node->left = left;
        node->right = right;
        node->minimum = a[l];
        node->maximum = a[r];
        return node;
    }

    data_type query(LastLevelRangeTreeNode* root, index_type l, index_type r){
        if (r < root->minimum[K] or root->maximum[K] < l) return data_type(0);
        if (l <= root->minimum[K] and root->maximum[K] <= r) {
            return root->sum;
        }
        return query(root->left, l, r) + query(root->right, l, r);
    }

    data_type query(index_type l, index_type r) {
        return query(root, l, r);
    }

    vector<build_type> get_leaves() {
        vector<build_type> res;
        stack<LastLevelRangeTreeNode*> S;
        S.emplace(root);
        while(not S.empty()) {
            LastLevelRangeTreeNode *node = S.top(); S.pop();
            if (node->left or node->right) {
                if (node->right) S.emplace(node->right);
                if (node->left) S.emplace(node->left);
            }
            else {
                res.emplace_back(node->minimum);
            }
        }
        return res;
    }
};

template<typename data_type, typename build_type, typename index_type, int K>
struct TrivialRangeTree {
    
    struct TrivialRangeTreeNode {
        data_type sum;
        build_type minimum, maximum;
        LastLevelRangeTree<data_type, build_type, index_type, K + 1> *next;
        TrivialRangeTreeNode *left;
        TrivialRangeTreeNode *right;

        TrivialRangeTreeNode(build_type single_value) : sum(single_value[K]), minimum(single_value), maximum(single_value) {
            left = right = nullptr;
        }

        TrivialRangeTreeNode(data_type sum) : sum(sum), minimum(), maximum() {
            left = right = nullptr;
        }
    };

    TrivialRangeTreeNode* root;

    TrivialRangeTree(vector<build_type>& a) {
        root = build_range_tree_from_indices(0, (index_type)a.size() - 1, a);
    }

    TrivialRangeTreeNode* build_range_tree_from_indices(index_type l, index_type r, vector<build_type> &a) {
        if (l == r) {
            TrivialRangeTreeNode *node = new TrivialRangeTreeNode(a[l]);
            vector<build_type> single_value = {a[l]};
            node->next = new LastLevelRangeTree<data_type, build_type, index_type, K + 1>(single_value);
            node->minimum = a[l];
            node->maximum = a[l];
            return node;
        }
        index_type mi = (l + r) / 2;
        TrivialRangeTreeNode *left = build_range_tree_from_indices(l, mi, a);
        TrivialRangeTreeNode *right = build_range_tree_from_indices(mi + 1, r, a);
        TrivialRangeTreeNode *node = new TrivialRangeTreeNode(left->sum + right->sum);
        node->left = left;
        node->right = right;
        node->minimum = a[l];
        node->maximum = a[r];
        vector<build_type> sorted_by_next_dimension = compute_sorted_values<build_type, TrivialRangeTreeNode>(K + 1, node->left, node->right);
        node->next = new LastLevelRangeTree<data_type, build_type, index_type, K + 1>(sorted_by_next_dimension);
        return node;
    }

    data_type query(TrivialRangeTreeNode *root, index_type l, index_type r, index_type d, index_type u) {
        if (r < root->minimum[K] or root->maximum[K] < l) return data_type(0);
        if (l <= root->minimum[K] and root->maximum[K] <= r) {
            return root->next->query(d, u);
        }
        return query(root->left, l, r, d, u) + query(root->right, l, r, d, u);
    }

    data_type query(index_type l, index_type r, index_type d, index_type u) {
        return query(root, l, r, d, u);
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, q;
    cin >> n >> q;
    vector<array<int, 3>> a(n);
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < 3; ++j) {
            cin >> a[i][j];
        }
    }
    sort(a.begin(), a.end());
    TrivialRangeTree<long long, array<int, 3>, int, 0> Solver(a);
    while (q--) {
        int l, r, d, u;
        cin >> l >> d >> r >> u;
        --r; --u;
        cout << Solver.query(l, r, d, u) << '\n';
    }
    return 0;
}