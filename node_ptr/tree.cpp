#include <cassert>
#include <iostream>
#include <memory>

/*
template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node, const T* min, const T* max, const T* base) {
	
	if (!node) {
		return true;
	}
	if (
		(min && node->value < *min) || (max && node->value > *max)) {
		return false;
	}
	if (   node->right!= nullptr && node->left != nullptr  ) {
		if (node->right->value < node->left->value) { return false; }
	}

	return true;
}

template <typename T>
bool CheckTreeProperty(const TreeNode<T>* root) {
	stack<const TreeNode<T>*> s;
	const TreeNode<T>* prev = nullptr;

	while (root != nullptr || !s.empty()) {
		while (root != nullptr) {
			s.push(root);
			root = root->left;
		}

		root = s.top();
		s.pop();

		if (prev != nullptr && root->value <= prev->value) {
			return false;
		}

		prev = root;
		root = root->right;
	}

	return true;
}

template <class T>
TreeNode<T>* begin(TreeNode<T>* node) {
    // место для вашей реализации
    while (node && node->left) {
        node = node->left;
    }
    return node;
}


template <class T>
TreeNode<T>* next(TreeNode<T>* node) {
    // место для вашей реализации
    if (!node) {
        return nullptr;
    }
    if (node->right) {
        node = node->right;
        return begin(node);
    }
    while (node->parent && node->parent->right == node) {
        node = node->parent;
    }
    return node->parent;
}

TreeNode<int>* N(int val, TreeNode<int>* left = nullptr, TreeNode<int>* right = nullptr) {
    auto res = new TreeNode<int>{val, nullptr, left, right};
    if (left) {
        left->parent = res;
    }
    if (right) {
        right->parent = res;
    }

    return res;
}
*/
/////////////////////////////////////////////////////////////////////////
template <typename T>
struct TreeNode;

template <typename T>
using TreeNodePtr = std::unique_ptr<TreeNode<T>>;

template <typename T>
struct TreeNode {
    // Используйте TreeNodePtr<T> вместо сырых указателей
    // Примите умные указатели по rvalue-ссылке
    TreeNode(T val, TreeNode<T>* left, TreeNode<T>* right)
        : value(std::move(val))
        , left(left) 
        , right(right) {
    }

    T value;
    TreeNode* left;  // Замените TreeNode* на TreeNodePtr<T>
    TreeNode* right; // Замените TreeNode* на TreeNodePtr<T>
    TreeNode* parent = nullptr;
};

template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node) noexcept {
    // реализуйте функцию
    return false;
}

template <typename T>
TreeNode<T>* begin(TreeNode<T>* node) noexcept {
    // Реализуйте функцию
    return nullptr;
}

template <typename T>
TreeNode<T>* next(TreeNode<T>* node) noexcept {
    // Реализуйте функцию
    return nullptr;
}

// функция создаёт новый узел с заданным значением и потомками
TreeNodePtr<int> N(int val, TreeNodePtr<int>&& left = {}, TreeNodePtr<int>&& right = {}) {
    // Реализуйте функцию
    return nullptr;
}

int main() {
    using namespace std;
    using T = TreeNode<int>;
    auto root1 = N(6, N(4, N(3), N(5)), N(7));
    assert(CheckTreeProperty(root1.get()));

    T* iter = begin(root1.get());
    while (iter) {
        cout << iter->value << " "s;
        iter = next(iter);
    }
    cout << endl;

    auto root2 = N(6, N(4, N(3), N(5)), N(7, N(8)));
    assert(!CheckTreeProperty(root2.get()));

    // Функция DeleteTree не нужна. Узлы дерева будут рекурсивно удалены
    // благодаря деструкторам unique_ptr
} 