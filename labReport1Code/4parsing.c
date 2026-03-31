#include <stdio.h>
#include <string.h>

#define MAXSTACK 200
#define MAXTOK 200
#define MAXSYM 50

// ----------------------
// Grammar Symbols
// ----------------------
char *NT[] = {"S", "E", "Eprime", "T", "Tprime", "F"};
#define NNT 6

char *TERMINALS[] = {"id", "=", "+", "*", "(", ")", "$"};
#define NTER 7

char *RHS[] = {
    "id = E",      // 1. S -> id = E
    "T Eprime",    // 2. E -> T E'
    "+ T Eprime",  // 3. E' -> + T E'
    "",            // 4. E' -> epsilon
    "F Tprime",    // 5. T -> F T'
    "* F Tprime",  // 6. T' -> * F T'
    "",            // 7. T' -> epsilon
    "( E )",       // 8. F -> ( E )
    "id"           // 9. F -> id
};

// ----------------------
// Parsing Table (NT x T -> Production #)
// ----------------------
int TABLE[NNT][NTER] = {
    // id  =   +   * (   )   $
    { 1,  0,  0,  0,  0,  0,  0 }, // S
    { 2,  0,  0,  0,  2,  0,  0 }, // E
    { 0,  0,  3,  0,  0,  4,  4 }, // Eprime
    { 5,  0,  0,  0,  5,  0,  0 }, // T
    { 0,  0,  7,  6,  0,  7,  7 }, // Tprime
    { 9,  0,  0,  0,  8,  0,  0 }  // F
};

// ----------------------
// Stack
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
    printf("[");
    for (int i = 0; i <= top; i++) {
        printf("%s", stack[i]);
        if (i < top) printf(", ");
    }
    printf("]");
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
// Parser
// ----------------------
int main() {
    char line[500];
    printf("Enter input (e.g., id = id + id * id $): ");
    fgets(line, sizeof(line), stdin);

    char input[MAXTOK][MAXSYM];
    int n = tokenize(line, input);

    push("$");
    push("S"); // Start Symbol

    int ip = 0;
    int rejected = 0;

    printf("\n%-20s %-12s %-10s %-20s\n", "Stack", "Lookahead", "Top", "Applied Rule");
    printf("----------------------------------------------------------------------\n");

    while (top >= 0) {
        char X[MAXSYM];
        strcpy(X, pop());
        char *a = input[ip];

        printf("%-20s %-12s %-10s ", "", a, X);

        int tindex = find_t(X);

        // Terminal Match logic
        if (tindex != -1) {
            if (strcmp(X, a) == 0) {
                printf("%-20s\n", "match");
                ip++;
            } else {
                printf("%-20s\n", "error (not match)");
                rejected = 1;
                break;
            }
            continue;
        }

        // Non-Terminal Expansion logic
        int ntindex = find_nt(X);
        int aindex = find_t(a);

        if (ntindex == -1 || aindex == -1) {
            printf("%-20s\n", "error (invalid)");
            rejected = 1;
            break;
        }

        int prod = TABLE[ntindex][aindex];

        if (prod == 0) {
            printf("%-20s\n", "error (no rule)");
            rejected = 1;
            break;
        }

        // Output rule and push RHS in reverse
        if (strlen(RHS[prod - 1]) == 0) {
            printf("%-20s\n", "epsilon");
        } else {
            printf("%s -> %-15s\n", X, RHS[prod - 1]);
            
            char temp[200];
            strcpy(temp, RHS[prod - 1]);
            char *p = strtok(temp, " ");
            char symbols[10][MAXSYM];
            int k = 0;
            while (p) { 
                strcpy(symbols[k++], p); 
                p = strtok(NULL, " "); 
            }
            // Push to stack in reverse order
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

// id = id $
// id = id + id * id $
// id = ( id + id ) * id $
