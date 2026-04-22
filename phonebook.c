#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE      100
#define MAX_NAME_LEN   50
#define MAX_PHONE_LEN  20
#define FILE_NAME     "phonebook.txt"

struct Contact {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char mobileNumber[MAX_PHONE_LEN];
};

struct Contact phonebook[MAX_SIZE];
int currentSize = 0;

struct HuffNode {
    char ch;
    int  freq;
    struct HuffNode *left;
    struct HuffNode *right;
};

void toLower(char *dest, const char *src) {
    int i;
    for (i = 0; src[i]; i++)
        dest[i] = tolower((unsigned char)src[i]);
    dest[i] = '\0';
}

void saveToFile() {
    FILE *fp = fopen(FILE_NAME, "w");
    if (fp == NULL) {
        printf("Error: could not save to %s\n", FILE_NAME);
        return;
    }
    int i;
    for (i = 0; i < currentSize; i++) {
        fprintf(fp, "%s|%s|%s\n",
                phonebook[i].firstName,
                phonebook[i].lastName,
                phonebook[i].mobileNumber);
    }
    fclose(fp);
    printf("Saved to %s\n", FILE_NAME);
}

void loadFromFile() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (fp == NULL) return;

    char line[150];
    while (currentSize < MAX_SIZE && fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        char *first  = strtok(line, "|");
        char *last   = strtok(NULL, "|");
        char *mobile = strtok(NULL, "|");

        if (first == NULL || last == NULL || mobile == NULL) continue;

        strcpy(phonebook[currentSize].firstName,    first);
        strcpy(phonebook[currentSize].lastName,     last);
        strcpy(phonebook[currentSize].mobileNumber, mobile);
        currentSize++;
    }
    fclose(fp);
    printf("Loaded %d contact(s) from %s\n", currentSize, FILE_NAME);
}

int partition(int lo, int hi) {
    char pivot[MAX_NAME_LEN];
    toLower(pivot, phonebook[hi].firstName);
    int i = lo - 1, j;
    for (j = lo; j < hi; j++) {
        char cur[MAX_NAME_LEN];
        toLower(cur, phonebook[j].firstName);
        if (strcmp(cur, pivot) <= 0) {
            i++;
            struct Contact tmp = phonebook[i];
            phonebook[i] = phonebook[j];
            phonebook[j] = tmp;
        }
    }
    struct Contact tmp = phonebook[i+1];
    phonebook[i+1] = phonebook[hi];
    phonebook[hi]  = tmp;
    return i + 1;
}

void quickSort(int lo, int hi) {
    if (lo < hi) {
        int p = partition(lo, hi);
        quickSort(lo, p - 1);
        quickSort(p + 1, hi);
    }
}

void selectionSort() {
    int i, j, minIdx;
    for (i = 0; i < currentSize - 1; i++) {
        minIdx = i;
        for (j = i + 1; j < currentSize; j++) {
            if (strcmp(phonebook[j].mobileNumber,
                       phonebook[minIdx].mobileNumber) < 0)
                minIdx = j;
        }
        if (minIdx != i) {
            struct Contact tmp = phonebook[i];
            phonebook[i]      = phonebook[minIdx];
            phonebook[minIdx] = tmp;
        }
    }
}

int binarySearch(const char *key) {
    char keyLow[MAX_NAME_LEN];
    toLower(keyLow, key);
    int lo = 0, hi = currentSize - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        char midLow[MAX_NAME_LEN];
        toLower(midLow, phonebook[mid].firstName);
        int cmp = strcmp(midLow, keyLow);
        if      (cmp == 0) return mid;
        else if (cmp < 0)  lo = mid + 1;
        else               hi = mid - 1;
    }
    return -1;
}

struct HuffNode *newNode(char ch, int freq) {
    struct HuffNode *n = malloc(sizeof(struct HuffNode));
    n->ch = ch; n->freq = freq;
    n->left = n->right = NULL;
    return n;
}

struct HuffNode *extractMin(struct HuffNode **heap, int *size) {
    int i, minIdx = 0;
    for (i = 1; i < *size; i++)
        if (heap[i]->freq < heap[minIdx]->freq) minIdx = i;
    struct HuffNode *mn = heap[minIdx];
    heap[minIdx] = heap[--(*size)];
    return mn;
}

void printCodes(struct HuffNode *root, char *code, int depth) {
    if (!root) return;
    if (!root->left && !root->right) {
        code[depth] = '\0';
        printf("   '%c'  =>  %s\n",
               root->ch == ' ' ? '_' : root->ch, code);
        return;
    }
    code[depth] = '0'; printCodes(root->left,  code, depth + 1);
    code[depth] = '1'; printCodes(root->right, code, depth + 1);
}

