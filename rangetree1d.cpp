#include <vector>
#include <iostream>
using namespace std;

template<typename data_type, typename build_type, typename index_type>
struct RangeTree{

    struct RangeTreeNode{
        data_type sum;
        index_type l, r;
        RangeTreeNode *left;
        RangeTreeNode *right;

        RangeTreeNode(build_type single_value, index_type l, index_type r) 
            : sum(single_value), l(l), r(r){
                left = right = nullptr;
        }

        RangeTreeNode(data_type sum, index_type l, index_type r)
            : sum(sum), l(l), r(r){
                left = right = nullptr;
        }
    };

    RangeTreeNode *root;

    RangeTree(vector<build_type> &a) {
        root = a.empty() ? nullptr : build_range_tree_from_indices(0, (int)a.size() - 1, a);
    }

    RangeTreeNode* build_range_tree_from_indices(index_type l, index_type r, vector<build_type> &a){
        if (l == r) {
            return new RangeTreeNode(a[l], l, r);
        }

        index_type mi = (l + r) / 2;
        RangeTreeNode *left = build_range_tree_from_indices(l, mi, a);
        RangeTreeNode *right = build_range_tree_from_indices(mi + 1, r, a);
        
        RangeTreeNode *node = new RangeTreeNode(left->sum + right->sum, l, r);
        node->left = left;
        node->right = right;
        return node;
    }

    data_type query(RangeTreeNode *node, index_type l, index_type r) {
        if (node == nullptr) return data_type(0);
        if (r < node->l || node->r < l) return data_type(0);
        if (l <= node->l && node->r <= r) return node->sum;

        return query(node->left, l, r) + query(node->right, l, r);
    }

    data_type query(index_type l, index_type r) {
        return query(root, l, r);
    }

    void print(RangeTreeNode *node) {
        if (node->l == node->r) return;
        print(node->left);
        print(node->right);
    }

    void print() {
        print(root);
        cout << endl;
    }

};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    if(!(cin >> n >> q)) return 0;

    vector<int> a(n);
    for(int i = 0; i < n; ++i) cin >> a[i];

    RangeTree<long long, int, int> Solver(a);

    while(q--) {
        int l, r;
        cin >> l >> r;
        --r;
        cout << Solver.query(l, r) << '\n';
    }

    return 0;
}