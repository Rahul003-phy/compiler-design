#include <stdio.h>
#include <string.h>

#define MAXSTACK 200
#define MAXTOK 200
#define MAXSYM 50

// ----------------------
// Grammar Symbols
// ----------------------
char *NT[] = {"E","Eprime","T","Tprime","F","V"};
#define NNT 6

char *TERMINALS[] = {"id","num","|","&","~","(",")","$"};
#define NTER 8

char *RHS[] = {
    "T Eprime",        //1 E -> T E'
    "| T Eprime",      //2 E' -> | T E'
    "",                //3 E' -> epsilon
    "F Tprime",        //4 T -> F T'
    "& F Tprime",      //5 T' -> & F T'
    "",                //6 T' -> epsilon
    "~ F",             //7 F -> ~ F
    "V",               //8 F -> V
    "id",              //9 V -> id
    "num",             //10 V -> num
    "( E )"            //11 V -> ( E )
};

// ----------------------
// Parsing Table (NT x T -> Production #)
// ----------------------
int TABLE[NNT][NTER] = {
    {1, 1, 0, 0, 1, 1, 0, 0},
    {0, 0, 2, 0, 0, 0, 3, 3},
    {4, 4, 0, 0, 4, 4, 0, 0},
    {0, 0, 6, 5, 0, 0, 6, 6},
    {8, 8, 0, 0, 7, 8, 0, 0},
    {9, 10, 0, 0, 0, 11, 0, 0}
};

// ----------------------
// Stack
// ----------------------
char stack[MAXSTACK][MAXSYM];
int top = -1;

void push(char *s){ strcpy(stack[++top], s); }
char* pop(){ return (top>=0)?stack[top--]:NULL; }

void print_stack(){
    printf("[");
    for(int i=top;i>=0;i--){
        printf("%s",stack[i]);
        if(i>0) printf(", ");
    }
    printf("]");
}

// ----------------------
// Helpers
// ----------------------
int find_nt(char *x){
    for(int i=0;i<NNT;i++)
        if(strcmp(NT[i],x)==0) return i;
    return -1;
}

int find_t(char *x){
    for(int i=0;i<NTER;i++)
        if(strcmp(TERMINALS[i],x)==0) return i;
    return -1;
}

int tokenize(char *line, char tokens[][MAXSYM]){
    int n=0;
    char *p=strtok(line," \t\n");
    while(p){
        strcpy(tokens[n++],p);
        p=strtok(NULL," \t\n");
    }
    if(n==0 || strcmp(tokens[n-1],"$")!=0)
        strcpy(tokens[n++],"$");
    return n;
}

// ----------------------
// Parser
// ----------------------
int main(){
    char line[500];
    printf("Enter input : ");
    fgets(line,sizeof(line),stdin);

    char input[MAXTOK][MAXSYM];
    int n = tokenize(line,input);

    push("$");
    push("E"); // start symbol

    int ip=0;
    int rejected = 0;  // flag for rejection

    printf("%-25s %-10s %-10s %-25s\n","Stack","Lookahead","Top","Production Applied");
    printf("-------------------------------------------------------------------\n");

    while(top>=0){
        char X[MAXSYM];
        strcpy(X,pop());

        char *a = input[ip];

        printf("%-25s %-10s %-10s ","", a, X);

        int tindex = find_t(X);

        // Terminal
        if(tindex!=-1){
            if(strcmp(X,a)==0){
                printf("%-25s\n","match");
                ip++;
            } else {
                printf("%-25s\n","not match");
                rejected = 1;
                break;   // stop parsing
            }
            print_stack(); printf("\n");
            continue;
        }

        int ntindex = find_nt(X);
        int aindex = find_t(a);

        if(ntindex==-1 || aindex==-1){
            printf("%-25s\n","not match");
            rejected = 1;
            break;
        }

        int prod = TABLE[ntindex][aindex];

        if(prod==0){
            printf("%-25s\n","not match");
            rejected = 1;
            break;
        }

        if(strlen(RHS[prod-1])==0)
            printf("%-25s\n","epsilon");
        else
            printf("%-25s\n",RHS[prod-1]);

        // push RHS in reverse
        if(strlen(RHS[prod-1])>0){
            char temp[200];
            strcpy(temp,RHS[prod-1]);
            char *p = strtok(temp," ");
            char symbols[10][MAXSYM];
            int k=0;
            while(p){ strcpy(symbols[k++],p); p=strtok(NULL," "); }
            for(int i=k-1;i>=0;i--) push(symbols[i]);
        }

        print_stack(); printf("\n");
    }

    if(rejected) printf("\nREJECTED\n");
    else if(top==-1 && ip==n)
        printf("\nACCEPTED\n");
    else
        printf("\nREJECTED\n");

    return 0;
}