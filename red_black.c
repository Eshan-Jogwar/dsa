#include <stdio.h>
#include <stdlib.h>

// -----------------------------------------------------------------
// 1. Type Definitions and Structures
// -----------------------------------------------------------------

// Color enumeration
typedef enum { RED, BLACK } Color;

// Node structure
typedef struct Node {
    int key;
    Color color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
} Node;

// Red-Black Tree structure
typedef struct RedBlackTree {
    Node *root;
    Node *NIL; // Sentinel node
} RedBlackTree;

// -----------------------------------------------------------------
// 2. Function Prototypes
// -----------------------------------------------------------------

// --- Public Functions ---
RedBlackTree* createRedBlackTree();
void insert(RedBlackTree* tree, int key);
void deleteNode(RedBlackTree* tree, int key);
Node* search(RedBlackTree* tree, int key);
void inorder(RedBlackTree* tree);
void printTree(RedBlackTree* tree);
void freeRedBlackTree(RedBlackTree* tree);

// --- Internal Helper Functions ---
Node* createNode(RedBlackTree* tree, int key);
void leftRotate(RedBlackTree* tree, Node* x);
void rightRotate(RedBlackTree* tree, Node* y);
void insertFixup(RedBlackTree* tree, Node* z);
void transplant(RedBlackTree* tree, Node* u, Node* v);
Node* minimum(RedBlackTree* tree, Node* node);
void deleteFixup(RedBlackTree* tree, Node* x);
void inorderHelper(RedBlackTree* tree, Node* node);
void printTreeHelper(RedBlackTree* tree, Node* root, int space);
void freeTreeHelper(RedBlackTree* tree, Node* node);


// -----------------------------------------------------------------
// 3. Main Function (Driver Code)
// -----------------------------------------------------------------

int main() {
    // 1. Create a new Red-Black Tree
    RedBlackTree* rbt = createRedBlackTree();

    // 2. Insert nodes
    int keys_to_insert[] = {10, 20, 30, 15, 25, 5, 1};
    int num_keys = sizeof(keys_to_insert) / sizeof(keys_to_insert[0]);

    printf("Inserting keys: ");
    for (int i = 0; i < num_keys; i++) {
        printf("%d ", keys_to_insert[i]);
        insert(rbt, keys_to_insert[i]);
    }
    printf("\n\n");

    // 3. Print the tree
    printTree(rbt);
    inorder(rbt);

    // 4. Search for a node
    printf("\n=====================================\n");
    int key_to_search = 15;
    Node* found_node = search(rbt, key_to_search);
    if (found_node != rbt->NIL) {
        printf("Search: Found node %d (Color: %s)\n", 
               found_node->key, (found_node->color == RED ? "RED" : "BLACK"));
    } else {
        printf("Search: Node %d not found\n", key_to_search);
    }

    // 5. Delete nodes
    printf("=====================================\n");
    int keys_to_delete[] = {1, 30, 10};
    num_keys = sizeof(keys_to_delete) / sizeof(keys_to_delete[0]);

    for (int i = 0; i < num_keys; i++) {
        printf("\nDeleting: %d\n", keys_to_delete[i]);
        deleteNode(rbt, keys_to_delete[i]);
        printTree(rbt);
        inorder(rbt);
    }

    // 6. Search for a deleted node
    printf("\n=====================================\n");
    key_to_search = 10;
    found_node = search(rbt, key_to_search);
    if (found_node != rbt->NIL) {
        printf("Search: Found node %d (Color: %s)\n", 
               found_node->key, (found_node->color == RED ? "RED" : "BLACK"));
    } else {
        printf("Search: Node %d not found\n", key_to_search);
    }

    // 7. Clean up memory
    freeRedBlackTree(rbt);
    printf("\nTree memory freed.\n");

    return 0;
}

// -----------------------------------------------------------------
// 4. Function Implementations
// -----------------------------------------------------------------

// --- Create ---

/**
 * @brief Creates and initializes a new Red-Black Tree.
 * Sets up the NIL sentinel node.
 */
