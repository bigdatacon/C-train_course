#include <cassert>
#include <stack>
#include <iostream>

using namespace std;


template <typename T>
struct TreeNode {
	T value;
	TreeNode* left = nullptr;
	TreeNode* right = nullptr;
};


template <typename T>
void DeleteTree(TreeNode<T>* node) {
	if (!node) {
		return;
	}
	DeleteTree(node->left);
	DeleteTree(node->right);
	delete node;
}

template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node, const T* min, const T* max) {
	return (!(node->left) || node->value > *min) && (!(node->right) || node->value < *max);

}


template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node) {

	return	!node || (
		CheckTreeProperty(node, &(node->left->value), &(node->right->value)) &&
		CheckTreeProperty(node->left) && CheckTreeProperty(node->right)
		)
		;

}

int main() {
	using T = TreeNode<int>;
	T* root1 = new T{ 6,
		new T{4, new T{3}, new T{5}}, new T{7} };
	assert(CheckTreeProperty(root1));

	T* root2 = new T{ 6,
		new T{4, new T{3}, new T{5}}, new T{7, new T{8}} };
	assert(!CheckTreeProperty(root2));

	DeleteTree(root1);
	DeleteTree(root2);
}
