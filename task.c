#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

// Task structure
struct Task {
    char description[256];
    int completed;
};

// Global file pointer for logging
FILE *log_file = NULL;

// Custom print function that writes to both screen and file
void log_print(const char *format, ...) {
    va_list args;
    
    // Print to screen
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    // Print to file
    if (log_file != NULL) {
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fflush(log_file); // Ensure data is written immediately
    }
}

// Function prototypes
void add_tasks(struct Task **tasks, int *count, int *capacity);
void view_tasks(struct Task *tasks, int count);
void delete_task(struct Task **tasks, int *count);
void mark_complete(struct Task *tasks, int count);
void save_tasks(struct Task *tasks, int count);
void load_tasks(struct Task **tasks, int *count, int *capacity);
void init_log();
void close_log();

int main() {
    struct Task *tasks = NULL;
    int count = 0;
    int capacity = 2;
    
    // Initialize log file
    init_log();
    
    // Allocate initial memory
    tasks = malloc(capacity * sizeof(struct Task));
    if (tasks == NULL) {
        log_print("Memory allocation failed!\n");
        close_log();
        return 1;
    }
    
    log_print("===== TO-DO LIST PROGRAM STARTED =====\n");
    
    // Load existing tasks from file
    load_tasks(&tasks, &count, &capacity);
    
    int choice;
    
    while (1) {
        log_print("\n===== TO-DO LIST =====\n");
        log_print("1. Add Tasks\n");
        log_print("2. View Tasks\n");
        log_print("3. Delete Task\n");
        log_print("4. Mark as Complete\n");
        log_print("5. Save Tasks\n");
        log_print("6. Exit\n");
        log_print("======================\n");
        log_print("Enter choice: ");
        
        scanf("%d", &choice);
        getchar();
        
        // Log the choice
        fprintf(log_file, "%d\n", choice);
        
        switch (choice) {
            case 1:
                add_tasks(&tasks, &count, &capacity);
                break;
            case 2:
                view_tasks(tasks, count);
                break;
            case 3:
                delete_task(&tasks, &count);
                break;
            case 4:
                mark_complete(tasks, count);
                break;
            case 5:
                save_tasks(tasks, count);
                break;
            case 6:
                save_tasks(tasks, count);
                free(tasks);
                log_print("Goodbye!\n");
                log_print("===== PROGRAM ENDED =====\n");
                close_log();
                return 0;
            default:
                log_print("Invalid choice!\n");
        }
    }
    
    return 0;
}

// Initialize log file
void init_log() {
    // Create log filename with timestamp
    char log_filename[100];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    strftime(log_filename, sizeof(log_filename), "todo_log_%Y%m%d_%H%M%S.txt", tm_info);
    
    log_file = fopen(log_filename, "w");
    if (log_file == NULL) {
        printf("Warning: Could not create log file\n");
    } else {
        printf("📁 Log file created: %s\n", log_filename);
    }
}

// Close log file
void close_log() {
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }
}

// Add tasks until user types "done"
void add_tasks(struct Task **tasks, int *count, int *capacity) {
    char input[256];
    
    log_print("\n--- ADD TASKS ---\n");
    log_print("Enter task description (or 'done' to finish):\n");
    
    while (1) {
        log_print("Task %d: ", *count + 1);
        fgets(input, sizeof(input), stdin);
        
        // Log the input
        if (log_file != NULL) {
            fprintf(log_file, "%s", input);
        }
        
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "done") == 0 || strcmp(input, "DONE") == 0) {
            log_print("(User finished adding tasks)\n");
            break;
        }
        
        if (strlen(input) == 0) {
            log_print("Task cannot be empty!\n");
            continue;
        }
        
        if (*count == *capacity) {
            *capacity *= 2;
            struct Task *temp = realloc(*tasks, *capacity * sizeof(struct Task));
            if (temp == NULL) {
                log_print("Memory allocation failed!\n");
                return;
            }
            *tasks = temp;
            log_print("(Array grew to capacity %d)\n", *capacity);
        }
        
        strcpy((*tasks)[*count].description, input);
        (*tasks)[*count].completed = 0;
        (*count)++;
        
        log_print("✅ Task added!\n");
    }
}

