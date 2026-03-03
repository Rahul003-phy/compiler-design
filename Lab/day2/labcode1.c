#include <stdio.h>
#include <string.h>

#define NUM_STATES 8
#define NUM_INPUTS 4

// DFA states
enum { D0, D1, D2, D3, D4, D5, D6, DEAD };

// Input mapping: a -> 0, b -> 1, c -> 2 other -> 3
int get_input(char c) {
    if (c == 'a') return 0;
    if (c == 'b') return 1;
    if (c == 'c') return 2;
    return 3; // invalid input
}

// Accepting states with token labels
const char* accepting_tokens[NUM_STATES] = {
    NULL,              // DEAD 0
    NULL,              // DEAD 1
    "a*b",             // D2 ----
    "ca*",             // D3 ---
    NULL,              // DEAD 4
    "a*b",             // D5 ---
    "abc+",            // D6 ---
    NULL               // DEAD
};

int main() {
    // DFA transition table: next_state[current_state][input_symbol]
    int next_state[NUM_STATES][NUM_INPUTS] = {
        /* D0 */ { D1, D2, D3, DEAD },
        /* D1 */ { D4, D5, DEAD, DEAD },
        /* D2 */ { DEAD, DEAD, DEAD, DEAD },
        /* D3 */ { D3, DEAD, DEAD, DEAD },
        /* D4 */ { D4, D2, DEAD, DEAD },
        /* D5 */ { DEAD, DEAD, D6, DEAD },
        /* D6 */ { DEAD, DEAD, D6, DEAD },
        /* DEAD */{ DEAD, DEAD, DEAD }
    };

    char str[100];
    printf("Enter a string: ");
    //scanf("%s", str);
    gets(str);

    int state = D0;
    for (int i = 0; str[i] != '\0'; i++) {
        int input = get_input(str[i]);
        state = next_state[state][input];
        if (state == DEAD) break;
    }

    // Check if the final state is accepting
    if (accepting_tokens[state] != NULL) {
        printf("%s Accepted by DFA: %s\n", str, accepting_tokens[state]);
    } else {
        printf("%s Rejected by all patterns\n", str);
    }

    return 0;
}