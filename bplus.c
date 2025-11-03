/*
 * bplustree.c
 * A C implementation of a B+ Tree.
 *
 * This implementation includes:
 * - createBPlusTree
 * - insert
 * - search
 * - Printing the leaf list
 *
 * Deletion is omitted for brevity due to its high complexity.
 * We use (void*)(intptr_t) to store integer values as void pointers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h> // For intptr_t

// Define the order 't' (max number of children/pointers)
// Max keys = t - 1
// Min keys = ceil(t/2) - 1
#define ORDER 4

typedef struct BPlusTreeNode {
    bool isLeaf;
    int *keys;          // Array of keys (size ORDER-1)
    void **pointers;    // Array of pointers (size ORDER)
    int numKeys;
    struct BPlusTreeNode *parent;
    struct BPlusTreeNode *next; // For leaf nodes only
} BPlusTreeNode;

typedef struct BPlusTree {
    BPlusTreeNode *root;
    int order;
} BPlusTree;

// Function to create a new B+ Tree node
BPlusTreeNode* createNode(int order, bool isLeaf) {
    BPlusTreeNode *node = (BPlusTreeNode*)malloc(sizeof(BPlusTreeNode));
    if (!node) {
        perror("malloc failed for node");
        exit(1);
    }
    
    node->isLeaf = isLeaf;
    node->numKeys = 0;
    node->parent = NULL;
    node->next = NULL;

    // Max keys = order - 1
    node->keys = (int*)malloc(sizeof(int) * (order - 1));
    // Max pointers = order
    node->pointers = (void**)malloc(sizeof(void*) * order);
    
    if (!node->keys || !node->pointers) {
        perror("malloc failed for keys/pointers");
        exit(1);
    }

    // Initialize pointers to NULL
    for(int i = 0; i < order; i++) {
        node->pointers[i] = NULL;
    }

    return node;
}

// Function to create an empty B+ Tree
BPlusTree* createBPlusTree(int order) {
    BPlusTree *tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    if (!tree) {
        perror("malloc failed for tree");
        exit(1);
    }
    
    tree->order = order;
    tree->root = createNode(order, true); // Root is initially a leaf
    return tree;
}

// Helper function to find the leaf node for a given key
BPlusTreeNode* findLeaf(BPlusTreeNode *node, int key) {
    if (node->isLeaf) {
        return node;
    }

    // Find the child to descend into
    int i = 0;
    while (i < node->numKeys && key >= node->keys[i]) {
        i++;
    }
    return findLeaf((BPlusTreeNode*)node->pointers[i], key);
}

// Search for a key 'k' and return its associated value (as an int)
// Returns -1 if not found
int search(BPlusTree *tree, int key) {
    BPlusTreeNode *leaf = findLeaf(tree->root, key);

    // Look for the key in the leaf
    for (int i = 0; i < leaf->numKeys; i++) {
        if (leaf->keys[i] == key) {
            // Value is at the same index in the pointers array
            return (int)(intptr_t)leaf->pointers[i];
        }
    }
    return -1; // Not found
}

// Forward declaration
void insertIntoParent(BPlusTree *tree, BPlusTreeNode *left, int key, BPlusTreeNode *right);

// Function to insert a key-value pair into a leaf node
void insertIntoLeaf(BPlusTree *tree, BPlusTreeNode *leaf, int key, int value) {
    int i = 0;
    // Find insertion point
    while (i < leaf->numKeys && leaf->keys[i] < key) {
        i++;
    }

    // Shift keys and values (pointers) to the right
    for (int j = leaf->numKeys; j > i; j--) {
        leaf->keys[j] = leaf->keys[j - 1];
        leaf->pointers[j] = leaf->pointers[j - 1];
    }

    // Insert new key and value
    leaf->keys[i] = key;
    leaf->pointers[i] = (void*)(intptr_t)value;
    leaf->numKeys++;

    // Check if the leaf needs to be split
    if (leaf->numKeys == tree->order) {
        // Node is over-full (max keys is order-1)
        BPlusTreeNode *newLeaf = createNode(tree->order, true);
        int splitPoint = (tree->order) / 2;

        // Move right-half keys and values to the new leaf
        newLeaf->numKeys = tree->order - splitPoint;
        for (int j = 0; j < newLeaf->numKeys; j++) {
            newLeaf->keys[j] = leaf->keys[j + splitPoint];
            newLeaf->pointers[j] = leaf->pointers[j + splitPoint];
        }

        // Update original leaf's key count
        leaf->numKeys = splitPoint;

        // Update the leaf linked list
        newLeaf->next = leaf->next;
        leaf->next = newLeaf;

        // Set parents
        newLeaf->parent = leaf->parent;
        
        // "Copy up" the first key of the new leaf to the parent
        int keyToPush = newLeaf->keys[0];
        insertIntoParent(tree, leaf, keyToPush, newLeaf);
    }
}

// Function to insert a new key and child pointer into an internal node
void insertIntoParent(BPlusTree *tree, BPlusTreeNode *left, int key, BPlusTreeNode *right) {
    BPlusTreeNode *parent = left->parent;

    if (parent == NULL) {
        // We split the root. Create a new root.
        BPlusTreeNode *newRoot = createNode(tree->order, false);
        newRoot->keys[0] = key;
        newRoot->pointers[0] = left;
        newRoot->pointers[1] = right;
        newRoot->numKeys = 1;
        
        left->parent = newRoot;
        right->parent = newRoot;
        tree->root = newRoot;
        return;
    }

    // Parent exists. Find spot for key and right child.
    int i = 0;
    while (i < parent->numKeys && parent->keys[i] < key) {
        i++;
    }

    // Shift keys to the right
    for (int j = parent->numKeys; j > i; j--) {
        parent->keys[j] = parent->keys[j - 1];
    }
    // Shift children (pointers) to the right
    for (int j = parent->numKeys + 1; j > i + 1; j--) {
        parent->pointers[j] = parent->pointers[j - 1];
    }

    // Insert the new key and child
    parent->keys[i] = key;
    parent->pointers[i + 1] = right;
    parent->numKeys++;

    // Check if the parent node is now over-full
    if (parent->numKeys == tree->order) {
        // Internal node split (max keys is order-1)
        BPlusTreeNode *newInternal = createNode(tree->order, false);
        
        // Middle key is "moved up", not copied
        int splitPoint = (tree->order - 1) / 2;
        int keyToPush = parent->keys[splitPoint];

        // Move keys to the right of splitPoint to new node
        newInternal->numKeys = (tree->order - 1) - (splitPoint + 1);
        for (int j = 0; j < newInternal->numKeys; j++) {
            newInternal->keys[j] = parent->keys[j + splitPoint + 1];
        }

        // Move children to the right of splitPoint to new node
        for (int j = 0; j <= newInternal->numKeys; j++) {
            newInternal->pointers[j] = parent->pointers[j + splitPoint + 1];
            // Update parent pointer of moved children
            ((BPlusTreeNode*)newInternal->pointers[j])->parent = newInternal;
        }

        // Update original node's key count
        parent->numKeys = splitPoint;
        
        // Set parent and recurse
        newInternal->parent = parent->parent;
        insertIntoParent(tree, parent, keyToPush, newInternal);
    }
}

// Main insertion function
void insert(BPlusTree *tree, int key, int value) {
    BPlusTreeNode *leaf = findLeaf(tree->root, key);
    insertIntoLeaf(tree, leaf, key, value);
}

// Utility function to print the leaf linked list
void printLeaves(BPlusTree *tree) {
    BPlusTreeNode *node = tree->root;
    // Find the first leaf
    while (!node->isLeaf) {
        node = (BPlusTreeNode*)node->pointers[0];
    }

    // Iterate through all leaves
    printf("Leaf nodes linked list:\n");
    while (node) {
        printf("[");
        for (int i = 0; i < node->numKeys; i++) {
            printf("%d(v%d)", node->keys[i], (int)(intptr_t)node->pointers[i]);
            if (i < node->numKeys - 1) printf(", ");
        }
        printf("] -> ");
        node = node->next;
    }
    printf("NULL\n");
}

// Main function to demonstrate B+ Tree operations
int main() {
    BPlusTree *t = createBPlusTree(ORDER);

    // Values are just 100 + key for demonstration
    int keys_to_insert[] = {10, 20, 30, 5, 15, 25, 7, 12, 17, 35, 40};
    int n = sizeof(keys_to_insert) / sizeof(keys_to_insert[0]);
    
    printf("Inserting keys into B+ Tree (order=%d):\n", ORDER);
    for (int i = 0; i < n; i++) {
        int key = keys_to_insert[i];
        int value = key + 100;
        printf("Inserting (%d, %d)\n", key, value);
        insert(t, key, value);
    }

    printf("\n--- B+ Tree Leaf List ---\n");
    printLeaves(t);

    printf("\n--- B+ Tree Search ---\n");
    int key_to_find = 17;
    int val = search(t, key_to_find);
    if (val != -1) {
        printf("Found key %d, value = %d\n", key_to_find, val);
    } else {
        printf("Key %d not found.\n", key_to_find);
    }

    key_to_find = 99;
    val = search(t, key_to_find);
    if (val != -1) {
        printf("Found key %d, value = %d\n", key_to_find, val);
    } else {
        printf("Key %d not found.\n", key_to_find);
    }

    // Note: Freeing the tree (post-order traversal free) is omitted.
    
    return 0;
}