// Display all tasks
void view_tasks(struct Task *tasks, int count) {
    log_print("\n--- VIEW TASKS ---\n");
    
    if (count == 0) {
        log_print("No tasks yet!\n");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        if (tasks[i].completed) {
            log_print("%d. [✅] %s\n", i + 1, tasks[i].description);
        } else {
            log_print("%d. [❌] %s\n", i + 1, tasks[i].description);
        }
    }
}

// Delete a task by number
void delete_task(struct Task **tasks, int *count) {
    int task_number;
    
    log_print("\n--- DELETE TASK ---\n");
    
    if (*count == 0) {
        log_print("No tasks to delete!\n");
        return;
    }
    
    view_tasks(*tasks, *count);
    log_print("Enter task number to delete: ");
    scanf("%d", &task_number);
    getchar();
    
    // Log the input
    if (log_file != NULL) {
        fprintf(log_file, "%d\n", task_number);
    }
    
    if (task_number < 1 || task_number > *count) {
        log_print("Invalid task number!\n");
        return;
    }
    
    for (int i = task_number - 1; i < *count - 1; i++) {
        (*tasks)[i] = (*tasks)[i + 1];
    }
    
    (*count)--;
    log_print("✅ Task deleted!\n");
}

// Toggle task completion status
void mark_complete(struct Task *tasks, int count) {
    int task_number;
    
    log_print("\n--- MARK AS COMPLETE ---\n");
    
    if (count == 0) {
        log_print("No tasks to mark!\n");
        return;
    }
    
    view_tasks(tasks, count);
    log_print("Enter task number to toggle: ");
    scanf("%d", &task_number);
    getchar();
    
    // Log the input
    if (log_file != NULL) {
        fprintf(log_file, "%d\n", task_number);
    }
    
    if (task_number < 1 || task_number > count) {
        log_print("Invalid task number!\n");
        return;
    }
    
    if (tasks[task_number - 1].completed == 0) {
        tasks[task_number - 1].completed = 1;
        log_print("✅ Task marked as complete!\n");
    } else {
        tasks[task_number - 1].completed = 0;
        log_print("↩️  Task marked as incomplete!\n");
    }
}

// Save tasks to file
void save_tasks(struct Task *tasks, int count) {
    FILE *file = fopen("tasks.txt", "w");
    
    if (file == NULL) {
        log_print("Error opening file for saving!\n");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s|%d\n", tasks[i].description, tasks[i].completed);
    }
    
    fclose(file);
    log_print("✅ %d tasks saved to tasks.txt!\n", count);
}

// Load tasks from file
void load_tasks(struct Task **tasks, int *count, int *capacity) {
    FILE *file = fopen("tasks.txt", "r");
    
    if (file == NULL) {
        log_print("No existing tasks file found. Starting fresh.\n");
        return;
    }
    
    char line[256];
    char description[256];
    int completed;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        char *separator = strchr(line, '|');
        if (separator != NULL) {
            *separator = '\0';
            strcpy(description, line);
            completed = atoi(separator + 1);
            
            if (*count == *capacity) {
                *capacity *= 2;
                struct Task *temp = realloc(*tasks, *capacity * sizeof(struct Task));
                if (temp == NULL) {
                    log_print("Memory allocation failed!\n");
                    fclose(file);
                    return;
                }
                *tasks = temp;
            }
            
            strcpy((*tasks)[*count].description, description);
            (*tasks)[*count].completed = completed;
            (*count)++;
        }
    }
    
    fclose(file);
    
    if (*count > 0) {
        log_print("✅ %d tasks loaded from file!\n", *count);
    }
}
