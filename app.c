#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Terminal colors
#define GREEN "\033[42;30m"
#define YELLOW "\033[43;30m"
#define GRAY "\033[47;30m"
#define RESET "\033[0m"

// Limits
#define MAX_WORDS 100
#define WORD_LEN 6 // 5 letters + '\0'
#define ROWS 6
#define COLS 5
#define CELL_SIZE 20

// Globals
char wordList[MAX_WORDS][WORD_LEN];
char board[ROWS][COLS][CELL_SIZE];
int totalWords = 0;

// Prototypes
int isValid(char guess[]);
void printDaBoard(void);
void clearScreen(void);

int main(int argc, char *argv[])
{
    /* ---------- Initialize board ---------- */
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            board[i][j][0] = '\0';
        }
    }

    /* ---------- Load dictionary ---------- */
    FILE *file = fopen("words.txt", "r");
    if (!file)
    {
        perror("Failed to open words.txt");
        return 1;
    }

    char buffer[10];
    while (totalWords < MAX_WORDS && fgets(buffer, sizeof(buffer), file))
    {
        // Remove newline/carriage return
        buffer[strcspn(buffer, "\r\n")] = '\0';

        // Only keep 5-letter words
        if (strlen(buffer) == 5)
        {
            // Convert to uppercase and store
            for (int i = 0; i < 5; i++)
            {
                wordList[totalWords][i] = toupper(buffer[i]);
            }
            wordList[totalWords][5] = '\0';
            totalWords++;
        }
    }
    fclose(file);

    if (totalWords == 0)
    {
        printf("No words loaded!\n");
        return 1;
    }

    /* ---------- Pick secret ---------- */
    srand(time(NULL));
    char secret[WORD_LEN];
    strcpy(secret, wordList[rand() % totalWords]);

    int attempts = ROWS;
    int currentRow = 0;

    /* ---------- Game loop ---------- */
    while (attempts--)
    {
        char guess[20];

        printf("\nAttempt %d → Enter 5-letter word: ", currentRow + 1);

        if (scanf("%19s", guess) != 1)
        {
            printf("\nInvalid input!\n");
            break;
        }

        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;

        // Convert to uppercase
        for (int i = 0; guess[i] != '\0'; i++)
            guess[i] = toupper(guess[i]);

        if (strlen(guess) != 5)
        {
            printf("\nWord must be exactly 5 letters!\n");
            attempts++;
            continue;
        }

        if (!isValid(guess))
        {
            printf("\nWord not in the list..\n");
            attempts++;
            continue;
        }

        // Coloring logic
        int freq[26] = {0};
        char result[5][CELL_SIZE];

        // Count letters in secret
        for (int i = 0; i < 5; i++)
            freq[secret[i] - 'A']++;

        // GREEN pass - exact matches
        for (int i = 0; i < 5; i++)
        {
            if (guess[i] == secret[i])
            {
                sprintf(result[i], GREEN "   %c   " RESET, guess[i]);
                freq[guess[i] - 'A']--;
            }
            else
            {
                result[i][0] = '\0'; // unresolved
            }
        }

        // YELLOW / GRAY pass
        for (int i = 0; i < 5; i++)
        {
            if (result[i][0] != '\0')
                continue;

            int idx = guess[i] - 'A';
            if (idx >= 0 && idx < 26 && freq[idx] > 0)
            {
                sprintf(result[i], YELLOW "   %c   " RESET, guess[i]);
                freq[idx]--;
            }
            else
            {
                sprintf(result[i], GRAY "   %c   " RESET, guess[i]);
            }
        }

        // Store row in board
        for (int i = 0; i < 5; i++)
        {
            strcpy(board[currentRow][i], result[i]);
        }

        currentRow++;
        clearScreen();
        printDaBoard();

        if (strcmp(guess, secret) == 0)
        {
            printf("\nCorrect! You solved the Wordle!\n");
            return 0;
        }
    }

    printf("\nOut of attempts! Secret word was: %s\n", secret);
    return 0;
}

/* ---------- Dictionary check ---------- */
int isValid(char guess[])
{
    for (int i = 0; i < totalWords; i++)
    {
        if (strcmp(guess, wordList[i]) == 0)
            return 1;
    }
    return 0;
}

/* ---------- Clear screen ---------- */
void clearScreen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* ---------- Board rendering ---------- */
void printDaBoard(void)
{

    printf("\n");

    for (int i = 0; i < ROWS; i++)
    {
        // Top padding line
        for (int j = 0; j < COLS; j++)
        {
            if (board[i][j][0] != '\0')
            {
                if (strstr(board[i][j], GREEN))
                    printf(" " GREEN "       " RESET " ");
                else if (strstr(board[i][j], YELLOW))
                    printf(" " YELLOW "       " RESET " ");
                else
                    printf(" " GRAY "       " RESET " ");
            }
            else
                printf(" " GRAY "       " RESET " ");
        }
        printf("\n");

        // Main content line with letter
        for (int j = 0; j < COLS; j++)
        {
            if (board[i][j][0] != '\0')
                printf(" %s ", board[i][j]);
            else
                printf(" " GRAY "       " RESET " ");
        }
        printf("\n");

        // Bottom padding line
        for (int j = 0; j < COLS; j++)
        {
            if (board[i][j][0] != '\0')
            {
                if (strstr(board[i][j], GREEN))
                    printf(" " GREEN "       " RESET " ");
                else if (strstr(board[i][j], YELLOW))
                    printf(" " YELLOW "       " RESET " ");
                else
                    printf(" " GRAY "       " RESET " ");
            }
            else
                printf(" " GRAY "       " RESET " ");
        }
        printf("\n");

        // Empty spacing line
        printf("\n");
    }
}
