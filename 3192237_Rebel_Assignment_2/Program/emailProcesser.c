// Name: Email Processor
// Takes in a list of email commands of EMAIL, NEXT, READ, COUNT
// Processes those commands and print output to console based on the execution of those commands
// Input: Path to file
// Output: Console output of result of processing file
// Collaborators: N/A
// Sources: Gemini Pro 3.1 (Gemini)
// Author: Braeden Rebel (Rebel)
// Creation Date: 9/17/2026
// Revision Date: 9/17/2026

// Include useful libraries (Gemini)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define constants (Gemini)
#define MAX_EMAILS 1000
#define MAX_STR_LEN 256

// Email definition (Gemini)
typedef struct {
    char category[MAX_STR_LEN];
    char subject[MAX_STR_LEN];
    char date[MAX_STR_LEN];
    int id;
} Email;

// MaxHeap definition (Gemini)
typedef struct {
    Email data[MAX_EMAILS];
    int size;
} MaxHeap;

// Utility function to trim leading and trailing whitespaces (Gemini)
char *trimWhitespace(char *str) {
    char *end; // Declare a pointer to the end of the string (Gemini)
    while(isspace((unsigned char)*str)) str++; // While the current character is a space, check next character until not a space (Gemini)
    if(*str == 0) return str; // If the string has no address then return (Gemini)
    end = str + strlen(str) - 1; // Get the end of the string (Gemini)
    while(end > str && isspace((unsigned char)*end)) end--; // While the current character is a space and within the string, check previous character (Gemini)
    end[1] = '\0'; // Clear the trailing whitespace (Gemini)
    return str; // Return the final string address (Gemini)
}

// Map the string category to an integer priority (Gemini)
int getCategoryValue(const char* cat) {
    if (strcmp(cat, "Boss") == 0) return 5; // If category is "Boss" then give priority of 5 (Gemini)
    if (strcmp(cat, "Subordinate") == 0) return 4; // If category is "Subordinate" then then give priority of 4 (Rebel)
    if (strcmp(cat, "Peer") == 0) return 3; // If category is "Peer" then then give priority of 3 (Gemini)
    if (strcmp(cat, "ImportantPerson") == 0) return 2; // If category is "ImportantPerson" then give priority of 2 (Rebel)
    return 1; // Assume category is "OtherPerson" since it is the only option left (Gemini)
}

// Compare two emails based on Category, then by Date (Newest first) (Gemini)
int compareEmails(Email a, Email b) {
    // Get category values for both emails a and b (Gemini)
    int catA = getCategoryValue(a.category);
    int catB = getCategoryValue(b.category);
    
    // Check if category value is different and return the difference of catA and catB if they are (Gemini)
    if (catA != catB) {
        return catA - catB; 
    }
    
    // Tie-breaker if categories are same: Date in MM-DD-YYYY format (Gemini)
    int m1, d1, y1, m2, d2, y2;
    sscanf(a.date, "%d-%d-%d", &m1, &d1, &y1);
    sscanf(b.date, "%d-%d-%d", &m2, &d2, &y2);
    
    // Convert to comparable integers (YYYYMMDD) (Gemini)
    int date1 = y1 * 10000 + m1 * 100 + d1;
    int date2 = y2 * 10000 + m2 * 100 + d2;
    
    // If dates are different, return their result (Rebel)
    if (date1 != date2)
    {
        return date1 - date2;
    }
    // Final tiebreaker using id
    return a.id - b.id;
}

// Swap two emails in the heap (Gemini)
void swap(Email *a, Email *b) {
    Email temp = *a; // Create temp variable to hold pointer to a (Gemini)
    *a = *b; // Assign address a to look at address b (Gemini)
    *b = temp; // Assign address b to look at old address a (Gemini)
}

// Bubble up logic to maintain MaxHeap property after insertion (Gemini)
void bubbleUp(MaxHeap *heap, int index) {
    if (index == 0) return; // If trying to bubble up email stored in top, then bubbling up is done (Gemini)
    int parent = (index - 1) / 2; // Get parent index of target index (Gemini)
    if (compareEmails(heap->data[index], heap->data[parent]) > 0) { // Compare the emails and if the current email has higher priority than the parent email... (Gemini)
        swap(&heap->data[index], &heap->data[parent]); // Swap the addresses of the two indexes (Gemini)
        bubbleUp(heap, parent); // Bubble the current email up which is now located at the parent index (Gemini)
    }
}

