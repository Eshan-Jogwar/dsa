#include<stdio.h>
#include<stdlib.h>
typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} tnode;

tnode *createTreeNode(int data) {
    tnode *newNode = (tnode*)malloc(sizeof(tnode));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void inOrder(tnode *root) {
    if (root == NULL) return;
    inOrder(root->left);
    printf(" %d ", root->data);
    inOrder(root->right);
}

tnode *insert(tnode *root, int data) {
    if (root == NULL) {
        return createTreeNode(data);
    }

    if (root->data > data) {
        root->left = insert(root->left, data);
    }
    else {
        root->right = insert(root->right, data);
    }
    return root;
}
tnode *rightRotate(tnode *x) {
    tnode *y = x->left;
    tnode *T2 = y->right;

    y->right = x;
    x->left = T2;

    return y;
}
tnode *leftRotate(tnode *x) {
    tnode *y = x->right;
    tnode *T1 = y->left;

    y->left = x;
    x->right = T1;

    return y;
}

tnode *splay(tnode *root, int data) {
    if (root == NULL || root->data == data)
        return root;


    if (root->data > data) {
        if (root->left == NULL) return root;

        if (root->left->data > data) {
            root->left->left = splay(root->left->left, data);
            root = rightRotate(root);
        }
        else if (root->left->data < data) {
            root->left->right = splay(root->left->right, data);
            if (root->left->right != NULL)
                root->left = leftRotate(root->left);
        }

        if (root->left == NULL)
            return root;
        else
            return rightRotate(root);
    }
    else {
        if (root->right == NULL) return root;

        if (root->right->data > data) {
            root->right->left = splay(root->right->left, data);
            if (root->right->left != NULL)
                root->right = rightRotate(root->right);
        }
        else if (root->right->data < data) {
            root->right->right = splay(root->right->right, data);
            root = leftRotate(root);
        }

        if (root->right == NULL)
            return root;
        else
            return leftRotate(root);
    }
}

tnode *findMin(tnode *root) {
    tnode *current = root;
    while (current && current->left) {
        current = current->left;
    } 
    return current;
}

tnode *delete(tnode *root, int data) {
    if (root == NULL) return NULL;

    if (root->data > data) { 
        root->left = delete(root->left, data);
    }
    else if (root->data < data) {
        root->right = delete(root->right, data);
    }
    else {
        if (root->data == data) {
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        else if (root->left == NULL) {
            tnode *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL){
            tnode *temp = root->left;
            free(root);
            return temp;
        }
        else {
            tnode *temp_right = root->right;
            tnode *temp_left = root->left;
            free(root);

            tnode *minimum = findMin(temp_right);
            minimum->left = temp_left;
            return temp_right;
        }
    }
    }

    return root;
}

tnode *search(tnode *root, int data) {
    return splay(root, data);
}

tnode *insertSplay(tnode *root, int data) {
    root = insert(root, data);
    root = splay(root, data);
    
    return root;
}

tnode *root = NULL;


tnode *deleteSplay(tnode *root, int data) {
    if (root == NULL) {
        return NULL;
    }

    root = splay(root, data);

    if (root->data != data) {
        return root;
    }

    tnode *temp_left = root->left;
    tnode *temp_right = root->right;
    
    free(root);

    if (temp_left == NULL) {
        return temp_right;
    }
    else if (temp_right == NULL) {
        return temp_left;
    }
    else {
        tnode *minimum = findMin(temp_right);
        minimum->left = temp_left;
        return temp_right;
    }
}

int main() {
    root = insertSplay(root, 10);
    root = insertSplay(root, 4);
    root = insertSplay(root, 5);
    root = insertSplay(root, 2);
    root = insertSplay(root, 1);
    root = insertSplay(root, 1);

    inOrder(root);
}