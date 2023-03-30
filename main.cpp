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
		return (!min || node->value > min->value) && (!max || node->value < max->value);
	}


	template <typename T>
	bool CheckTreeProperty(const TreeNode<T>* node) {
		const TreeNode<T>* current = node;
		stack<const TreeNode<T>*> s;

		const T* left = nullptr;
		const T* right = nullptr;

		if (current != nullptr) {

			left = current->left;
			right = current->right;
			if (CheckTreeProperty(node, left, right)) {
				s.push(current);
				current = current->left;
			}
			else { return false; }
		}
		else {
			current = s.top();
			s.pop();
			cout << current->value << " ";
			if (CheckTreeProperty(node, left, right)) {
				current = current->right;
			}
			else { return false; }
		}
		return true;
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
