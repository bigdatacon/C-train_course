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
		if (node != nullptr) {
			return (!min || node->value > *min) && (!max || node->value < *max);
		}
		else { return false; }
	}

	template <typename T>
	bool CheckTreeProperty(const TreeNode<T>* node) {
		auto min = nullptr;
		auto max = nullptr;
		if (node != nullptr) {
			return	CheckTreeProperty(node, &(node->left->value), &(node->right->value)) &&
				CheckTreeProperty(node->left) && CheckTreeProperty(node->right);
		}
		else { return false; }
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
