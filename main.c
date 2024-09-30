#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SYMBOLS 30
#define MAX_STATES 50
#define MAX_TAPE 100
#define MAX_TRANSITIONS 100


// example: 1 a x D 2
typedef struct {
    int current_state;
    char read_symbol;
    char recorded_symbol;
    char direction;
    int next_state; 
} Transition;


// expected structure to be received in the file
typedef struct {
    char alphabet[MAX_SYMBOLS];
    int acceptance_state;
    int numof_transitions;
    Transition transitions[MAX_TRANSITIONS];
    int numof_words;
    char tape[MAX_TAPE][MAX_TAPE];
} TuringVariables;


// extract all variables according to the expected structure of the inserted file
TuringVariables get_input_file(char *file_name) {
    TuringVariables variables;
    FILE *file = fopen(file_name, "r");

    if (!file) {
        printf("Can not open the file\n");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%s", variables.alphabet);
    fscanf(file, "%d", &variables.acceptance_state);
    fscanf(file, "%d", &variables.numof_transitions);

    for (int i = 0; i < variables.numof_transitions; i++) {
        fscanf(file, "%d %c %c %c %d",
                &variables.transitions[i].current_state,
                &variables.transitions[i].read_symbol,
                &variables.transitions[i].recorded_symbol,
                &variables.transitions[i].direction,
                &variables.transitions[i].next_state);
    }

    fscanf(file, "%d", &variables.numof_words);

    for (int i = 0; i < variables.numof_words; i++) {
        fscanf(file, "%s", variables.tape[i]);
    }

    fclose(file);

    return variables;
}


// print all extracted variables as a test that the information was extracted as it should
void print_input_variables(TuringVariables input_variables) {
    printf("Alphabet: %s\n", input_variables.alphabet);   
    printf("Acceptance state: %d\n", input_variables.acceptance_state);
    printf("Number of transitions: %d\n", input_variables.numof_transitions);
    
    for (int i = 0; i < input_variables.numof_transitions; i++) {
        printf("Transition %d: \n", i + 1);
        printf("  Current state: %d\n", input_variables.transitions[i].current_state);
        printf("  Read symbol: %c\n", input_variables.transitions[i].read_symbol);
        printf("  Recorded symbol: %c\n", input_variables.transitions[i].recorded_symbol);
        printf("  Direction: %c\n", input_variables.transitions[i].direction);
        printf("  Next state: %d\n", input_variables.transitions[i].next_state);
    }

    printf("Number of words: %d\n", input_variables.numof_words);
    
    for (int i = 0; i < input_variables.numof_words; i++) {
        printf("Word %d: %s\n", i + 1, input_variables.tape[i]);
    }
}


// function that simulates the Turing Machine for a word using the transitions informed in the file
bool turing_machine(TuringVariables variables, int word_index) {
    // place the symbol - to mark the first blank space at the end of the word
    char whitespace[] = "-";
    strcat(variables.tape[word_index], whitespace);
    
    bool checking = true;
    int cursor = 0;
    int state = 1;

    while (checking) {
        char symbol = variables.tape[word_index][cursor];

        bool transition_found = false;
        for (int i = 0; i < variables.numof_transitions; i++) {
            if (variables.transitions[i].current_state == state
            && variables.transitions[i].read_symbol == symbol) {
                // record symbol
                variables.tape[word_index][cursor] = variables.transitions[i].recorded_symbol;
                // set next state
                state = variables.transitions[i].next_state;
                // move cursor
                if (variables.transitions[i].direction == 'D') {
                    cursor++;
                } else if (variables.transitions[i].direction == 'E') {
                    cursor--;
                }

                transition_found = true;
                // stop search because transition rule was found
                break;
            }
        }

        // check if it has reached acceptance status
        if (state == variables.acceptance_state) {
            return true;
        }

        // if it finds no transition to the current state and symbol, reject the word
        if (!transition_found) {
            return false;
        }
    }
    
    return false;
}


int main (int argc, char* argv[]) {
    if (argc != 2) {
        printf("expected command: <program_name> <input_file_name>\n");
        exit(EXIT_FAILURE);
    }

    TuringVariables input_variables = get_input_file(argv[1]); 
    //print_input_variables(input_variables);
    
    for (int i = 0; i < input_variables.numof_words; i++) {
        bool acceptance = turing_machine(input_variables, i);
        printf("%d:\t%s\t%s\n", (i + 1), input_variables.tape[i], acceptance ? "OK" : "not OK");
    }

    exit(EXIT_SUCCESS);
}
