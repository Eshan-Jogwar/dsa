#include <stdio.h>
#include <stdlib.h>

// Node structure for linked list
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Function to create a new node
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a node at the end of the list
void insertEnd(Node** head, int data) {
    Node* newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    
    Node* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

// Function to get the maximum value in the linked list
int getMaxList(Node* head) {
    int max = head->data;
    Node* current = head->next;
    
    while (current != NULL) {
        if (current->data > max) {
            max = current->data;
        }
        current = current->next;
    }
    return max;
}

// Function to get the digit at a specific place
int getDigit(int number, int exp) {
    return (number / exp) % 10;
}

// Radix Sort function for linked list
Node* radixSortLinkedList(Node* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }
    
    // Find the maximum number to know number of digits
    int max = getMaxList(head);
    
    // Create buckets for digits 0-9
    Node* buckets[10];
    Node* tails[10];
    
    // Initialize all buckets to NULL
    for (int i = 0; i < 10; i++) {
        buckets[i] = NULL;
        tails[i] = NULL;
    }
    
    // Process each digit place (units, tens, hundreds, etc.)
    for (int exp = 1; max / exp > 0; exp *= 10) {
        // Distribute nodes into buckets based on current digit
        Node* current = head;
        while (current != NULL) {
            int digit = getDigit(current->data, exp);
            
            if (buckets[digit] == NULL) {
                buckets[digit] = current;
                tails[digit] = current;
            } else {
                tails[digit]->next = current;
                tails[digit] = current;
            }
            
            current = current->next;
        }
        
        // Collect nodes from buckets and rebuild the list
        head = NULL;
        Node* tail = NULL;
        
        for (int i = 0; i < 10; i++) {
            if (buckets[i] != NULL) {
                if (head == NULL) {
                    head = buckets[i];
                } else {
                    tail->next = buckets[i];
                }
                tail = tails[i];
                
                // Reset buckets
                buckets[i] = NULL;
                tails[i] = NULL;
            }
        }
        
        // Terminate the list
        if (tail != NULL) {
            tail->next = NULL;
        }
    }
    
    return head;
}

// Utility function to print a linked list
void printList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

// Function to free the linked list
void freeList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
}

// Test function for linked list radix sort
void testLinkedListRadixSort() {
    Node* head = NULL;
    
    // Insert elements
    int elements[] = {170, 45, 75, 90, 2, 802, 24, 66};
    int n = sizeof(elements) / sizeof(elements[0]);
    
    for (int i = 0; i < n; i++) {
        insertEnd(&head, elements[i]);
    }
    
    printf("Original linked list: ");
    printList(head);
    
    head = radixSortLinkedList(head);
    
    printf("Sorted linked list: ");
    printList(head);
    
    // Free memory
    freeList(head);
}

int main() {
    testLinkedListRadixSort();
}