#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset

// Structure for the Segment Tree
// We can also just use a simple int* array,
// but a struct is cleaner.
typedef struct {
    int* tree; // Pointer to the array storing the tree nodes
    int n;      // Size of the original input array
} SegmentTree;

/**
 * @brief Recursive function to build the Segment Tree.
 * @param st Pointer to the SegmentTree
 * @param arr The input array
 * @param node Index of the current node in the 'tree' array
 * @param start The starting index of the segment for this node
 * @param end The ending index of the segment for this node
 */
void buildRecursive(SegmentTree* st, int arr[], int node, int start, int end) {
    if (start == end) {
        // Leaf node: represents a single element
        st->tree[node] = arr[start];
    } else {
        int mid = (start + end) / 2;
        int leftChild = 2 * node + 1;
        int rightChild = 2 * node + 2;

        // Recursively build left and right children
        buildRecursive(st, arr, leftChild, start, mid);
        buildRecursive(st, arr, rightChild, mid + 1, end);

        // Internal node: store the sum of its children
        st->tree[node] = st->tree[leftChild] + st->tree[rightChild];
    }
}

/**
 * @brief Allocates memory and builds the Segment Tree.
 * @param arr The input array
 * @param n Size of the input array
 * @return A new, initialized SegmentTree
 */
SegmentTree createSegmentTree(int arr[], int n) {
    SegmentTree st;
    st.n = n;

    // A Segment Tree for an array of size n
    // can have at most 4n nodes.
    st.tree = (int*)malloc(sizeof(int) * 4 * n);
    if (st.tree == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    // Initialize tree with 0 (optional, but good practice)
    memset(st.tree, 0, sizeof(int) * 4 * n);

    // Build the tree
    buildRecursive(&st, arr, 0, 0, n - 1);
    return st;
}

/**
 * @brief Recursive function for range sum query.
 * @param st Pointer to the SegmentTree
 * @param node Index of the current node
 * @param start Start index of the current node's segment
 * @param end End index of the current node's segment
 * @param L Left boundary of the query range
 * @param R Right boundary of the query range
 * @return The sum of the elements in the range [L, R]
 */
int queryRecursive(SegmentTree* st, int node, int start, int end, int L, int R) {
    // Case 1: No overlap
    // [start, end] is completely outside [L, R]
    if (start > R || end < L) {
        return 0; // Identity element for sum
    }

    // Case 2: Total overlap
    // [start, end] is completely inside [L, R]
    if (L <= start && end <= R) {
        return st->tree[node];
    }

    // Case 3: Partial overlap
    // Recursively query left and right children
    int mid = (start + end) / 2;
    int leftChild = 2 * node + 1;
    int rightChild = 2 * node + 2;

    int leftSum = queryRecursive(st, leftChild, start, mid, L, R);
    int rightSum = queryRecursive(st, rightChild, mid + 1, end, L, R);

    return leftSum + rightSum;
}

/**
 * @brief Public function to query a range sum.
 */
int query(SegmentTree* st, int L, int R) {
    if (L < 0 || R > st->n - 1 || L > R) {
        fprintf(stderr, "Invalid query range\n");
        return -1; // Or some other error
    }
    return queryRecursive(st, 0, 0, st->n - 1, L, R);
}

/**
 * @brief Recursive function for point update.
 * @param st Pointer to the SegmentTree
 * @param node Index of the current node
 * @param start Start index of the current node's segment
 * @param end End index of the current node's segment
 * @param idx The index in the original array to update
 * @param newValue The new value for the element
 */
void updateRecursive(SegmentTree* st, int node, int start, int end, int idx, int newValue) {
    if (start == end) {
        // Leaf node: update the value
        st->tree[node] = newValue;
    } else {
        int mid = (start + end) / 2;
        int leftChild = 2 * node + 1;
        int rightChild = 2 * node + 2;

        if (idx <= mid) {
            // Update is in the left child
            updateRecursive(st, leftChild, start, mid, idx, newValue);
        } else {
            // Update is in the right child
            updateRecursive(st, rightChild, mid + 1, end, idx, newValue);
        }

        // Re-calculate the sum for the current internal node
        st->tree[node] = st->tree[leftChild] + st->tree[rightChild];
    }
}

/**
 * @brief Public function to update a value.
 */
void update(SegmentTree* st, int idx, int newValue) {
    if (idx < 0 || idx > st->n - 1) {
        fprintf(stderr, "Invalid update index\n");
        return;
    }
    updateRecursive(st, 0, 0, st->n - 1, idx, newValue);
}

/**
 * @brief Frees all allocated memory for the Segment Tree.
 */
void freeSegmentTree(SegmentTree* st) {
    free(st->tree);
    st->tree = NULL;
    st->n = 0;
}

// --- Example Usage ---
int main() {
    int arr[] = {1, 3, 5, 7, 9, 11};
    int n = sizeof(arr) / sizeof(arr[0]);

    // Build the tree
    SegmentTree st = createSegmentTree(arr, n);

    // Query: Sum from index 1 to 3 (3 + 5 + 7)
    printf("Sum of range [1, 3] is: %d\n", query(&st, 1, 3)); // Output: 15

    // Update: Change arr[2] (value 5) to 6
    update(&st, 2, 6);
    
    // Original array is now logically {1, 3, 6, 7, 9, 11}

    // Query again: Sum from index 1 to 3 (3 + 6 + 7)
    printf("Sum of range [1, 3] after update: %d\n", query(&st, 1, 3)); // Output: 16
    
    // Query: Full range (1 + 3 + 6 + 7 + 9 + 11)
    printf("Sum of range [0, 5] after update: %d\n", query(&st, 0, 5)); // Output: 37

    // Clean up
    freeSegmentTree(&st);

    return 0;
}