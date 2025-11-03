#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // For true/false C99 standard
#include <string.h>   // For strlen

// The problem states we only use lowercase English letters
#define ALPHABET_SIZE 26

/**
 * @brief Defines the structure for a single trie node.
 */
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord; // Marks the end of an inserted word
} TrieNode;

/**
 * @brief Initializes a new trie node (Trie constructor).
 * This function acts as the `Trie()` initializer.
 * @return A pointer to the newly created root node.
 */
TrieNode* trieCreate() {
    // Allocate memory for the new node
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));

    if (node) {
        node->isEndOfWord = false;
        // Initialize all children pointers to NULL
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

/**
 * @brief Inserts a string word into the trie.
 * @param root The root node of the trie.
 * @param word The word to insert.
 */
void trieInsert(TrieNode* root, const char* word) {
    TrieNode* pCrawl = root;

    for (int i = 0; word[i] != '\0'; i++) {
        // Calculate the index (0-25) for the character
        int index = word[i] - 'a';

        // If the path doesn't exist, create a new node
        if (pCrawl->children[index] == NULL) {
            pCrawl->children[index] = trieCreate();
        }

        // Move to the next node
        pCrawl = pCrawl->children[index];
    }

    // Mark the last node as the end of a word
    pCrawl->isEndOfWord = true;
}

/**
 * @brief Returns true if the string word is in the trie.
 * @param root The root node of the trie.
 * @param word The word to search for.
 * @return true if the word exists, false otherwise.
 */
bool trieSearch(TrieNode* root, const char* word) {
    TrieNode* pCrawl = root;

    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';

        // If the path breaks, the word doesn't exist
        if (pCrawl->children[index] == NULL) {
            return false;
        }

        pCrawl = pCrawl->children[index];
    }

    // The path exists, but we must check if it's marked as a word.
    // This is what differentiates "app" from "apple"
    return (pCrawl != NULL && pCrawl->isEndOfWord);
}

/**
 * @brief Returns true if there is a previously inserted string word
 * that has the prefix.
 * @param root The root node of the trie.
 * @param prefix The prefix to check for.
 * @return true if the prefix exists, false otherwise.
 */
bool trieStartsWith(TrieNode* root, const char* prefix) {
    TrieNode* pCrawl = root;

    for (int i = 0; i < prefix[i] != '\0'; i++) {
        int index = prefix[i] - 'a';

        // If the path breaks, the prefix doesn't exist
        if (pCrawl->children[index] == NULL) {
            return false;
        }

        pCrawl = pCrawl->children[index];
    }

    // If we finished the loop, the prefix path exists.
    // We don't care if it's an end-of-word node or not.
    return (pCrawl != NULL);
}

/**
 * @brief Frees the entire trie to prevent memory leaks.
 * This is a good practice function, though not required by the prompt.
 * @param root The root node to start freeing from.
 */
void trieFree(TrieNode* root) {
    if (root == NULL) {
        return;
    }
    
    // Recursively free all children
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        trieFree(root->children[i]);
    }
    
    // Free the node itself
    free(root);
}

/**
 * @brief Helper function to print bool as "true" or "false".
 */
const char* boolToStr(bool b) {
    return b ? "true" : "false";
}

/**
 * @brief Main function to run the example from the problem.
 */
int main() {
    printf("--- Trie Example 1 --- \n");

    // Trie trie = new Trie();
    TrieNode* trie = trieCreate();
    printf("Trie() -> [null]\n");

    // trie.insert("apple");
    trieInsert(trie, "apple");
    printf("insert(\"apple\") -> [null]\n");

    // trie.search("apple");
    printf("search(\"apple\") -> [%s]\n", boolToStr(trieSearch(trie, "apple"))); // return True

    // trie.search("app");
    printf("search(\"app\") -> [%s]\n", boolToStr(trieSearch(trie, "app"))); // return False

    // trie.startsWith("app");
    printf("startsWith(\"app\") -> [%s]\n", boolToStr(trieStartsWith(trie, "app"))); // return True

    // trie.insert("app");
    trieInsert(trie, "app");
    printf("insert(\"app\") -> [null]\n");

    // trie.search("app");
    printf("search(\"app\") -> [%s]\n", boolToStr(trieSearch(trie, "app"))); // return True

    // Clean up memory
    trieFree(trie);
    printf("----------------------\n");

    return 0;
}