RedBlackTree* createRedBlackTree() {
    RedBlackTree* tree = (RedBlackTree*)malloc(sizeof(RedBlackTree));
    if (tree == NULL) {
        fprintf(stderr, "Failed to allocate memory for tree\n");
        exit(EXIT_FAILURE);
    }

    // Create the sentinel NIL node
    tree->NIL = (Node*)malloc(sizeof(Node));
    if (tree->NIL == NULL) {
        fprintf(stderr, "Failed to allocate memory for NIL node\n");
        free(tree);
        exit(EXIT_FAILURE);
    }
    
    tree->NIL->color = BLACK;
    tree->NIL->key = 0; // Key doesn't matter for NIL
    tree->NIL->left = NULL; // Not strictly necessary but good practice
    tree->NIL->right = NULL;
    tree->NIL->parent = NULL;

    // Root initially points to NIL (empty tree)
    tree->root = tree->NIL;

    return tree;
}

/**
 * @brief Creates a new node with the given key.
 * Color is initialized to RED.
 */
Node* createNode(RedBlackTree* tree, int key) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Failed to allocate memory for new node\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->color = RED; // New nodes are always RED
    node->parent = tree->NIL;
    node->left = tree->NIL;
    node->right = tree->NIL;
    return node;
}

// --- Rotations ---

/**
 * @brief Performs a left rotation on node x.
 */
void leftRotate(RedBlackTree* tree, Node* x) {
    Node* y = x->right;
    x->right = y->left;

    if (y->left != tree->NIL) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == tree->NIL) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

/**
 * @brief Performs a right rotation on node y.
 */
void rightRotate(RedBlackTree* tree, Node* y) {
    Node* x = y->left;
    y->left = x->right;

    if (x->right != tree->NIL) {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == tree->NIL) {
        tree->root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

// --- Insert ---

/**
 * @brief Inserts a new node with the given key into the tree.
 */
void insert(RedBlackTree* tree, int key) {
    Node* z = createNode(tree, key);
    Node* y = tree->NIL;
    Node* x = tree->root;

    // 1. Standard BST Insert
    while (x != tree->NIL) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;
    if (y == tree->NIL) {
        tree->root = z; // Tree was empty
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    // 2. Fix Red-Black properties
    insertFixup(tree, z);
}

/**
 * @brief Restores Red-Black properties after insertion.
 */
void insertFixup(RedBlackTree* tree, Node* z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) { // Parent is left child
            Node* y = z->parent->parent->right; // Uncle
            
            // Case 1: Uncle is RED
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                // Case 2: Uncle is BLACK, z is a right child (Triangle)
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(tree, z);
                }
                // Case 3: Uncle is BLACK, z is a left child (Line)
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(tree, z->parent->parent);
            }
        } else { // Parent is right child (symmetric)
            Node* y = z->parent->parent->left; // Uncle
            
            // Case 1: Uncle is RED
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                // Case 2: Uncle is BLACK, z is a left child (Triangle)
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(tree, z);
                }
                // Case 3: Uncle is BLACK, z is a right child (Line)
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(tree, z->parent->parent);
            }
        }
    }
    // Ensure root is always BLACK
    tree->root->color = BLACK;
}

// --- Delete ---

/**
 * @brief Replaces the subtree rooted at u with the subtree at v.
 */
