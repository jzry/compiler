// Virtual machine representation PM/0.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PAS_LENGTH 500
#define DEBUG 0

// Print the array.
void printArray(int *pas, int size);

// Initialize a dynamic array.
int *init(int size);

// Represents components for the instruction to be executed.
typedef struct ir
{
    // Operation to be executed (Operation code).
    int OP;

    // Lexicographical level.
    int L;

    // M Can represent:
    // - A number (instructions: LIT, INC).
    // - A program address (instructions: JMP, JPC, CAL).
    // - A data address (instructions: LOD, STO)
    // - The identity of the operator OPR (e.g. OPR 0 2 (ADD) or OPR 0 4 (MUL))
    int M;
} ir;

// This function will be helpful to find a variable
// in a different Activation Record (AR) some L levels down:
// Find base L levels down.
// For example in the instruction:
// STO L, M  - You can do stack [base (IR.L) -  IR.M] = pas[SP]
// to store the content of  the top of the stack into an AR
// in the stack,  located L levels down from the current AR.
int base(int L, int *pas, int BP)
{
    // arb = activation record base
    int arb = BP;

    // Find base L levels down
    while (L > 0)
    {
        arb = pas[arb];
        L--;
    }
    return arb;
}

// Print output properly.
void print_execution(int line, char *opname, ir *IR, int PC,
                     int BP, int SP, int DP, int *pas, int GP)
{
    int i;

    // Print out instruction and registers.
    printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t",
           line, opname, IR->L, IR->M, PC, BP, SP, DP);

    // Print data section.
    for (i = GP; i <= DP; i++)
    {
        printf("%d ", pas[i]);
    }
    printf("\n");

    // Print stack.
    printf("\tstack : ");
    for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
    {
        printf("%d ", pas[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    FILE *ifp;
    char *opname;
    int i, counter = 0, op = 0, FLAG = 1, *pas, line = 0, IC = 0, num, k;
    ir *IR;

    // Global CPU variables.
    int BP, SP, PC, DP, GP, FREE;

    if (argc != 2)
    {
        fprintf(stderr, "Syntax: %s <input>\n", argv[0]);
        return 1;
    }

    // File doesn't exist.
    if ((ifp = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "ERROR OPENING: (%s)\n", argv[1]);
        return 1;
    }

    // Initialize all process address space to 0.
    pas = init(MAX_PAS_LENGTH);

    // Scan file and put each character in the array.
    while ((fscanf(ifp, "%d", &num)) != EOF)
    {
        // Read instructions from file into the process address space.
        pas[counter] = num;

        counter++;

        // Only increment the IC every instruction.
        // Once instruction is 3 array cells: <OP L M>
        if (counter % 3 == 0)
        {
            IC = IC + 3;
        }
    }

    // Dynamically allocate the instruction register.
    IR = calloc(1, sizeof(ir));

    // Dynamically allocate 3 letters for opcode + 1 space for \0.
    opname = malloc(sizeof(char) * 4);

    // Global CPU variables.
    // bp - base pointer
    // sp - stack pointer
    // pc - program counter
    // dp - data pointer
    // gp - global data pointer
    // FREE - heap pointer
    // ic - instruction counter
    // ir - instruction register (as a struct)

    // Program counter begins with 0.
    PC = 0;

    // Global Pointer – Points to DATA segment.
    GP = IC;

    // Data Pointer – To access variables in Main.
    DP = IC - 1;

    // FREE points to Heap.
    FREE = IC + 40;

    // Points to base of DATA or activation records.
    BP = IC;

    // Stack pointer – Points to top of stack.
    SP = MAX_PAS_LENGTH;

    // Display pre-compilation values.
    printf("\t\t\t\tPC\tBP\tSP\tDP\tdata\n");
    printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, DP);

    // Instruction Set Architecture.
    while (FLAG)
    {
        if (DEBUG == 1)
        {
            printArray(pas, MAX_PAS_LENGTH);
        }

        // Fetch instruction.
        IR->OP = pas[PC];
        IR->L = pas[PC + 1];
        IR->M = pas[PC + 2];

        // Increment program counter by 3.
        line = PC / 3;
        PC = PC + 3;

        // Execute instruction.
        switch (IR->OP)
        {
            // LIT 0, M. Pushes a constant value (literal).
            // M onto the stack (or DATA).
            case 1:
                if (BP == GP)
                {
                    // Increment the data pointer.
                    DP = DP + 1;

                    // Data pointer points to current instruction register data.
                    pas[DP] = IR->M;
                }
                else
                {
                    // Decrement the stack pointer.
                    SP = SP - 1;

                    // Stack pointer points to current instruction register data.
                    pas[SP] = IR->M;
                }

                // Set up the opname.
                opname[0] = 'L'; opname[1] = 'I'; opname[2] = 'T'; opname[3] = '\0';
                break;

            // OPR 0, #. Operation to be performed on the
            // data at the top of the stack (or in data segment).
            case 2:
                if (IR->M == 0) // RTN
                {
                    SP = BP + 1;
                    BP = pas[SP - 2];
                    PC = pas[SP - 3];

                    // Set up the opname.
                    opname[0] = 'R'; opname[1] = 'T'; opname[2] = 'N'; opname[3] = '\0';
                }
                else if (IR->M == 1) // NEG
                {
                    if (BP == GP)
                    {
                        pas[DP] = (-1 * pas[DP]);
                    }
                    else
                    {
                        pas[SP] = (-1 * pas[SP]);
                    }

                    // Set up the opname.
                    opname[0] = 'N'; opname[1] = 'E'; opname[2] = 'G'; opname[3] = '\0';
                }
                else if (IR->M == 2) // ADD
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] + pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] + pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'A'; opname[1] = 'D'; opname[2] = 'D'; opname[3] = '\0';
                }
                else if (IR->M == 3) // SUB
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] - pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] - pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'S'; opname[1] = 'U'; opname[2] = 'B'; opname[3] = '\0';
                }
                else if (IR->M == 4) // MUL
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] * pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] * pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'M'; opname[1] = 'U'; opname[2] = 'L'; opname[3] = '\0';
                }
                else if (IR->M == 5) // DIV
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] / pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] / pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'D'; opname[1] = 'I'; opname[2] = 'V'; opname[3] = '\0';
                }
                else if (IR->M == 6) // ODD
                {
                    if (BP == GP)
                    {
                        pas[DP] = (pas[DP] % 2);
                    }
                    else
                    {
                        pas[SP] = (pas[SP] % 2);
                    }

                    // Set up the opname.
                    opname[0] = 'O'; opname[1] = 'D'; opname[2] = 'D'; opname[3] = '\0';
                }
                else if (IR->M == 7) // MOD
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] % pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] % pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'M'; opname[1] = 'O'; opname[2] = 'D'; opname[3] = '\0';
                }
                else if (IR->M == 8) // EQL
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] == pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] == pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'E'; opname[1] = 'Q'; opname[2] = 'L'; opname[3] = '\0';
                }
                else if (IR->M == 9) // NEQ
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] != pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] != pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'N'; opname[1] = 'E'; opname[2] = 'Q'; opname[3] = '\0';
                }
                else if (IR->M == 10) // LSS
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] < pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] < pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'L'; opname[1] = 'S'; opname[2] = 'S'; opname[3] = '\0';
                }
                else if (IR->M == 11) // LEQ
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] <= pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] <= pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'L'; opname[1] = 'E'; opname[2] = 'Q'; opname[3] = '\0';
                }
                else if (IR->M == 12) // GTR
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] > pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] > pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'G'; opname[1] = 'T'; opname[2] = 'R'; opname[3] = '\0';
                }
                else if (IR->M == 13) // GEQ
                {
                    if (BP == GP)
                    {
                        DP = DP - 1;
                        pas[DP] = (pas[DP] >= pas[DP + 1]);
                    }
                    else
                    {
                        SP = SP + 1;
                        pas[SP] = (pas[SP] >= pas[SP - 1]);
                    }

                    // Set up the opname.
                    opname[0] = 'G'; opname[1] = 'E'; opname[2] = 'Q'; opname[3] = '\0';
                }
                else
                {
                    printf("Unrecognized IR->M value in main()\n");
                }
                break;

            // LOD L, M. Load value to top of stack from
            // the stack location at offset M from L lexicographical levels down.
            // (Or load a value into the data segment).
            case 3:
                if (BP  == GP)
                {
                    DP = DP + 1;
                    pas[DP] = pas[GP + IR->M];
                }
                else
                {
                    if (base(IR->L, pas, BP) == GP)
                    {
                        SP = SP - 1;
                        pas[SP] = pas[GP + IR->M];
                    }
                    else
                    {
                        SP = SP - 1;
                        pas[SP] = pas[base(IR->L, pas, BP) - IR->M];
                    }
                }

                // Set up the opname.
                opname[0] = 'L'; opname[1] = 'O'; opname[2] = 'D'; opname[3] = '\0';
                break;

            // STO L, M. Load value to top of stack from
            // the stack location at offset M from L lexicographical levels down.
            // (Or store a value into the data segment)
            case 4:
                if (BP == GP)
                {
                    pas[GP + IR->M] = pas[DP];
                    DP = DP - 1;
                }
                else
                {
                    if (base(IR->L, pas, BP) == GP)
                    {
                        pas[GP + IR->M] = pas[SP];
                        SP = SP + 1;
                    }
                    else
                    {
                        pas[base(IR->L, pas, BP) - IR->M] = pas[SP];
                        SP = SP + 1;
                    }
                }

                // Set up the opname.
                opname[0] = 'S'; opname[1] = 'T'; opname[2] = 'O'; opname[3] = '\0';
                break;

            // CAL L, M. Call procedure at code index M
            // (generates new Activation Record and PC = M)
            case 5:
                pas[SP - 1]  =  base(IR->L, pas, BP); // Static link (SL)
                pas[SP - 2]  = BP; // Dynamic link (DL)
                pas[SP - 3]  = PC; // Return address (RA)
                BP = SP - 1;
                PC = IR->M;

                // Set up the opname.
                opname[0] = 'C'; opname[1] = 'A'; opname[2] = 'L'; opname[3] = '\0';
                break;

            // INC 0, M. Allocate M memory words (increment SP by M).
            // (Or increment dp)
            case 6:
                if (BP == GP)
                {
                    DP = (DP + IR->M);
                }
                else
                {
                    SP = (SP - IR->M);
                }

                // Set up the opname.
                opname[0] = 'I'; opname[1] = 'N'; opname[2] = 'C'; opname[3] = '\0';
                break;

            // JMP 0, M. Jump to instruction M (PC = M).
            case 7:
                PC = IR->M;

                // Set up the opname.
                opname[0] = 'J'; opname[1] = 'M'; opname[2] = 'P'; opname[3] = '\0';
                break;

            // JPC 0, M. Jump to instruction M if top stack or data element is 0.
            case 8:
                if (BP == GP)
                {
                    if (pas[DP] == 0)
                    {
                        PC = IR->M;
                    }
                    DP = DP - 1;
                }
                else
                {
                    if (pas[SP] == 0)
                    {
                        PC = IR->M;
                    }
                    SP = SP + 1;
                }

                // Set up the opname.
                opname[0] = 'J'; opname[1] = 'P'; opname[2] = 'C'; opname[3] = '\0';
                break;

            // SYS  0, 1. Write system call.
            case 9:

                // Write the top stack element or data element to the screen.
                if (IR->M == 1)
                {
                    printf("Top of Stack Value: ");
                    if (BP == GP)
                    {
                        printf("%d\n", pas[DP]);
                        DP = DP - 1;
                    }
                    else
                    {
                        printf("%d\n", pas[SP]);
                        SP = SP + 1;
                    }
                }

                // Read in input from the user and store it on top of the stack.
                // (or store it in data section)
                else if (IR->M == 2)
                {
                    if (BP == GP)
                    {
                        DP = DP + 1;
                        printf("Please Enter an Integer: ");
                        scanf("%d", &pas[DP]);
                    }
                    else
                    {
                        SP = SP - 1;
                        printf("Please Enter an Integer: ");
                        scanf("%d", &pas[SP]);
                    }
                }
                else // if (IR->M == 3)
                {
                    // End of program, break out of loop.
                    FLAG = 0;
                }
