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
bool CheckTreeProperty(const TreeNode<T>* node , const T* min, const T* max) {
	if (!node) {
		return true;
	}
	if (node->value < *min || node->value > *max) {
		return false;
	}
	return CheckTreeProperty(node->left, nullptr, &(node->value)) && CheckTreeProperty(node->right, node->value, nullptr);
}

template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node) {
	return
		CheckTreeProperty(node, &(node->left->value), &(node->right->value));

}


int main() {
	using T = TreeNode<int>;
	T* root1 = new T{ 6,
		new T{4, new T{3}, new T{5}}, new T{7} };
	assert(CheckTreeProperty(root1));

	T* root2 = new T{ 6,
		new T{4, new T{3}, new T{5}}, new T{7, new T{8}} };
	assert(!CheckTreeProperty(root2));


	T* root3 = new T{ 6,
	new T{4, new T{3}, new T{5}}, new T{2} };
	assert(!CheckTreeProperty(root3));

	T* root4 = new T{ 6,
		new T{10, new T{7}, new T{11}}, new T{15,  new T{12}, new T{16, new T{20}}} };
	assert(!CheckTreeProperty(root4));




	T* root5 = new T{ 26 , new T{24, new T{22, new T{20}, nullptr}, nullptr}, nullptr }; // только левые узлы 
	assert(CheckTreeProperty(root5));



	T* root6 = new T{ 26 , nullptr, new T{30, nullptr , new T{50 , nullptr , new T {60} }} };  // только првые узлы
	assert(CheckTreeProperty(root6));




	T* root6_5 = new T{100,
		new T { 80, new T{60}, new T{350, new T{40}, new  T {550}} } , new T {800} };
	assert(CheckTreeProperty(root6_5));


	T* root7 = new T{ 100,
		new T { 80, new T{60}, new T{600, new T{40}, new  T {550}} } , new T {800} };
	assert(!CheckTreeProperty(root7));

	T* root7_5 = new T{ 100,
	new T { 80, new T{60}, new T{190, new T{40}, new  T {550}} } , new T {800} }; // значение в правой и левой ветке больше коря 
	assert(CheckTreeProperty(root7_5));


	T* root8 = new T{ 100,
		new T{80, new T{60, new T {40} , new T {600}}, new T{700}} , new T {800, new T{100} , new T {1500}} }; //значение в правой ветке равное корню 
	assert(CheckTreeProperty(root8));

	T* root9 = new T{ 100,
	new T{80, new T{60, new T {40} , new T {100}}, new T{700}} , new T {800, new T{100} , new T {1500}} }; //значение в левой ветке равное корню 
		assert(CheckTreeProperty(root9));


	DeleteTree(root1);
	DeleteTree(root2);
	DeleteTree(root3);
	DeleteTree(root4);
	DeleteTree(root5);
	DeleteTree(root6);
	DeleteTree(root7);
	DeleteTree(root7_5);
	DeleteTree(root6_5);
	DeleteTree(root8);
	DeleteTree(root9);

}
