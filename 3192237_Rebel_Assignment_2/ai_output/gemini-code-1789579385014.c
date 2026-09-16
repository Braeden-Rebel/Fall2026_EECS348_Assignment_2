#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EMAILS 1000
#define MAX_STR_LEN 256

// Represents a single email as outlined in the assignment criteria
typedef struct {
    char category[MAX_STR_LEN];
    char subject[MAX_STR_LEN];
    char date[MAX_STR_LEN];
} Email;

// MaxHeap definition
typedef struct {
    Email data[MAX_EMAILS];
    int size;
} MaxHeap;

// Utility function to trim leading and trailing whitespaces
char *trimWhitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// Map the string category to an integer priority
int getCategoryValue(const char* cat) {
    if (strcmp(cat, "Boss") == 0) return 5;
    // Handle potential variations in spacing as seen in the prompt vs example
    if (strcmp(cat, "Important Person") == 0 || strcmp(cat, "ImportantPerson") == 0) return 4;
    if (strcmp(cat, "Subordinate") == 0) return 3;
    if (strcmp(cat, "Peer") == 0) return 2;
    return 1; // "Other Person" or "OtherPerson"
}

// Compare two emails based on Category, then by Date (Newest first)
int compareEmails(Email a, Email b) {
    int catA = getCategoryValue(a.category);
    int catB = getCategoryValue(b.category);
    
    // Category priority first
    if (catA != catB) {
        return catA - catB; 
    }
    
    // Tie-breaker: Date in MM-DD-YYYY format
    int m1, d1, y1, m2, d2, y2;
    sscanf(a.date, "%d-%d-%d", &m1, &d1, &y1);
    sscanf(b.date, "%d-%d-%d", &m2, &d2, &y2);
    
    // Convert to comparable integers (YYYYMMDD)
    int date1 = y1 * 10000 + m1 * 100 + d1;
    int date2 = y2 * 10000 + m2 * 100 + d2;
    
    return date1 - date2;
}

// Swap two emails in the heap
void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

// Bubble up logic to maintain MaxHeap property after insertion
void bubbleUp(MaxHeap *heap, int index) {
    if (index == 0) return;
    int parent = (index - 1) / 2;
    if (compareEmails(heap->data[index], heap->data[parent]) > 0) {
        swap(&heap->data[index], &heap->data[parent]);
        bubbleUp(heap, parent);
    }
}

// Bubble down logic to maintain MaxHeap property after extraction
void bubbleDown(MaxHeap *heap, int index) {
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int maxIndex = index;

    if (leftChild < heap->size && compareEmails(heap->data[leftChild], heap->data[maxIndex]) > 0) {
        maxIndex = leftChild;
    }
    if (rightChild < heap->size && compareEmails(heap->data[rightChild], heap->data[maxIndex]) > 0) {
        maxIndex = rightChild;
    }
    if (maxIndex != index) {
        swap(&heap->data[index], &heap->data[maxIndex]);
        bubbleDown(heap, maxIndex);
    }
}

// Insert an email into the heap
void insertEmail(MaxHeap *heap, Email email) {
    if (heap->size >= MAX_EMAILS) return; // Prevent overflow
    heap->data[heap->size] = email;
    bubbleUp(heap, heap->size);
    heap->size++;
}

// Remove and return the top email from the heap
Email extractMax(MaxHeap *heap) {
    Email maxEmail = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    bubbleDown(heap, 0);
    return maxEmail;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    MaxHeap heap;
    heap.size = 0;

    char line[512];
    
    // State machine flags for NEXT and READ rules
    int can_next = 1;
    int can_read = 0;

    while (fgets(line, sizeof(line), file)) {
        char *trimmedLine = trimWhitespace(line);
        if (strlen(trimmedLine) == 0) continue;

        if (strncmp(trimmedLine, "EMAIL ", 6) == 0) {
            // Parse EMAIL command
            char *category = strtok(trimmedLine + 6, ",");
            char *subject = strtok(NULL, ",");
            char *date = strtok(NULL, ""); // Read till end

            if (category && subject && date) {
                Email newEmail;
                strcpy(newEmail.category, trimWhitespace(category));
                strcpy(newEmail.subject, trimWhitespace(subject));
                strcpy(newEmail.date, trimWhitespace(date));
                insertEmail(&heap, newEmail);
            }
        } 
        else if (strcmp(trimmedLine, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap.size);
        } 
        else if (strcmp(trimmedLine, "NEXT") == 0) {
            if (heap.size == 0) {
                printf("Error: No emails in the MaxHeap.\n");
            } else if (!can_next) {
                printf("Error: NEXT is invalid without a READ command between.\n");
            } else {
                printf("Next email:\n");
                printf("Sender: %s\n", heap.data[0].category);
                printf("Subject: %s\n", heap.data[0].subject);
                printf("Date: %s\n", heap.data[0].date);
                
                can_next = 0; 
                can_read = 1;
            }
        } 
        else if (strcmp(trimmedLine, "READ") == 0) {
            if (heap.size == 0) {
                printf("Error: No emails in the MaxHeap.\n");
            } else if (!can_read) {
                printf("Error: READ is invalid without a NEXT command between.\n");
            } else {
                extractMax(&heap);
                can_read = 0;
                can_next = 1;
            }
        }
    }

    fclose(file);
    return 0;
}