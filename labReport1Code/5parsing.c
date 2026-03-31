#include <stdio.h>
#include <string.h>

#define MAXSTACK 200
#define MAXTOK 200
#define MAXSYM 50

// ----------------------
// Grammar Symbols
// ----------------------
// NTs: S, Sprime, E, Eprime, T
char *NT[] = {"S", "Sprime", "E", "Eprime", "T"};
#define NNT 5

// Terminals: if, then, else, id, num, +, (, ), $
char *TERMINALS[] = {"if", "then", "else", "id", "num", "+", "(", ")", "$"};
#define NTER 9

char *RHS[] = {
    "if E then S Sprime", // 1. S -> if E then S S'
    "else S",             // 2. S' -> else S
    "",                   // 3. S' -> epsilon
    "T Eprime",           // 4. E -> T E'
    "+ T Eprime",         // 5. E' -> + T E'
    "",                   // 6. E' -> epsilon
    "id",                 // 7. T -> id
    "( E )",              // 8. T -> ( E )
    "num"                 // 9. F -> num (Mapped to T for this table)
};

// ----------------------
// Parsing Table (NT x T -> Production #)
// 0 means Error/Empty
// ----------------------
int TABLE[NNT][NTER] = {
    // if, then, else, id, num, +,  (,  ),  $
    { 1,  0,    0,    0,   0,   0,  0,  0,  0 }, // S
    { 0,  0,    2,    0,   0,   0,  0,  3,  3 }, // Sprime
    { 0,  0,    0,    4,   9,   0,  4,  0,  0 }, // E
    { 0,  6,    0,    0,   0,   5,  0,  6,  0 }, // Eprime
    { 0,  0,    0,    7,   9,   0,  8,  0,  0 }  // T
};

// ----------------------
// Stack Implementation
// ----------------------
char stack[MAXSTACK][MAXSYM];
int top = -1;

void push(char *s) { 
    if (top < MAXSTACK - 1) strcpy(stack[++top], s); 
}

char* pop() { 
    return (top >= 0) ? stack[top--] : NULL; 
}

void print_stack() {
    printf("Stack: [");
    for (int i = 0; i <= top; i++) {
        printf("%s", stack[i]);
        if (i < top) printf(", ");
    }
    printf("] ");
}

// ----------------------
// Helpers
// ----------------------
int find_nt(char *x) {
    for (int i = 0; i < NNT; i++)
        if (strcmp(NT[i], x) == 0) return i;
    return -1;
}

int find_t(char *x) {
    for (int i = 0; i < NTER; i++)
        if (strcmp(TERMINALS[i], x) == 0) return i;
    return -1;
}

int tokenize(char *line, char tokens[][MAXSYM]) {
    int n = 0;
    char *p = strtok(line, " \t\n");
    while (p) {
        strcpy(tokens[n++], p);
        p = strtok(NULL, " \t\n");
    }
    if (n == 0 || strcmp(tokens[n - 1], "$") != 0)
        strcpy(tokens[n++], "$");
    return n;
}

// ----------------------
// Parser Main
// ----------------------
int main() {
    char line[500];
    printf("Enter input (space separated, e.g., 'if id then id else id $'):\n> ");
    fgets(line, sizeof(line), stdin);

    char input[MAXTOK][MAXSYM];
    int n = tokenize(line, input);

    push("$");
    push("S"); // Start symbol changed to S

    int ip = 0;
    int rejected = 0;

    printf("\n%-25s %-10s %-10s %-25s\n", "Stack State", "Lookahead", "Top", "Action");
    printf("-----------------------------------------------------------------------------\n");

    while (top >= 0) {
        char X[MAXSYM];
        strcpy(X, pop());
        char *a = input[ip];

        // Print current state
        print_stack();
        printf(" | L: %-8s | Top: %-6s ", a, X);

        int tindex = find_t(X);

        // Case 1: Top is a terminal
        if (tindex != -1) {
            if (strcmp(X, a) == 0) {
                printf("-> Match '%s'\n", a);
                ip++;
            } else {
                printf("-> Error: Match Failed\n");
                rejected = 1;
                break;
            }
            continue;
        }

        // Case 2: Top is a Non-Terminal
        int ntindex = find_nt(X);
        int aindex = find_t(a);

        if (ntindex == -1 || aindex == -1) {
            printf("-> Error: Invalid Symbol\n");
            rejected = 1;
            break;
        }

        int prod = TABLE[ntindex][aindex];

        if (prod == 0) {
            printf("-> Error: No Rule in Table\n");
            rejected = 1;
            break;
        }

        // Apply production
        if (strlen(RHS[prod - 1]) == 0) {
            printf("-> Apply: %s -> epsilon\n", X);
        } else {
            printf("-> Apply: %s -> %s\n", X, RHS[prod - 1]);
            
            // Push RHS to stack in reverse order
            char temp[200];
            strcpy(temp, RHS[prod - 1]);
            char *p = strtok(temp, " ");
            char symbols[10][MAXSYM];
            int k = 0;
            while (p) { 
                strcpy(symbols[k++], p); 
                p = strtok(NULL, " "); 
            }
            for (int i = k - 1; i >= 0; i--) {
                push(symbols[i]);
            }
        }
    }

    if (!rejected && ip == n)
        printf("\nRESULT: ACCEPTED\n");
    else
        printf("\nRESULT: REJECTED\n");

    return 0;
}

// if id then id $
// if id then id else id $
// if ( id + num ) then if id then id else num $