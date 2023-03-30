#include <iostream>
#include <cassert>

using namespace std;

template <class T>
struct TreeNode {
    T value;
    TreeNode* parent = nullptr;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

template <class T>
void DeleteTree(TreeNode<T>* node) {
    if (!node) {
        return;
    }
    DeleteTree(node->left);
    DeleteTree(node->right);
    delete node;
}

template <class T>
void PrintTree(const TreeNode<T>* root, ostream& out = cout) {
    out << " ( "s;
    out << root->value;
    if (root->left || root->right) {
        if (root->left) {
            PrintTree(root->left, out);
        } else {
            out << "*"s;
        }
        if (root->right) {
            PrintTree(root->right, out);
        } else {
            out << "*"s;
        }
    }
    out << " ) "s;
}

template <class T>
ostream& operator << (ostream& out, const TreeNode<T>* node) {
    PrintTree(node, out);
    return out;
}
/*
Если у узла есть правое поддерево, ищите следующий в нём
Вам пригодится функция begin для поиска следующего в правом поддереве
Если справа ничего нет, идите в родителя. Родитель будет следующим, если вы пришли в него из левого поддерева. А если из правого, то отправляйтесь ещё выше
*/
template <class T>
TreeNode<T>* begin(TreeNode<T>* node) {
    // место для вашей реализации
}

template <class T>
TreeNode<T>* next(TreeNode<T>* node) {
    // место для вашей реализации
}

// функция создаёт новый узел с заданным значением и потомками
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

int main() {
    using T = TreeNode<int>;

    T* root = N(6, N(4, N(3), N(5)), N(8, N(7)));
    cout << root << endl;

    T* iter = begin(root);

    while (iter) {
        cout << iter->value << " "s;
        iter = next(iter);
    }
    cout << endl;

    DeleteTree(root);
}
