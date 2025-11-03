#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Assuming ASCII (256 characters)
#define ALPHABET_SIZE 256

// Suffix Trie Node
typedef struct SuffixTrieNode {
    struct SuffixTrieNode* children[ALPHABET_SIZE];
} SuffixTrieNode;

/**
 * @brief Creates and initializes a new SuffixTrieNode.
 * @return Pointer to the new node.
 */
SuffixTrieNode* createSuffixTrieNode() {
    SuffixTrieNode* node = (SuffixTrieNode*)malloc(sizeof(SuffixTrieNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    // Initialize all children to NULL
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

/**
 * @brief Inserts a single suffix into the trie.
 * @param root The root of the Suffix Trie
 * @param suffix The suffix string to insert
 */
void insertSuffix(SuffixTrieNode* root, const char* suffix) {
    SuffixTrieNode* currentNode = root;
    for (int i = 0; suffix[i] != '\0'; i++) {
        // Use unsigned char for correct indexing
        unsigned char index = (unsigned char)suffix[i];
        
        if (currentNode->children[index] == NULL) {
            // Create a new node if path doesn't exist
            currentNode->children[index] = createSuffixTrieNode();
        }
        // Move to the next node
        currentNode = currentNode->children[index];
    }
}

/**
 * @brief Builds the complete Suffix Trie for a given text.
 * @param text The input text
 * @return The root of the newly built Suffix Trie
 */
SuffixTrieNode* buildSuffixTrie(const char* text) {
    SuffixTrieNode* root = createSuffixTrieNode();
    int n = strlen(text);

    // Insert all suffixes one by one
    for (int i = 0; i < n; i++) {
        // &text[i] is a pointer to the i-th suffix
        insertSuffix(root, &text[i]);
    }
    return root;
}

/**
 * @brief Searches for a pattern in the Suffix Trie.
 * @param root The root of the Suffix Trie
 * @param pattern The pattern to search for
 * @return 1 (true) if pattern is found, 0 (false) otherwise
 */
int search(SuffixTrieNode* root, const char* pattern) {
    SuffixTrieNode* currentNode = root;
    for (int i = 0; pattern[i] != '\0'; i++) {
        unsigned char index = (unsigned char)pattern[i];
        
        if (currentNode->children[index] == NULL) {
            // Path does not exist, pattern not found
            return 0; // false
        }
        currentNode = currentNode->children[index];
    }
    // If we reached the end of the pattern, it exists
    // as a prefix of some suffix (i.e., it's a substring)
    return 1; // true
}

/**
 * @brief Recursively frees all nodes in the trie.
 * @param node The current node to free (starts with root)
 */
void freeSuffixTrie(SuffixTrieNode* node) {
    if (node == NULL) {
        return;
    }
    // Free all children first (post-order traversal)
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeSuffixTrie(node->children[i]);
    }
    // Free the node itself
    free(node);
}

// --- Example Usage ---
int main() {
    const char* text = "banana$"; // Using '$' as a unique terminator is common
    SuffixTrieNode* root = buildSuffixTrie(text);

    // Search for substrings
    printf("Searching for 'ana': %s\n", search(root, "ana") ? "Found" : "Not Found");
    printf("Searching for 'nana': %s\n", search(root, "nana") ? "Found" : "Not Found");
    printf("Searching for 'ban': %s\n", search(root, "ban") ? "Found" : "Not Found");
    printf("Searching for 'apple': %s\n", search(root, "apple") ? "Found" : "Not Found");
    printf("Searching for '$': %s\n", search(root, "$") ? "Found" : "Not Found");

    // Clean up
    freeSuffixTrie(root);

    return 0;
}