#include <stdio.h>
#include <stdlib.h>

typedef struct AVLNode {
    int key;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

// Create a new node
AVLNode* createNode(int key) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

// Get height of node
int height(AVLNode* node) {
    if (node == NULL) return 0;
    return node->height;
}

// Get balance factor
int getBalance(AVLNode* node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

// Update height of node
void updateHeight(AVLNode* node) {
    if (node != NULL) {
        int leftHeight = height(node->left);
        int rightHeight = height(node->right);
        node->height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    }
}

// Right rotation
AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    updateHeight(y);
    updateHeight(x);

    return x;
}

// Left rotation
AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    updateHeight(x);
    updateHeight(y);

    return y;
}

// Find node with minimum value
AVLNode* minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

// Search for a key
AVLNode* search(AVLNode* root, int key) {
    if (root == NULL || root->key == key)
        return root;
    
    if (key < root->key)
        return search(root->left, key);
    else
        return search(root->right, key);
}

// Insert node and balance the tree
AVLNode* insert(AVLNode* node, int key) {
    // 1. Perform normal BST insertion
    if (node == NULL) return createNode(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else // Duplicate keys not allowed
        return node;

    // 2. Update height of current node
    updateHeight(node);

    // 3. Get balance factor to check if unbalanced
    int balance = getBalance(node);

    // 4. Perform rotations if unbalanced

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Delete node and balance the tree
AVLNode* deleteNode(AVLNode* root, int key) {
    // 1. Perform standard BST delete
    if (root == NULL) return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        // Node to be deleted found
        
        // Node with only one child or no child
        if (root->left == NULL) {
            AVLNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            AVLNode* temp = root->left;
            free(root);
            return temp;
        }

        // Node with two children: get inorder successor
        AVLNode* temp = minValueNode(root->right);
        
        // Copy the inorder successor's content to this node
        root->key = temp->key;
        
        // Delete the inorder successor
        root->right = deleteNode(root->right, temp->key);
    }

    // If tree had only one node then return
    if (root == NULL) return root;

    // 2. Update height of current node
    updateHeight(root);

    // 3. Get balance factor to check if unbalanced
    int balance = getBalance(root);

    // 4. Perform rotations if unbalanced

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Inorder traversal
void inorder(AVLNode* root) {
    if (root != NULL) {
        inorder(root->left);
        printf("%d ", root->key);
        inorder(root->right);
    }
}

// Preorder traversal
void preorder(AVLNode* root) {
    if (root != NULL) {
        printf("%d ", root->key);
        preorder(root->left);
        preorder(root->right);
    }
}

// Example usage
int main() {
    AVLNode* root = NULL;

    printf("Inserting nodes: 10, 20, 30, 40, 50, 25\n");
    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 30);  // Right Right case
    root = insert(root, 40);
    root = insert(root, 50);
    root = insert(root, 25);  // Right Left case

    printf("Inorder traversal: ");
    inorder(root);
    printf("\n");

    printf("Preorder traversal: ");
    preorder(root);
    printf("\n");

    // Search examples
    int searchKey = 30;
    AVLNode* found = search(root, searchKey);
    if (found != NULL)
        printf("Key %d found in the tree\n", searchKey);
    else
        printf("Key %d not found in the tree\n", searchKey);

    searchKey = 35;
    found = search(root, searchKey);
    if (found != NULL)
        printf("Key %d found in the tree\n", searchKey);
    else
        printf("Key %d not found in the tree\n", searchKey);

    printf("\nDeleting node 30\n");
    root = deleteNode(root, 30);
    printf("Inorder traversal after deletion: ");
    inorder(root);
    printf("\n");

    printf("Preorder traversal after deletion: ");
    preorder(root);
    printf("\n");

    printf("\nDeleting node 25\n");
    root = deleteNode(root, 25);
    printf("Inorder traversal after deletion: ");
    inorder(root);
    printf("\n");

    return 0;
}