#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 100
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 20
#define VERSION "1.0.0"

/* Define the missing types */
typedef enum {
    FIRST_NAME,
    LAST_NAME,
    MOBILE_NUMBER
} search_t;

/* Define the Contact structure */
typedef struct {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char mobileNumber[MAX_PHONE_LEN];
} Contact;

/* Function prototypes */
void addEntry();
void display(int index);
void displayAll();
void search(char *key, search_t type);
void removeEntry(char *mobileNumber);
void readFromFile();
void saveToFile();
void strtolower(char *str);

/* array to hold our contacts */
Contact phonebook[MAX_SIZE];

/* current size of the phonebook array */
int currentSize = 0;

/* Helper function to convert string to lowercase */
void strtolower(char *str) {
    for(int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void addEntry()
{
    if(currentSize == MAX_SIZE) {
        puts("Error: phonebook is full!");
        return;
    }

    Contact c;
    printf("Enter First Name: ");
    fgets(c.firstName, sizeof(c.firstName), stdin);
    c.firstName[strcspn(c.firstName, "\n")] = '\0';

    printf("Enter Last Name: ");
    fgets(c.lastName, sizeof(c.lastName), stdin);
    c.lastName[strcspn(c.lastName, "\n")] = '\0';

    printf("Enter Mobile Number: ");
    fgets(c.mobileNumber, sizeof(c.mobileNumber), stdin);
    c.mobileNumber[strcspn(c.mobileNumber, "\n")] = '\0';

    phonebook[currentSize] = c;
    ++currentSize;

    saveToFile();  /* auto-save: creates file if not exists, updates if exists */
    printf("Contact added.\n");
}

void display(int index)
{
    if(index < 0 || index >= currentSize) {
        puts("Error: invalid index!");
        return;
    }

    Contact c = phonebook[index];
    printf("First Name: %s\n", c.firstName);
    printf("Last Name : %s\n", c.lastName);
    printf("Mobile No : %s\n", c.mobileNumber);
}

void displayAll()
{
    if(currentSize == 0) {
        puts("Phonebook is empty!");
        return;
    }
    int i;
    for(i=0; i<currentSize; i++) {
        display(i);
        puts(""); // extra new line
    }
}

void search(char *key, search_t type)
{
    int found = 0;
    int i;

    char key_lower[41];
    strcpy(key_lower, key);
    strtolower(key_lower);

    char content[41];

    if(type == FIRST_NAME) {
        for(i=0; i<currentSize; i++) {
            strcpy(content, phonebook[i].firstName);
            strtolower(content);

            if(strcmp(content, key_lower) == 0) {
                display(i);
                found = 1;
            }
        }
    }
    else if(type == LAST_NAME) {
        for(i=0; i<currentSize; i++) {
            strcpy(content, phonebook[i].lastName);
            strtolower(content);

            if(strcmp(content, key_lower) == 0) {
                display(i);
                found = 1;
            }
        }
    }
    else if(type == MOBILE_NUMBER) {
        for(i=0; i<currentSize; i++) {
            strcpy(content, phonebook[i].mobileNumber);
            strtolower(content);

            if(strcmp(content, key_lower) == 0) {
                display(i);
                found = 1;
            }
        }
    }
    else {
        puts("Error: invalid search type!");
        return;
    }

    if(!found) {
        puts("Not found in the phone book");
    }
}

void removeEntry(char *mobileNumber)
{
    if(currentSize == 0) {
        puts("Phonebook is empty! Nothing to delete!");
        return;
    }

    int i, j;
    int count = 0;
    for(i=0; i<currentSize; i++) {
        if(strcmp(phonebook[i].mobileNumber, mobileNumber) == 0) {
            for(j=i; j<currentSize-1; j++) {
                phonebook[j] = phonebook[j+1];
            }

            // Clear the last entry
            strcpy(phonebook[currentSize-1].firstName, "");
            strcpy(phonebook[currentSize-1].lastName, "");
            strcpy(phonebook[currentSize-1].mobileNumber, "");

            currentSize -= 1;
            ++count;
            i--; // Check the new element at current index
        }
    }
    if(count == 0) {
        puts("No entry deleted");
    }
    else {
        printf("%d entries deleted\n", count);
        saveToFile();  /* auto-save after deletion */
    }
}

void readFromFile()
{
    FILE *fp;
    if((fp = fopen("phonebook.txt", "r")) == NULL) {
        /* No file yet — first run, start with empty phonebook */
        return;
    }

    currentSize = 0;
    char line[200];

    /*
     * TXT format (one contact = 3 lines):
     *   FirstName|LastName|MobileNumber
     * Example:
     *   John|Doe|01711-123456
     */
    while(currentSize < MAX_SIZE && fgets(line, sizeof(line), fp) != NULL) {
        /* Strip trailing newline */
        line[strcspn(line, "\n")] = '\0';

        /* Skip empty lines */
        if(strlen(line) == 0) continue;

        /* Parse the pipe-delimited line */
        char *first  = strtok(line, "|");
        char *last   = strtok(NULL, "|");
        char *mobile = strtok(NULL, "|");

        if(first == NULL || last == NULL || mobile == NULL) {
            puts("Warning: skipping malformed line in phonebook.txt");
            continue;
        }

        strncpy(phonebook[currentSize].firstName,    first,  MAX_NAME_LEN - 1);
        strncpy(phonebook[currentSize].lastName,     last,   MAX_NAME_LEN - 1);
        strncpy(phonebook[currentSize].mobileNumber, mobile, MAX_PHONE_LEN - 1);

        /* Ensure null-termination */
        phonebook[currentSize].firstName[MAX_NAME_LEN - 1]    = '\0';
        phonebook[currentSize].lastName[MAX_NAME_LEN - 1]     = '\0';
        phonebook[currentSize].mobileNumber[MAX_PHONE_LEN - 1] = '\0';

        currentSize++;
    }

    fclose(fp);
    printf("Loaded %d contact(s) from phonebook\n", currentSize);
}

void saveToFile()
{
    FILE *fp;
    if((fp = fopen("phonebook.txt", "w")) == NULL) {
        puts("Error: can't create phonebook.txt!");
        return;
    }

    /*
     * Write each contact as one pipe-delimited line:
     *   FirstName|LastName|MobileNumber
     */
    int i;
    for(i = 0; i < currentSize; i++) {
        fprintf(fp, "%s|%s|%s\n",
                phonebook[i].firstName,
                phonebook[i].lastName,
                phonebook[i].mobileNumber);
    }

    fclose(fp);
    printf("%d contact is Saved\n", currentSize);
}

/* Main function for testing */
int main(int argc, char *argv[]) {
    int choice;
    char searchKey[50];
    char mobileNumber[20];

    // Check for version flag
    if (argc > 1) {
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            printf("Phonebook version %s\n", VERSION);
            return 0;
        }
    }

    // Try to load existing phonebook
    readFromFile();

    do {
        printf("\n=== Phonebook Menu ===\n");
        printf("1. Add Contact\n");
        printf("2. Display All Contacts\n");
        printf("3. Search by First Name\n");
        printf("4. Search by Last Name\n");
        printf("5. Search by Mobile Number\n");
        printf("6. Remove Contact by Mobile Number\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear the newline character

        switch(choice) {
            case 1:
                addEntry();
                break;
            case 2:
                displayAll();
                break;
            case 3:
                printf("Enter first name to search: ");
                fgets(searchKey, sizeof(searchKey), stdin);
                searchKey[strcspn(searchKey, "\n")] = '\0';
                search(searchKey, FIRST_NAME);
                break;
            case 4:
                printf("Enter last name to search: ");
                fgets(searchKey, sizeof(searchKey), stdin);
                searchKey[strcspn(searchKey, "\n")] = '\0';
                search(searchKey, LAST_NAME);
                break;
            case 5:
                printf("Enter mobile number to search: ");
                fgets(searchKey, sizeof(searchKey), stdin);
                searchKey[strcspn(searchKey, "\n")] = '\0';
                search(searchKey, MOBILE_NUMBER);
                break;
            case 6:
                printf("Enter mobile number to remove: ");
                fgets(mobileNumber, sizeof(mobileNumber), stdin);
                mobileNumber[strcspn(mobileNumber, "\n")] = '\0';
                removeEntry(mobileNumber);
                break;
            case 7:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while(choice != 7);

    return 0;
}
