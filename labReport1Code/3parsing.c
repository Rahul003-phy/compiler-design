#include <stdio.h>
#include <string.h>

#define MAXSTACK 200
#define MAXTOK 200
#define MAXSYM 50

// ----------------------
// Grammar Symbols
// ----------------------
char *NT[] = {"C", "Cprime", "D", "E", "Eprime", "F"};
#define NNT 6

// Terminals
char *TERMINALS[] = {"check", "then", "otherwise", "v", "=", "relop", "n", "+", "(", ")", "$"};
#define NTER 11

char *RHS[] = {
    "check D then C Cprime", // 1. C -> check D then C C'
    "v = E",                 // 2. C -> v = E
    "otherwise C",           // 3. C' -> otherwise C
    "",                      // 4. C' -> epsilon
    "E relop E",             // 5. D -> E relop E
    "F Eprime",              // 6. E -> F E'
    "+ F Eprime",            // 7. E' -> + F E'
    "",                      // 8. E' -> epsilon
    "v",                     // 9. F -> v
    "n",                     // 10. F -> n
    "( E )"                  // 11. F -> ( E )
};

// ----------------------
// Parsing Table
// ----------------------
int TABLE[NNT][NTER] = {
    // chk then othe  v   =  rel  n   +   (   )   $
    { 1,   0,   0,   2,  0,  0,  0,  0,  0,  0,  0 }, // C
    { 0,   0,   3,   0,  0,  0,  0,  0,  0,  4,  4 }, // Cprime
    { 0,   0,   0,   5,  0,  0,  5,  0,  5,  0,  0 }, // D
    { 0,   0,   0,   6,  0,  0,  6,  0,  6,  0,  0 }, // E
    { 0,   8,   8,   0,  0,  8,  0,  7,  0,  8,  8 }, // Eprime
    { 0,   0,   0,   9,  0,  0,  10, 0,  11, 0,  0 }  // F
};

// ----------------------
// Stack
// ----------------------
char stack[MAXSTACK][MAXSYM];
int top = -1;

void push(char *s) { if (top < MAXSTACK - 1) strcpy(stack[++top], s); }
char* pop() { return (top >= 0) ? stack[top--] : NULL; }

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
    for (int i = 0; i < NNT; i++) if (strcmp(NT[i], x) == 0) return i;
    return -1;
}

int find_t(char *x) {
    for (int i = 0; i < NTER; i++) if (strcmp(TERMINALS[i], x) == 0) return i;
    return -1;
}

int tokenize(char *line, char tokens[][MAXSYM]) {
    int n = 0;
    char *p = strtok(line, " \t\n");
    while (p) {
        strcpy(tokens[n++], p);
        p = strtok(NULL, " \t\n");
    }
    if (n == 0 || strcmp(tokens[n - 1], "$") != 0) strcpy(tokens[n++], "$");
    return n;
}

// ----------------------
// Main Parser
// ----------------------
int main() {
    char line[500];
    printf("Example: check v relop n then v = n otherwise v = v $\n");
    printf("Enter input: ");
    fgets(line, sizeof(line), stdin);

    char input[MAXTOK][MAXSYM];
    int n = tokenize(line, input);

    push("$");
    push("C"); // Start symbol

    int ip = 0, rejected = 0;

    printf("\n%-30s %-12s %-12s %-20s\n", "Stack", "Lookahead", "Top", "Rule");
    printf("--------------------------------------------------------------------------------\n");

    while (top >= 0) {
        char X[MAXSYM];
        strcpy(X, pop());
        char *a = input[ip];

        printf("%-30s %-12s %-12s ", "", a, X);

        int tindex = find_t(X);
        if (tindex != -1) {
            if (strcmp(X, a) == 0) {
                printf("match\n");
                ip++;
            } else {
                printf("ERROR (match)\n");
                rejected = 1; break;
            }
            continue;
        }

        int ntindex = find_nt(X);
        int aindex = find_t(a);

        if (ntindex == -1 || aindex == -1) {
            printf("ERROR (invalid)\n");
            rejected = 1; break;
        }

        int prod = TABLE[ntindex][aindex];
        if (prod == 0) {
            printf("ERROR (no rule)\n");
            rejected = 1; break;
        }

        if (strlen(RHS[prod - 1]) == 0) {
            printf("%s -> epsilon\n", X);
        } else {
            printf("%s -> %s\n", X, RHS[prod - 1]);
            char temp[200];
            strcpy(temp, RHS[prod - 1]);
            char *p = strtok(temp, " ");
            char symbols[10][MAXSYM];
            int k = 0;
            while (p) { strcpy(symbols[k++], p); p = strtok(NULL, " "); }
            for (int i = k - 1; i >= 0; i--) push(symbols[i]);
        }
    }

    if (!rejected && ip == n) printf("\nRESULT: ACCEPTED\n");
    else printf("\nRESULT: REJECTED\n");

    return 0;
}

// check v relop n then v = n $
// check v relop v then v = n otherwise v = v $
// check ( v + n ) relop n then check v relop v then v = n + n $