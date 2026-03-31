#include <stdio.h>
#include <string.h>

#define NUM_STATES 8
#define NUM_INPUTS 4

enum { D0, D1, D2, D3, D4, D5, D6, DEAD };

int get_input(char c){
    if(c=='a') return 0;
    if(c=='b') return 1;
    if(c=='c') return 2;
    return 3;
}

const char* accepting_tokens[NUM_STATES] = {
    NULL,      // D0
    NULL,      // D1
    NULL,      // D2
    "b*c",     // D3
    "a+b",     // D4
    "b*c",     // D5
    "ca+",     // D6
    NULL       // DEAD
};

int main(){

    int next_state[NUM_STATES][NUM_INPUTS] = {

      /* D0 */ { D1, D2, D3, DEAD },
      /* D1 */ { D1, D4, DEAD, DEAD },
      /* D2 */ { DEAD, D2, D5, DEAD },
      /* D3 */ { D6, DEAD, DEAD, DEAD },
      /* D4 */ { DEAD, DEAD, DEAD, DEAD },
      /* D5 */ { DEAD, DEAD, DEAD, DEAD },
      /* D6 */ { D6, DEAD, DEAD, DEAD },
      /* DEAD */{ DEAD, DEAD, DEAD, DEAD }
    };

    char str[100];
    printf("Enter string: ");
    fgets(str,sizeof(str),stdin);
    str[strcspn(str,"\n")] = 0;

    int state = D0;

    for(int i=0; str[i]!='\0'; i++){
        int input = get_input(str[i]);
        state = next_state[state][input];
        if(state==DEAD) break;
    }

    if(accepting_tokens[state])
        printf("%s Accepted (%s)\n",str,accepting_tokens[state]);
    else
        printf("%s Rejected\n",str);

    return 0;
}

// PROGRAM 2
// DFA for: a+b | b*c | ca+
// Accepted patterns
// one or more a then b
// zero or more b then c
// c followed by one or more a

// | State | a    | b    | c    | Final |
// | ----- | ---- | ---- | ---- | ----- |
// | D0    | D1   | D2   | D3   | ❌    |
// | D1    | D1   | D4   | Dead | ❌    |
// | D2    | Dead | D2   | D5   | ❌    |
// | D3    | D6   | Dead | Dead | ✅    |
// | D4    | Dead | Dead | Dead | ✅    |
// | D5    | Dead | Dead | Dead | ✅    |
// | D6    | D6   | Dead | Dead | ✅    |
// | Dead  | Dead | Dead | Dead | ❌    |