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


	T* root3 = new T{ 6,
	new T{4, new T{3}, new T{5}}, new T{2} };
	assert(!CheckTreeProperty(root3));

	T* root4 = new T{ 6,
		new T{10, new T{7}, new T{11}}, new T{15,  new T{12}, new T{16, new T{20}}} };
	assert(!CheckTreeProperty(root4));



	T* root5 = new T{ 26,
		new T{22, new T{18, new T{14, new T{8}, new T{16}}
		
		, new T{19}}
		
		, new T{24}
	}
	};
	assert(CheckTreeProperty(root5));



	T* root6 = new T{ 26,
		new T{28, new T{30, new T{40, new T{46 , new T {60}}


	}
	}
	} };

	assert(!CheckTreeProperty(root6));

	T* root7 = new T{ 26,
	new T{24, new T{23, new T{22, new T{21 , new T {20 , new T {19}}}


}
}
} };

	assert(CheckTreeProperty(root7));



	DeleteTree(root1);
	DeleteTree(root2);
	DeleteTree(root3);
	DeleteTree(root4);
	DeleteTree(root5);
	DeleteTree(root6);
	DeleteTree(root7);

}