// Bubble down logic to maintain MaxHeap property after extraction (Gemini)
void bubbleDown(MaxHeap *heap, int index) {
    int leftChild = 2 * index + 1; // Get left index of target index (Gemini)
    int rightChild = 2 * index + 2; // Get right index of target index (Gemini)
    int maxIndex = index; // Assign current index to maxIndex (Gemini)

    // Check if the left index is in the domain of the heap's size and
    // If the priority of the left email is greater than the current email
    // Assign the maxIndex to leftChild (Gemini)
    if (leftChild < heap->size && compareEmails(heap->data[leftChild], heap->data[maxIndex]) > 0) {
        maxIndex = leftChild;
    }
    // Check if the right index is in the domain of the heap's size and
    // If the priority of the right email is greater than the current email
    // Assign the maxIndex to rightChild (Gemini)
    if (rightChild < heap->size && compareEmails(heap->data[rightChild], heap->data[maxIndex]) > 0) {
        maxIndex = rightChild;
    }
    // If the index of the email with the max value isn't the target index... (Gemini)
    if (maxIndex != index) {
        swap(&heap->data[index], &heap->data[maxIndex]); // Swap the max email with the current email (Gemini)
        bubbleDown(heap, maxIndex); // Recursively bubble down the current email which is now located at maxIndex (Gemini)
    }
}

// Insert an email into the heap (Gemini)
void insertEmail(MaxHeap *heap, Email email) {
    if (heap->size >= MAX_EMAILS) // Check if the heap's size exceeds or equals MAX_EMAILS constant before inserting (Gemini)
    {
        printf("Error: emails have exceeded max and no more can be added"); // Print error message for user to know no more emails can be added (Rebel)
        return; // Prevent overflow of beyond MAX_EMAILS (Gemini)
    }
    heap->data[heap->size] = email; // Assign the last element of the heap to the last index of the heap (Gemini)
    bubbleUp(heap, heap->size); // Bubble up the newly added email (Gemini)
    heap->size++; // Increase the size of the heap to account for the new element (Gemini)
}

// Remove and return the max email from the heap (Gemini)
Email extractMax(MaxHeap *heap) {
    Email maxEmail = heap->data[0]; // Assign max email to first element of the heap's data (Gemini)
    heap->data[0] = heap->data[heap->size - 1]; // Assign the first element of the heap's data to the last element in the heap (Gemini)
    heap->size--; // Reduce the heap's size by one to account for the lost element (Gemini)
    bubbleDown(heap, 0); // Bubble down the email now at the max position (Gemini)
    return maxEmail; // Return the removed email (Gemini)
}

int main(int argc, char *argv[]) {
    // If the program is only passed with one argument then inform the user of usage and exit (Gemini)
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Get the file that was passed in and read it, printing a failure message if it fails (Gemini)
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    // Create a heap and set initialize its size to 0 (Gemini)
    MaxHeap heap;
    heap.size = 0;

    // Create a variable to be used for input line (Gemini)
    char line[512];
    
    // Current email for keeping track of next / read calls (Rebel)
    Email *current_email;
    current_email = NULL;
    // Id tracker to know which emails came in first (Rebel)
    int idTracker = 0;

    while (fgets(line, sizeof(line), file)) { // Get the next line in the file (Gemini)
        char *trimmedLine = trimWhitespace(line); // Trim the line (Gemini)
        if (strlen(trimmedLine) == 0) continue; // If the line is empty then continue (Gemini)

        if (strncmp(trimmedLine, "EMAIL ", 6) == 0) { // If the line has a command of EMAIL (Gemini)...
            // Read line and get pointers to the values of the new emails, category, subject, and date (Gemini)
            char *category = strtok(trimmedLine + 6, ",");
            char *subject = strtok(NULL, ",");
            char *date = strtok(NULL, "");

            if (category && subject && date) { // If the category, subject, and date all exist... (Gemini)
                Email newEmail; // Create a new email and assign all of its properties (Gemini)
                strcpy(newEmail.category, trimWhitespace(category));
                strcpy(newEmail.subject, trimWhitespace(subject));
                strcpy(newEmail.date, trimWhitespace(date));
                newEmail.id = idTracker++;
                // Insert the newly created email into the heap (Gemini)
                insertEmail(&heap, newEmail);
            }
        } 
        else if (strcmp(trimmedLine, "COUNT") == 0) { // If the command is COUNT... (Gemini)
            printf("There are %d emails to read.\n", heap.size + (current_email != NULL)); // Print the size of the heap (Rebel)
        } 
        else if (strcmp(trimmedLine, "NEXT") == 0) { // If the command is NEXT (Gemini)
            // Check if the heap is empty and print error message if so (Gemini)
            if (heap.size == 0) {
                printf("Error: No emails in the MaxHeap.\n");
            // Check if next is a valid command and print error message if so (Rebel)
            } else if (current_email != NULL) {
                printf("Error: NEXT is invalid until the current email has been read.\n");
            } else {
                // Assign next email to current_email (Rebel)
                Email target = extractMax(&heap);
                current_email = &target;
                // Print current email information (Rebel)
                printf("Next email:\n");
                printf("Sender: %s\n", current_email->category);
                printf("Subject: %s\n", current_email->subject);
                printf("Date: %s\n", current_email->date);
                printf("\n");
            }
        } 
        // If the command is READ (Gemini)
        else if (strcmp(trimmedLine, "READ") == 0) {
            // If there is no current email then print error message (Rebel)
            if (current_email == NULL) {
                printf("Error: READ is invalid when no email is active.\n");
            } else {
                current_email = NULL; // Point the current_email at null (Rebel)
            }
        }
    }

    fclose(file);
    return 0;
}