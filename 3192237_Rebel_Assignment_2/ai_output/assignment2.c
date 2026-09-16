#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 10
#define MAX_LINE 1024

/*
 * Email definition:
 *   category: Boss, Subordinate, Peer, ImportantPerson, OtherPerson
 *   subject:  string
 *   date:     MM-DD-YYYY
 */
typedef struct {
    char *category;
    char *subject;
    char *date;
    unsigned long long sequence;
} Email;

/* Array-based MaxHeap implemented from scratch. */
typedef struct {
    Email *items;
    int size;
    int capacity;
} MaxHeap;

/* ---------- Utility functions ---------- */

static char *duplicate_string(const char *source) {
    size_t length = strlen(source);
    char *copy = malloc(length + 1);

    if (copy == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(copy, source);
    return copy;
}

static int category_priority(const char *category) {
    if (strcmp(category, "Boss") == 0) {
        return 5;
    }
    if (strcmp(category, "Subordinate") == 0) {
        return 4;
    }
    if (strcmp(category, "Peer") == 0) {
        return 3;
    }
    if (strcmp(category, "ImportantPerson") == 0) {
        return 2;
    }
    if (strcmp(category, "OtherPerson") == 0) {
        return 1;
    }

    return 0; /* Input is assumed valid according to the prompt. */
}

/* Converts MM-DD-YYYY into YYYYMMDD for easy chronological comparison. */
static long date_value(const char *date) {
    int month, day, year;

    if (sscanf(date, "%2d-%2d-%4d", &month, &day, &year) != 3) {
        return 0;
    }

    return (long)year * 10000L + month * 100L + day;
}

/*
 * Returns > 0 when a should have higher priority than b.
 * Priority rules:
 *   1. Category
 *   2. Newest date when category is the same
 *   3. Earlier insertion sequence as a deterministic tie-breaker
 */
static int higher_priority(const Email *a, const Email *b) {
    int category_a = category_priority(a->category);
    int category_b = category_priority(b->category);

    if (category_a != category_b) {
        return category_a > category_b;
    }

    if (date_value(a->date) != date_value(b->date)) {
        return date_value(a->date) > date_value(b->date);
    }

    return a->sequence < b->sequence;
}

static void free_email(Email *email) {
    free(email->category);
    free(email->subject);
    free(email->date);

    email->category = NULL;
    email->subject = NULL;
    email->date = NULL;
}

/* ---------- MaxHeap functions ---------- */

static void heap_init(MaxHeap *heap) {
    heap->items = malloc(INITIAL_CAPACITY * sizeof(Email));

    if (heap->items == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    heap->size = 0;
    heap->capacity = INITIAL_CAPACITY;
}

static void heap_resize(MaxHeap *heap) {
    int new_capacity = heap->capacity * 2;
    Email *new_items = realloc(heap->items, new_capacity * sizeof(Email));

    if (new_items == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    heap->items = new_items;
    heap->capacity = new_capacity;
}

static void swap_email(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

static void heapify_up(MaxHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (!higher_priority(&heap->items[index], &heap->items[parent])) {
            break;
        }

        swap_email(&heap->items[index], &heap->items[parent]);
        index = parent;
    }
}

static void heapify_down(MaxHeap *heap, int index) {
    while (1) {
        int left = index * 2 + 1;
        int right = index * 2 + 2;
        int highest = index;

        if (left < heap->size &&
            higher_priority(&heap->items[left], &heap->items[highest])) {
            highest = left;
        }

        if (right < heap->size &&
            higher_priority(&heap->items[right], &heap->items[highest])) {
            highest = right;
        }

        if (highest == index) {
            break;
        }

        swap_email(&heap->items[index], &heap->items[highest]);
        index = highest;
    }
}

static void heap_insert(MaxHeap *heap, Email email) {
    if (heap->size == heap->capacity) {
        heap_resize(heap);
    }

    heap->items[heap->size] = email;
    heapify_up(heap, heap->size);
    heap->size++;
}

static Email heap_peek(const MaxHeap *heap) {
    return heap->items[0];
}

static Email heap_remove_max(MaxHeap *heap) {
    Email max_email = heap->items[0];

    heap->size--;

    if (heap->size > 0) {
        heap->items[0] = heap->items[heap->size];
        heapify_down(heap, 0);
    }

    return max_email;
}

static void heap_destroy(MaxHeap *heap) {
    int i;

    for (i = 0; i < heap->size; i++) {
        free_email(&heap->items[i]);
    }

    free(heap->items);
    heap->items = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

/* ---------- Command processing ---------- */

static void trim_newline(char *line) {
    size_t length = strlen(line);

    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
        line[length - 1] = '\0';
        length--;
    }
}

static void process_email_command(MaxHeap *heap, const char *arguments,
                                  unsigned long long *sequence_counter) {
    char buffer[MAX_LINE];
    char *category;
    char *subject;
    char *date;
    Email email;

    strncpy(buffer, arguments, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    category = strtok(buffer, ",");
    subject = strtok(NULL, ",");
    date = strtok(NULL, ",");

    if (category == NULL || subject == NULL || date == NULL) {
        return;
    }

    email.category = duplicate_string(category);
    email.subject = duplicate_string(subject);
    email.date = duplicate_string(date);
    email.sequence = (*sequence_counter)++;

    heap_insert(heap, email);
}

int main(int argc, char *argv[]) {
    FILE *input;
    char line[MAX_LINE];
    MaxHeap heap;
    unsigned long long sequence_counter = 0;
    int next_pending = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Could not open input file");
        return EXIT_FAILURE;
    }

    heap_init(&heap);

    while (fgets(line, sizeof(line), input) != NULL) {
        char *command;
        char *arguments;

        trim_newline(line);

        if (line[0] == '\0') {
            continue;
        }

        command = strtok(line, " ");
        arguments = strtok(NULL, "");

        if (command == NULL) {
            continue;
        }

        if (strcmp(command, "EMAIL") == 0) {
            if (arguments != NULL) {
                process_email_command(&heap, arguments, &sequence_counter);
            }
        }
        else if (strcmp(command, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap.size);
        }
        else if (strcmp(command, "NEXT") == 0) {
            /* NEXT is invalid if there are no emails or if the previous NEXT
               has not yet been followed by READ. */
            if (heap.size == 0 || next_pending) {
                continue;
            }

            Email next_email = heap_peek(&heap);

            printf("Next email:\n");
            printf("Sender: %s\n", next_email.category);
            printf("Subject: %s\n", next_email.subject);
            printf("Date: %s\n", next_email.date);

            next_pending = 1;
        }
        else if (strcmp(command, "READ") == 0) {
            /* READ is invalid if there are no emails or if there has not
               been a NEXT command since the previous READ. */
            if (heap.size == 0 || !next_pending) {
                continue;
            }

            Email read_email = heap_remove_max(&heap);
            free_email(&read_email);
            next_pending = 0;
        }
        /* Any other command is ignored. The prompt says the input commands
           may be nonsensical, so invalid commands do not produce output. */
    }

    heap_destroy(&heap);
    fclose(input);

    return EXIT_SUCCESS;
}
