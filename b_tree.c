/*
 * btree_complete.c
 * Complete B-Tree Implementation in C
 * Based on CLRS Algorithm with minimum degree 't'
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Minimum degree of the B-Tree
#define T 3

// B-Tree Node
typedef struct BTreeNode {
    int n;           // Current number of keys
    bool leaf;       // True if leaf node
    int *keys;       // Array of keys (size: 2*T - 1)
    struct BTreeNode **children; // Array of child pointers (size: 2*T)
} BTreeNode;

// B-Tree structure
typedef struct BTree {
    BTreeNode *root;
    int t;           // Minimum degree
} BTree;

// Function prototypes
BTree* createBTree(int t);
BTreeNode* createNode(bool leaf, int t);
void traverse(BTreeNode *node);
BTreeNode* search(BTreeNode *node, int key);
void insert(BTree *tree, int key);
void insertNonFull(BTreeNode *node, int key, int t);
void splitChild(BTreeNode *parent, int i, BTreeNode *child, int t);
void deleteKey(BTree *tree, int key);
void deleteFromNode(BTreeNode *node, int key, int t);
int findKey(BTreeNode *node, int key);
void removeFromLeaf(BTreeNode *node, int idx);
void removeFromNonLeaf(BTreeNode *node, int idx, int t);
int getPredecessor(BTreeNode *node, int idx);
int getSuccessor(BTreeNode *node, int idx);
void fill(BTreeNode *node, int idx, int t);
void borrowFromPrev(BTreeNode *node, int idx);
void borrowFromNext(BTreeNode *node, int idx);
void merge(BTreeNode *node, int idx, int t);
void freeBTree(BTreeNode *node);
void printTree(BTreeNode *node, int level);

// Create a new B-Tree
BTree* createBTree(int t) {
    BTree *tree = (BTree*)malloc(sizeof(BTree));
    if (!tree) {
        perror("Failed to create B-Tree");
        exit(EXIT_FAILURE);
    }
    tree->t = t;
    tree->root = createNode(true, t);
    return tree;
}

// Create a new node
BTreeNode* createNode(bool leaf, int t) {
    BTreeNode *node = (BTreeNode*)malloc(sizeof(BTreeNode));
    if (!node) {
        perror("Failed to create node");
        exit(EXIT_FAILURE);
    }
    
    node->leaf = leaf;
    node->n = 0;
    node->keys = (int*)malloc(sizeof(int) * (2 * t - 1));
    node->children = (BTreeNode**)malloc(sizeof(BTreeNode*) * (2 * t));
    
    if (!node->keys || !node->children) {
        perror("Failed to allocate node arrays");
        exit(EXIT_FAILURE);
    }
    
    // Initialize children to NULL
    for (int i = 0; i < (2 * t); i++) {
        node->children[i] = NULL;
    }
    
    return node;
}

// Search for a key in the tree
BTreeNode* search(BTreeNode *node, int key) {
    if (!node) return NULL;
    
    int i = 0;
    while (i < node->n && key > node->keys[i])
        i++;
    
    if (i < node->n && key == node->keys[i])
        return node;
    
    if (node->leaf)
        return NULL;
    
    return search(node->children[i], key);
}

// In-order traversal
void traverse(BTreeNode *node) {
    if (!node) return;
    
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            traverse(node->children[i]);
        printf("%d ", node->keys[i]);
    }
    
    if (!node->leaf)
        traverse(node->children[i]);
}

// Split child node
void splitChild(BTreeNode *parent, int idx, BTreeNode *child, int t) {
    BTreeNode *newChild = createNode(child->leaf, t);
    newChild->n = t - 1;
    
    // Copy the last t-1 keys from child to newChild
    for (int j = 0; j < t - 1; j++)
        newChild->keys[j] = child->keys[j + t];
    
    // Copy the last t children if not leaf
    if (!child->leaf) {
        for (int j = 0; j < t; j++)
            newChild->children[j] = child->children[j + t];
    }
    
    child->n = t - 1;
    
    // Make space for new child in parent
    for (int j = parent->n; j >= idx + 1; j--)
        parent->children[j + 1] = parent->children[j];
    
    parent->children[idx + 1] = newChild;
    
    // Make space for new key in parent
    for (int j = parent->n - 1; j >= idx; j--)
        parent->keys[j + 1] = parent->keys[j];
    
    // Move middle key from child to parent
    parent->keys[idx] = child->keys[t - 1];
    parent->n++;
}

// Insert into non-full node
void insertNonFull(BTreeNode *node, int key, int t) {
    int i = node->n - 1;
    
    if (node->leaf) {
        // Find position and insert in leaf
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->n++;
    } else {
        // Find child to descend into
        while (i >= 0 && key < node->keys[i])
            i--;
        i++;
        
        // Check if child is full
        if (node->children[i]->n == 2 * t - 1) {
            splitChild(node, i, node->children[i], t);
            if (key > node->keys[i])
                i++;
        }
        insertNonFull(node->children[i], key, t);
    }
}

// Main insert function
void insert(BTree *tree, int key) {
    BTreeNode *root = tree->root;
    int t = tree->t;
    
    if (root->n == 2 * t - 1) {
        // Root is full, need to split
        BTreeNode *newRoot = createNode(false, t);
        newRoot->children[0] = root;
        tree->root = newRoot;
        splitChild(newRoot, 0, root, t);
        insertNonFull(newRoot, key, t);
    } else {
        insertNonFull(root, key, t);
    }
}

// Find key in node
int findKey(BTreeNode *node, int key) {
    int idx = 0;
    while (idx < node->n && node->keys[idx] < key)
        idx++;
    return idx;
}

// Remove from leaf node
void removeFromLeaf(BTreeNode *node, int idx) {
    for (int i = idx + 1; i < node->n; i++)
        node->keys[i - 1] = node->keys[i];
    node->n--;
}

// Remove from non-leaf node
void removeFromNonLeaf(BTreeNode *node, int idx, int t) {
    int key = node->keys[idx];
    
    if (node->children[idx]->n >= t) {
        // Predecessor exists
        int pred = getPredecessor(node, idx);
        node->keys[idx] = pred;
        deleteFromNode(node->children[idx], pred, t);
    } else if (node->children[idx + 1]->n >= t) {
        // Successor exists
        int succ = getSuccessor(node, idx);
        node->keys[idx] = succ;
        deleteFromNode(node->children[idx + 1], succ, t);
    } else {
        // Merge children
        merge(node, idx, t);
        deleteFromNode(node->children[idx], key, t);
    }
}

// Get predecessor
int getPredecessor(BTreeNode *node, int idx) {
    BTreeNode *current = node->children[idx];
    while (!current->leaf)
        current = current->children[current->n];
    return current->keys[current->n - 1];
}

// Get successor
int getSuccessor(BTreeNode *node, int idx) {
    BTreeNode *current = node->children[idx + 1];
    while (!current->leaf)
        current = current->children[0];
    return current->keys[0];
}

// Fill underflowed child
void fill(BTreeNode *node, int idx, int t) {
    if (idx != 0 && node->children[idx - 1]->n >= t)
        borrowFromPrev(node, idx);
    else if (idx != node->n && node->children[idx + 1]->n >= t)
        borrowFromNext(node, idx);
    else {
        if (idx != node->n)
            merge(node, idx, t);
        else
            merge(node, idx - 1, t);
    }
}

// Borrow from previous child
void borrowFromPrev(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx - 1];
    
    // Shift all keys in child right
    for (int i = child->n - 1; i >= 0; i--)
        child->keys[i + 1] = child->keys[i];
    
    // Shift children if not leaf
    if (!child->leaf) {
        for (int i = child->n; i >= 0; i--)
            child->children[i + 1] = child->children[i];
    }
    
    // Move key from parent to child
    child->keys[0] = node->keys[idx - 1];
    
    // Move last child from sibling to child
    if (!child->leaf)
        child->children[0] = sibling->children[sibling->n];
    
    // Move key from sibling to parent
    node->keys[idx - 1] = sibling->keys[sibling->n - 1];
    
    child->n++;
    sibling->n--;
}

// Borrow from next child
void borrowFromNext(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];
    
    // Move key from parent to child
    child->keys[child->n] = node->keys[idx];
    
    // Move first child from sibling to child
    if (!child->leaf)
        child->children[child->n + 1] = sibling->children[0];
    
    // Move first key from sibling to parent
    node->keys[idx] = sibling->keys[0];
    
    // Shift all keys in sibling left
    for (int i = 1; i < sibling->n; i++)
        sibling->keys[i - 1] = sibling->keys[i];
    
    // Shift children if not leaf
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; i++)
            sibling->children[i - 1] = sibling->children[i];
    }
    
    child->n++;
    sibling->n--;
}

// Merge two children
void merge(BTreeNode *node, int idx, int t) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];
    
    // Move key from parent to child
    child->keys[t - 1] = node->keys[idx];
    
    // Copy keys from sibling to child
    for (int i = 0; i < sibling->n; i++)
        child->keys[i + t] = sibling->keys[i];
    
    // Copy children from sibling to child
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; i++)
            child->children[i + t] = sibling->children[i];
    }
    
    // Fill gap in parent
    for (int i = idx + 1; i < node->n; i++)
        node->keys[i - 1] = node->keys[i];
    
    for (int i = idx + 2; i <= node->n; i++)
        node->children[i - 1] = node->children[i];
    
    child->n += sibling->n + 1;
    node->n--;
    
    // Free sibling
    free(sibling->keys);
    free(sibling->children);
    free(sibling);
}

// Delete from node
void deleteFromNode(BTreeNode *node, int key, int t) {
    int idx = findKey(node, key);
    
    if (idx < node->n && node->keys[idx] == key) {
        // Key found in this node
        if (node->leaf)
            removeFromLeaf(node, idx);
        else
            removeFromNonLeaf(node, idx, t);
    } else {
        if (node->leaf) {
            printf("Key %d not found in the tree\n", key);
            return;
        }
        
        bool isLastChild = (idx == node->n);
        
        if (node->children[idx]->n < t)
            fill(node, idx, t);
        
        if (isLastChild && idx > node->n)
            deleteFromNode(node->children[idx - 1], key, t);
        else
            deleteFromNode(node->children[idx], key, t);
    }
}

// Main delete function
void deleteKey(BTree *tree, int key) {
    if (!tree || !tree->root) {
        printf("Tree is empty\n");
        return;
    }
    
    deleteFromNode(tree->root, key, tree->t);
    
    // If root becomes empty after deletion
    if (tree->root->n == 0) {
        BTreeNode *oldRoot = tree->root;
        if (tree->root->leaf) {
            tree->root = createNode(true, tree->t);
        } else {
            tree->root = tree->root->children[0];
        }
        free(oldRoot->keys);
        free(oldRoot->children);
        free(oldRoot);
    }
}

// Print tree in a structured format
void printTree(BTreeNode *node, int level) {
    if (!node) return;
    
    printf("Level %d: ", level);
    for (int i = 0; i < node->n; i++)
        printf("%d ", node->keys[i]);
    printf("\n");
    
    if (!node->leaf) {
        for (int i = 0; i <= node->n; i++)
            printTree(node->children[i], level + 1);
    }
}

// Free entire B-Tree
void freeBTree(BTreeNode *node) {
    if (!node) return;
    
    if (!node->leaf) {
        for (int i = 0; i <= node->n; i++)
            freeBTree(node->children[i]);
    }
    
    free(node->keys);
    free(node->children);
    free(node);
}

int main() {
    printf("=== Complete B-Tree Implementation (t=%d) ===\n\n", T);
    
    // Create B-Tree
    BTree *tree = createBTree(T);
    printf("1. Created empty B-Tree with minimum degree %d\n", T);
    
    // Test 1: Insertion
    printf("\n2. Testing Insertion:\n");
    int insert_keys[] = {10, 20, 5, 6, 12, 30, 7, 17, 3, 1, 8, 25, 15, 18, 22};
    int insert_count = sizeof(insert_keys) / sizeof(insert_keys[0]);
    
    printf("   Inserting keys: ");
    for (int i = 0; i < insert_count; i++) {
        printf("%d ", insert_keys[i]);
        insert(tree, insert_keys[i]);
    }
    printf("\n");
    
    printf("   In-order traversal after insertion: ");
    traverse(tree->root);
    printf("\n");
    
    // Test 2: Search
    printf("\n3. Testing Search:\n");
    int search_keys[] = {6, 17, 99, 3, 25};
    int search_count = sizeof(search_keys) / sizeof(search_keys[0]);
    
    for (int i = 0; i < search_count; i++) {
        BTreeNode *result = search(tree->root, search_keys[i]);
        if (result)
            printf("   Key %d: FOUND\n", search_keys[i]);
        else
            printf("   Key %d: NOT FOUND\n", search_keys[i]);
    }
    
    // Test 3: Tree Structure
    printf("\n4. Tree Structure:\n");
    printTree(tree->root, 0);
    
    // Test 4: Deletion from leaf
    printf("\n5. Testing Deletion (Leaf Nodes):\n");
    int delete_leaf[] = {1, 8, 22};
    int delete_leaf_count = sizeof(delete_leaf) / sizeof(delete_leaf[0]);
    
    for (int i = 0; i < delete_leaf_count; i++) {
        printf("   Deleting %d: ", delete_leaf[i]);
        deleteKey(tree, delete_leaf[i]);
        printf("Traversal: ");
        traverse(tree->root);
        printf("\n");
    }
    
    // Test 5: Deletion from internal nodes
    printf("\n6. Testing Deletion (Internal Nodes):\n");
    int delete_internal[] = {7, 17, 10};
    int delete_internal_count = sizeof(delete_internal) / sizeof(delete_internal[0]);
    
    for (int i = 0; i < delete_internal_count; i++) {
        printf("   Deleting %d: ", delete_internal[i]);
        deleteKey(tree, delete_internal[i]);
        printf("Traversal: ");
        traverse(tree->root);
        printf("\n");
    }
    
    // Test 6: Final tree structure
    printf("\n7. Final Tree Structure:\n");
    printTree(tree->root, 0);
    
    // Test 7: Search after deletions
    printf("\n8. Search After Deletions:\n");
    int final_search[] = {5, 10, 15, 20, 25, 30};
    int final_search_count = sizeof(final_search) / sizeof(final_search[0]);
    
    for (int i = 0; i < final_search_count; i++) {
        BTreeNode *result = search(tree->root, final_search[i]);
        if (result)
            printf("   Key %d: FOUND\n", final_search[i]);
        else
            printf("   Key %d: NOT FOUND\n", final_search[i]);
    }
    
    // Test 8: Insert more keys after deletions
    printf("\n9. Inserting More Keys After Deletions:\n");
    int more_keys[] = {100, 2, 13, 27};
    int more_keys_count = sizeof(more_keys) / sizeof(more_keys[0]);
    
    printf("   Inserting: ");
    for (int i = 0; i < more_keys_count; i++) {
        printf("%d ", more_keys[i]);
        insert(tree, more_keys[i]);
    }
    printf("\n");
    
    printf("   Final traversal: ");
    traverse(tree->root);
    printf("\n");
    
    printf("   Final tree structure:\n");
    printTree(tree->root, 0);
    
    // Cleanup
    printf("\n10. Cleaning up memory...\n");
    freeBTree(tree->root);
    free(tree);
    
    printf("\n=== B-Tree Implementation Completed Successfully ===\n");
    
    return 0;
}