void transplant(RedBlackTree* tree, Node* u, Node* v) {
    if (u->parent == tree->NIL) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

/**
 * @brief Finds the node with the minimum key in a subtree.
 */
Node* minimum(RedBlackTree* tree, Node* node) {
    while (node->left != tree->NIL) {
        node = node->left;
    }
    return node;
}

/**
 * @brief Deletes the node with the given key.
 */
void deleteNode(RedBlackTree* tree, int key) {
    Node* z = search(tree, key);
    if (z == tree->NIL) {
        printf("Node with key %d not found.\n", key);
        return;
    }

    Node* y = z;
    Node* x;
    Color y_original_color = y->color;

    // 1. Standard BST Delete Logic
    if (z->left == tree->NIL) {
        x = z->right;
        transplant(tree, z, z->right);
    } else if (z->right == tree->NIL) {
        x = z->left;
        transplant(tree, z, z->left);
    } else {
        // Node z has two children
        y = minimum(tree, z->right); // y is z's successor
        y_original_color = y->color;
        x = y->right; // x will replace y
        
        if (y->parent == z) {
            x->parent = y; // Handle case where x is NIL
        } else {
            transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        
        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color; // y takes z's original color
    }
    
    free(z); // Free the deleted node

    // 2. Fix Red-Black properties if a BLACK node was removed
    if (y_original_color == BLACK) {
        deleteFixup(tree, x);
    }
}

/**
 * @brief Restores Red-Black properties after deletion.
 */
void deleteFixup(RedBlackTree* tree, Node* x) {
    while (x != tree->root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node* w = x->parent->right; // Sibling

            // Case 1: Sibling w is RED
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(tree, x->parent);
                w = x->parent->right;
            }

            // Case 2: Sibling w is BLACK, both w's children are BLACK
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                // Case 3: Sibling w is BLACK, w.left is RED, w.right is BLACK
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(tree, w);
                    w = x->parent->right;
                }
                
                // Case 4: Sibling w is BLACK, w.right is RED
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                leftRotate(tree, x->parent);
                x = tree->root; // Fixup complete
            }
        } else { // Symmetric cases: x is a right child
            Node* w = x->parent->left; // Sibling
            
            // Case 1: Sibling w is RED
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(tree, x->parent);
                w = x->parent->left;
            }

            // Case 2: Sibling w is BLACK, both w's children are BLACK
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                // Case 3: Sibling w is BLACK, w.left is BLACK, w.right is RED
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(tree, w);
                    w = x->parent->left;
                }
                
                // Case 4: Sibling w is BLACK, w.left is RED
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(tree, x->parent);
                x = tree->root; // Fixup complete
            }
        }
    }
    x->color = BLACK; // Ensure root or final node is BLACK
}

// --- Search ---

/**
 * @brief Searches for a node with the given key.
 * Returns the node if found, otherwise returns tree->NIL.
 */
Node* search(RedBlackTree* tree, int key) {
    Node* current = tree->root;
    while (current != tree->NIL && key != current->key) {
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return current;
}

// --- Print ---

/**
 * @brief Public function to print the tree inorder.
 */
void inorder(RedBlackTree* tree) {
    printf("Inorder Traversal: ");
    inorderHelper(tree, tree->root);
    printf("\n");
}

/**
 * @brief Recursive helper for inorder traversal.
 */
void inorderHelper(RedBlackTree* tree, Node* node) {
    if (node != tree->NIL) {
        inorderHelper(tree, node->left);
        printf("%d(%c) ", node->key, (node->color == RED ? 'R' : 'B'));
        inorderHelper(tree, node->right);
    }
}

#define COUNT 10

/**
 * @brief Recursive helper to print the tree structure.
 */
void printTreeHelper(RedBlackTree* tree, Node* root, int space) {
    if (root == tree->NIL)
        return;

    space += COUNT;

    printTreeHelper(tree, root->right, space);

    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%d(%c)\n", root->key, (root->color == RED ? 'R' : 'B'));

    printTreeHelper(tree, root->left, space);
}

/**
 * @brief Public function to print a 2D representation of the tree.
 */
void printTree(RedBlackTree* tree) {
    printf("Tree Structure:\n");
    printTreeHelper(tree, tree->root, 0);
    printf("\n-------------------------------------\n");
}


// --- Free (Cleanup) ---

/**
 * @brief Public function to free all memory used by the tree.
 */
void freeRedBlackTree(RedBlackTree* tree) {
    if (tree == NULL) return;
    freeTreeHelper(tree, tree->root);
    free(tree->NIL); // Free the sentinel
    free(tree);     // Free the tree struct itself
}

/**
 * @brief Recursive helper to free all nodes.
 */
void freeTreeHelper(RedBlackTree* tree, Node* node) {
    if (node != tree->NIL) {
        freeTreeHelper(tree, node->left);
        freeTreeHelper(tree, node->right);
        free(node);
    }
}