//                else
//                {
//                    printf("Broken system call.\n");
//                }

                // Set up the opname.
                opname[0] = 'S'; opname[1] = 'Y'; opname[2] = 'S'; opname[3] = '\0';
                break;

            default:
                printf("Instruction operation doesn't work");
                break;
        }

        // Print CPU register information.
        print_execution(line, opname, IR, PC, BP, SP, DP, pas, GP);
    }

    // ========================================
    // END OF GIVEN
    // ========================================

    // Free memory for pas.
    if (pas != NULL)
    {
        free(pas);
    }

    // Free memory for IR.
    if (IR != NULL)
    {
        free(IR);
    }

    // Free opname for IR.
    if (opname != NULL)
    {
        free(opname);
    }

    // Close the input file.
    fclose(ifp);

    return 0;
}

// Print the array.
void printArray(int *pas, int size)
{
    int i;

    printf("In printArray():\n");
    for (i = 0; i < 90; i++)
    {
        printf("%d%c", pas[i], (((i + 1) % 3 == 0) && (i != 0)) ? '\n' : ' ');
    }
}

// Initialize a dynamic array.
int *init(int size)
{
    int *arr, i;

    arr = malloc(sizeof(int) * size);

    // Default set to 0.
    for (i = 0; i < size; i++)
    {
        arr[i] = 0;
    }

    return arr;
}
