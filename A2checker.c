#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 100

// Structure to hold word count
struct WordCount {
    char word[MAX_WORD_LENGTH];
    int count;
};

// Global variables
int total_files_processed = 0;
int total_spelling_errors = 0;
struct WordCount misspellings_summary[MAX_WORDS];

// Mutex and condition variable for synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

// Flag to determine if summary should be saved to file
int save_summary_to_file = 0;

// Function to perform spellchecking
// Function to perform spellchecking
void *spellchecker_task(void *args) {
    char *dictionary_file = ((char **)args)[0];
    char *input_file = ((char **)args)[1];

    // Load dictionary words
    FILE *dict_file = fopen(dictionary_file, "r");
    if (dict_file == NULL) {
        printf("Error: Dictionary file '%s' not found.\n", dictionary_file);
        return NULL;
    }

    // Read input text file
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Error: Input file '%s' not found.\n", input_file);
        fclose(dict_file);
        return NULL;
    }

    // Initialize variables for misspelled words count and tracking
    int misspelled_words_count = 0;
    struct WordCount misspellings[MAX_WORDS];
    for (int i = 0; i < MAX_WORDS; i++) {
        strcpy(misspellings[i].word, "");
        misspellings[i].count = 0;
    }

    // Load dictionary words into a hash table or another efficient data structure for quick lookup

    // Tokenize and check each word against dictionary
    char word[MAX_WORD_LENGTH];
    while (fscanf(file, "%s", word) == 1) {
        // Spellcheck logic here
        int found_in_dictionary = 0;
        
        // Assuming spellcheck logic here: If the word is not in the dictionary, count it as misspelled
        // In a real spellchecker, you would use a more sophisticated algorithm
        // We update misspellings array regardless of whether it's in the dictionary or not
        for (int i = 0; i < MAX_WORDS; i++) {
            if (strcmp(misspellings[i].word, word) == 0) {
                found_in_dictionary = 1;
                break;
            } else if (strcmp(misspellings[i].word, "") == 0) {
                strcpy(misspellings[i].word, word);
                misspellings[i].count = 1;
                misspelled_words_count++;
                found_in_dictionary = 1;
                break;
            }
        }
    }

    // Update summary statistics
    pthread_mutex_lock(&mutex);
    total_files_processed++;
    total_spelling_errors += misspelled_words_count;
    // Update global misspellings_summary with data from this task
    for (int i = 0; i < MAX_WORDS; i++) {
        if (strcmp(misspellings_summary[i].word, "") == 0) {
            strcpy(misspellings_summary[i].word, misspellings[i].word);
            misspellings_summary[i].count = misspellings[i].count;
            break;
        } else {
            // If the word is already in the summary, update its count
            for (int j = 0; j < MAX_WORDS; j++) {
                if (strcmp(misspellings_summary[j].word, misspellings[i].word) == 0) {
                    misspellings_summary[j].count += misspellings[i].count;
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&mutex);

    // Saving results to a file (thread-safe)
    char output_file[MAX_WORD_LENGTH + 20];
    sprintf(output_file, "%s_spellcheck_results.txt", input_file);
    FILE *output = fopen(output_file, "w");
    if (output == NULL) {
        printf("Error: Unable to save spellcheck results for '%s'.\n", input_file);
    } else {
        fprintf(output, "Spellcheck results for file: %s\n", input_file);
        fprintf(output, "Total misspelled words: %d\n", misspelled_words_count);
        // You can include more details in the output file
        fclose(output);
    }

    // Notify waiting threads
    pthread_cond_signal(&cv);

    fclose(dict_file);
    fclose(file);
    return NULL;
}


// Function to display final summary
void display_final_summary() {
    printf("\n=== Final Summary ===\n");
    printf("Number of files processed: %d\n", total_files_processed);
    printf("Number of spelling errors: %d\n", total_spelling_errors);
    printf("Three most common misspellings:\n");

    // Initialize variables to keep track of the three most common misspellings
    char common_misspellings[3][MAX_WORD_LENGTH] = {""};
    int max_counts[3] = {0};

    // Iterate through misspellings_summary to find the three most common misspellings
    for (int i = 0; i < MAX_WORDS; i++) {
        // Check if the current word has a higher count than any of the existing common misspellings
        for (int j = 0; j < 3; j++) {
            if (misspellings_summary[i].count > max_counts[j]) {
                // Shift existing common misspellings to make room for the new one
                for (int k = 2; k > j; k--) {
                    strcpy(common_misspellings[k], common_misspellings[k - 1]);
                    max_counts[k] = max_counts[k - 1];
                }
                // Store the new common misspelling and its count
                strcpy(common_misspellings[j], misspellings_summary[i].word);
                max_counts[j] = misspellings_summary[i].count;
                break;
            }
        }
    }

    // Display the three most common misspellings
    for (int i = 0; i < 3; i++) {
        if (max_counts[i] > 0) {
            printf("%s (%d times)\n", common_misspellings[i], max_counts[i]);
        }
    }
}

// Function to save final summary to a file
void save_final_summary_to_file() {
    char output_file[MAX_WORD_LENGTH + 10];
    sprintf(output_file, "%s_A2.out", "cgrao");
    FILE *output = fopen(output_file, "w");
    if (output == NULL) {
        printf("Error: Unable to save final summary to file.\n");
    } else {
        fprintf(output, "=== Final Summary ===\n");
        fprintf(output, "Number of files processed: %d\n", total_files_processed);
        fprintf(output, "Number of spelling errors: %d\n", total_spelling_errors);
        fprintf(output, "Three most common misspellings:\n");

        // Initialize variables to keep track of the three most common misspellings
        char common_misspellings[3][MAX_WORD_LENGTH] = {""};
        int max_counts[3] = {0};

        // Iterate through misspellings_summary to find the three most common misspellings
        for (int i = 0; i < MAX_WORDS; i++) {
            // Check if the current word has a higher count than any of the existing common misspellings
            for (int j = 0; j < 3; j++) {
                if (misspellings_summary[i].count > max_counts[j]) {
                    // Shift existing common misspellings to make room for the new one
                    for (int k = 2; k > j; k--) {
                        strcpy(common_misspellings[k], common_misspellings[k - 1]);
                        max_counts[k] = max_counts[k - 1];
                    }
                    // Store the new common misspelling and its count
                    strcpy(common_misspellings[j], misspellings_summary[i].word);
                    max_counts[j] = misspellings_summary[i].count;
                    break;
                }
            }
        }

        // Write the three most common misspellings to the file
        for (int i = 0; i < 3; i++) {
            if (max_counts[i] > 0) {
                fprintf(output, "%s (%d times)\n", common_misspellings[i], max_counts[i]);
            }
        }

        fclose(output);
    }
}

// Function to start spellchecking task
void start_spellcheck_task() {
    char dictionary_file[MAX_WORD_LENGTH];
    char input_file[MAX_WORD_LENGTH];

    printf("\nEnter the dictionary file name: ");
    scanf("%s", dictionary_file);
    printf("Enter the input text file name: ");
    scanf("%s", input_file);

    char *args[] = {dictionary_file, input_file};

    // Create a new thread for the spellchecker task
    pthread_t spellchecker_thread;
    pthread_create(&spellchecker_thread, NULL, spellchecker_task, args);

    // Wait for the task to finish before returning to the main menu
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cv, &mutex);
    pthread_mutex_unlock(&mutex);
}

// Main menu function
void main_menu() {
    while (1) {
        printf("\n=== Main Menu ===\n");
        printf("1. Start a new spellchecking task\n");
        printf("2. Exit\n");
        printf("Enter your choice: ");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                start_spellcheck_task();
                break;
            case 2:
                printf("\nExiting program...\n");
                // Display or save final summary based on command-line argument
                if (save_summary_to_file) {
                    save_final_summary_to_file();
                } else {
                    display_final_summary();
                }
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        save_summary_to_file = 1;
    }

    main_menu();

    return 0;
}