void freeTree(struct HuffNode *root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void huffmanCoding(const char *text) {
    int freq[256] = {0}, i;
    for (i = 0; text[i]; i++) freq[(unsigned char)text[i]]++;

    struct HuffNode *heap[256];
    int size = 0;
    for (i = 0; i < 256; i++)
        if (freq[i]) heap[size++] = newNode((char)i, freq[i]);

    if (size == 0) return;

    while (size > 1) {
        struct HuffNode *a = extractMin(heap, &size);
        struct HuffNode *b = extractMin(heap, &size);
        struct HuffNode *m = newNode('\0', a->freq + b->freq);
        m->left = a; m->right = b;
        heap[size++] = m;
    }

    printf("\n  Huffman Encoding for \"%s\":\n", text);
    printf("  ---------------------------------\n");
    char code[256];
    if (!heap[0]->left && !heap[0]->right)
        printf("   '%c'  =>  0\n", heap[0]->ch);
    else
        printCodes(heap[0], code, 0);

    printf("  Original: %d bits | Huffman: fewer bits\n",
           (int)strlen(text) * 8);
    freeTree(heap[0]);
}

void knapsack(int budget) {
    if (currentSize == 0) {
        printf("No contacts available.\n");
        return;
    }
    int n = currentSize;
    int i, w;

    int *weight = malloc(n * sizeof(int));
    int *value  = malloc(n * sizeof(int));

    for (i = 0; i < n; i++) {
        weight[i] = (int)(strlen(phonebook[i].firstName) +
                          strlen(phonebook[i].lastName)  +
                          strlen(phonebook[i].mobileNumber));
        value[i]  = 1;
    }

    int **dp = malloc((n + 1) * sizeof(int *));
    for (i = 0; i <= n; i++)
        dp[i] = calloc(budget + 1, sizeof(int));

    for (i = 1; i <= n; i++)
        for (w = 0; w <= budget; w++) {
            dp[i][w] = dp[i-1][w];
            if (weight[i-1] <= w &&
                dp[i-1][w - weight[i-1]] + value[i-1] > dp[i][w])
                dp[i][w] = dp[i-1][w - weight[i-1]] + value[i-1];
        }

    printf("\n  Contacts fitting in %d-char budget:\n", budget);
    printf("  ---------------------------------\n");
    w = budget;
    for (i = n; i >= 1; i--) {
        if (dp[i][w] != dp[i-1][w]) {
            printf("   %s %s | %s (size: %d)\n",
                   phonebook[i-1].firstName,
                   phonebook[i-1].lastName,
                   phonebook[i-1].mobileNumber,
                   weight[i-1]);
            w -= weight[i-1];
        }
    }
    printf("  Total contacts selected: %d\n", dp[n][budget]);

    for (i = 0; i <= n; i++) free(dp[i]);
    free(dp); free(weight); free(value);
}

void coinChange(int idx) {
    int total = (int)(strlen(phonebook[idx].firstName)  +
                      strlen(phonebook[idx].lastName)   +
                      strlen(phonebook[idx].mobileNumber));

    int coins[] = {1, 2, 5, 10, 25, 50};
    int nc = 6, i;

    int *dp = malloc((total + 1) * sizeof(int));
    for (i = 0; i <= total; i++) dp[i] = total + 1;
    dp[0] = 0;

    for (i = 1; i <= total; i++)
        for (int j = 0; j < nc; j++)
            if (coins[j] <= i && dp[i - coins[j]] + 1 < dp[i])
                dp[i] = dp[i - coins[j]] + 1;

    printf("\n  Coin Change for: %s %s\n",
           phonebook[idx].firstName, phonebook[idx].lastName);
    printf("  ---------------------------------\n");
    printf("  Total data length : %d chars\n", total);
    printf("  Min units needed  : %d\n", dp[total]);

    printf("  Breakdown         : ");
    int rem = total;
    int bigCoins[] = {50, 25, 10, 5, 2, 1};
    for (i = 0; i < nc && rem > 0; i++)
        while (rem >= bigCoins[i]) {
            printf("%d ", bigCoins[i]);
            rem -= bigCoins[i];
        }
    printf("\n");
    free(dp);
}

void displayOne(int i) {
    printf("  First Name : %s\n", phonebook[i].firstName);
    printf("  Last Name  : %s\n", phonebook[i].lastName);
    printf("  Mobile No  : %s\n", phonebook[i].mobileNumber);
}

void displayAll() {
    if (currentSize == 0) { printf("Phonebook is empty.\n"); return; }
    quickSort(0, currentSize - 1);
    int i;
    for (i = 0; i < currentSize; i++) {
        printf("\n--- Contact %d ---\n", i + 1);
        displayOne(i);
    }
}

void addContact() {
    if (currentSize == MAX_SIZE) {
        printf("Phonebook is full!\n");
        return;
    }

    printf("First Name   : ");
    fgets(phonebook[currentSize].firstName, MAX_NAME_LEN, stdin);
    phonebook[currentSize].firstName[strcspn(phonebook[currentSize].firstName, "\n")] = '\0';

    printf("Last Name    : ");
    fgets(phonebook[currentSize].lastName, MAX_NAME_LEN, stdin);
    phonebook[currentSize].lastName[strcspn(phonebook[currentSize].lastName, "\n")] = '\0';

    printf("Mobile Number: ");
    fgets(phonebook[currentSize].mobileNumber, MAX_PHONE_LEN, stdin);
    phonebook[currentSize].mobileNumber[strcspn(phonebook[currentSize].mobileNumber, "\n")] = '\0';

    currentSize++;
    quickSort(0, currentSize - 1);
    saveToFile();
    printf("Contact added successfully.\n");
}

void searchContact() {
    if (currentSize == 0) { printf("Phonebook is empty.\n"); return; }

    char key[MAX_NAME_LEN];
    printf("Enter first name to search: ");
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0';

    quickSort(0, currentSize - 1);
    int idx = binarySearch(key);

    if (idx == -1) {
        printf("Contact not found.\n");
    } else {
        printf("\n--- Contact Found ---\n");
        displayOne(idx);
        huffmanCoding(phonebook[idx].firstName);
    }
}

void deleteContact() {
    if (currentSize == 0) { printf("Phonebook is empty.\n"); return; }

    char mobile[MAX_PHONE_LEN];
    printf("Enter mobile number to delete: ");
    fgets(mobile, sizeof(mobile), stdin);
    mobile[strcspn(mobile, "\n")] = '\0';

    int i, j, count = 0;
    for (i = 0; i < currentSize; i++) {
        if (strcmp(phonebook[i].mobileNumber, mobile) == 0) {
            for (j = i; j < currentSize - 1; j++)
                phonebook[j] = phonebook[j + 1];
            currentSize--;
            count++;
            i--;
        }
    }

    if (count == 0) {
        printf("No contact found with that number.\n");
    } else {
        printf("%d contact(s) deleted.\n", count);
        saveToFile();
    }
}

void sortByMobile() {
    if (currentSize == 0) { printf("Phonebook is empty.\n"); return; }
    selectionSort();
    printf("\nContacts sorted by Mobile Number:\n");
    int i;
    for (i = 0; i < currentSize; i++) {
        printf("\n--- Contact %d ---\n", i + 1);
        displayOne(i);
    }
    quickSort(0, currentSize - 1);
}

void knapsackMenu() {
    if (currentSize == 0) { printf("Phonebook is empty.\n"); return; }
    int budget;
    printf("Enter storage budget (number of chars, e.g. 60): ");
    scanf("%d", &budget);
    getchar();
    knapsack(budget);
}

void coinChangeMenu() {
    if (currentSize == 0) { printf("Phonebook is empty.\n"); return; }

    printf("Enter first name of contact: ");
    char key[MAX_NAME_LEN];
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0';

    quickSort(0, currentSize - 1);
    int idx = binarySearch(key);
    if (idx == -1)
        printf("Contact not found.\n");
    else
        coinChange(idx);
}

int main() {
    int choice;

    loadFromFile();

    do {
        printf("\n====== Phonebook Menu ======\n");
        printf("1. Add Contact\n");
        printf("2. Display All         [Quick Sort]\n");
        printf("3. Search by Name      [Binary Search]\n");
        printf("4. Delete Contact\n");
        printf("5. Sort by Mobile      [Selection Sort]\n");
        printf("6. Knapsack Selector   [0/1 Knapsack]\n");
        printf("7. Data Length Units   [Coin Change]\n");
        printf("8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: addContact();    break;
            case 2: displayAll();    break;
            case 3: searchContact(); break;
            case 4: deleteContact(); break;
            case 5: sortByMobile();  break;
            case 6: knapsackMenu();  break;
            case 7: coinChangeMenu(); break;
            case 8: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }

    } while (choice != 8);

    return 0;
}
