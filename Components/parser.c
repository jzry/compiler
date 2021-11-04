// Homework 4 - Compiler parser / intermediate code generator.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

#define UNMARKED 0
#define MARKED 1
#define NOT_FOUND

instruction *code; // OP, L, M.
symbol *table; // Symbol table struct array.
int cIndex; // Code segment Index (Instruction Storage).
int tIndex; // Symbol table array index.
int lIndex; // Lexical level index.
int level; // Lexicographic level.
int ERROR_FLAG = INT_MIN; // If true, breaks out of all processes.

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

// Parser functional prototypes.
int var_dec(lexeme *list);
int mult_dec_check(lexeme *list);
int find_sym(lexeme *list, int kind);
void program(lexeme *list);
void block(lexeme *list);
void const_dec(lexeme *list);
void proc_dec(lexeme *list);
void statement(lexeme *list);
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);
void mark(void);

//typedef struct lexeme {
//    char name[12];
//    int value;
//    token_type type;
//} lexeme;
//
//typedef struct instruction {
//    int opcode;
//    int l;
//    int m;
//} instruction;
//
//typedef struct
//{
//    int kind;         // const = 1, var = 2
//    char name[12];    // name up to 11 chars
//    int val;         // number
//    int level;         // L level
//    int addr;         // M address
//    int mark;
//} symbol;

// =================================================================================================
// =================================================================================================
// Finished code.
// =================================================================================================
// =================================================================================================

// Code still needs refactoring.
void factor(lexeme *list)
{
    int symIdx_var, symIdx_const;

    if (ERROR_FLAG == 1)
    {
        return;
    }

    if (list[lIndex].type == identsym)
    {
        // System index variable is result if kind 2 is found in the lexeme token list.
        symIdx_var = find_sym(list, 2);

        // System index variable is result if kind 1 is found in the lexeme token list.
        symIdx_const = find_sym(list, 1);

        // Neither variable nor constant are found in token list.
        if (symIdx_var == -1 && symIdx_const == -1)
        {
            if (find_sym(list, 3) != -1)
            {
                printparseerror(11);
                ERROR_FLAG = 1;
                return;
            }
            else
            {
                printparseerror(19);
                ERROR_FLAG = 1;
                return;
            }
        }

        // Only variable isn't found in token list.
        if (symIdx_var == -1)
        {
            // Add an instruction to the code array and increment the code index.
            emit(1, 0, table[symIdx_const].val); // LIT instruction
        }
        else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level)
        {
            // Add an instruction to the code array and increment the code index.
            emit(3, (level - table[symIdx_var].level), table[symIdx_var].addr); // LOD instruction
        }
        else
        {
            // Add an instruction to the code array and increment the code index.
            emit(1, 0, table[symIdx_const].val); // LIT instruction
        }

        // Get next token.
        lIndex++;
    }
    else if (list[lIndex].type == numbersym)
    {
        // Add an instruction to the code array and increment the code index.
        emit(1, 0, list[lIndex].value); // LIT instruction

        // Get next token.
        lIndex++;
    }
    else if (list[lIndex].type == lparensym)
    {
        // Get next token.
        lIndex++;

        // Generate expression from lexeme list.
        expression(list);

        if (list[lIndex].type != rparensym)
        {
            printparseerror(12);
            ERROR_FLAG = 1;
            return;
        }

        // Get next token.
        lIndex++;
    }
    else
    {
        printparseerror(11);
        ERROR_FLAG = 1;
        return;
    }

    return;
}


// Handles arithmetic symbols.
void term(lexeme *list)
{
    if (ERROR_FLAG == 1)
    {
        return;
    }

    // Factor lexeme token list.
    factor(list);

    // Token is an arithmetic symbol.
    while (list[lIndex].type == multsym ||
           list[lIndex].type == divsym ||
           list[lIndex].type == modsym)
    {
        if (list[lIndex].type == multsym)
        {
            // Get next token.
            lIndex++;

            // Factor lexeme token list.
            factor(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 4); // MUL instruction
        }
        else if (list[lIndex].type == divsym)
        {
            // Get next token.
            lIndex++;

            // Factor lexeme token list.
            factor(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 5); // DIV instruction
        }
        else
        {
            // Get next token.
            lIndex++;

            // Factor lexeme token list.
            factor(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 7); // MOD instruction
        }
    }
}

// Generate expression statements from a lexeme token list.
void expression(lexeme *list)
{
    if (ERROR_FLAG == 1)
    {
        return;
    }

    // Current token is subtract symbol.
    if (list[lIndex].type == subsym)
    {
        // Get next token.
        lIndex++;

        // Generate the terms from the list.
        term(list);

        // Add an instruction to the code array and increment the code index.
        emit(2, 0, 1); // NEG instruction

        // Current token is add or subtract symbol.
        while (list[lIndex].type == addsym || list[lIndex].type == subsym)
        {
            if (list[lIndex].type == addsym)
            {
                // Get next token.
                lIndex++;

                // Generate term expression.
                term(list);

                // Add an instruction to the code array and increment the code index.
                emit(2, 0, 2); // ADD instruction
            }
            else
            {
                // Get next token.
                lIndex++;

                // Generate term expression.
                term(list);

                // Add an instruction to the code array and increment the code index.
                emit(2, 0, 3); // SUB instruction
            }
        }
    }
    else
    {
        // Current token is an add symbol.
        if (list[lIndex].type == addsym)
        {
            // Get next token.
            lIndex++;
        }

        // Generate term expression.
        term(list);

        // Current token is add or subtract symbol.
        while (list[lIndex].type == addsym || list[lIndex].type == subsym)
        {
            if (list[lIndex].type == addsym)
            {
                // Get next token.
                lIndex++;

                // Generate term expression.
                term(list);

                // Add an instruction to the code array and increment the code index.
                emit(2,0,2); // ADD instruction
            }
            else
            {
                // Get next token.
                lIndex++;

                // Generate term expression.
                term(list);

                // Add an instruction to the code array and increment the code index.
                emit(2,0,3); // SUB instruction
            }
        }
    }

    // Identifier number odd.
    if (list[lIndex].type == identsym ||
        list[lIndex].type == numbersym ||
        list[lIndex].type == oddsym)
    {
        printparseerror(17);
        ERROR_FLAG = 1;
        return;
    }
}

// Generate condition statements from a token list.
void condition(lexeme *list)
{
    if (ERROR_FLAG == 1)
    {
        return;
    }

    // Current token is an odd symbol.
    if (list[lIndex].type == oddsym)
    {
        // Get next token.
        lIndex++;

        // Generate expression from lexeme list.
        expression(list);

        // Add an instruction to the code array and increment the code index.
        emit(2,0,6); // Odd instruction
    }
    else
    {
        // Generate expression from lexeme list.
        expression(list);

        // Token is equal symbol.
        if (list[lIndex].type == eqlsym)
        {
            // Get next token.
            lIndex++;

            // Generate expression from lexeme list.
            expression(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 8); // EQL instruction
        }
        else if (list[lIndex].type == neqsym)
        {
            // Get next token.
            lIndex++;

            // Generate expression from lexeme list.
            expression(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 9); // NEQ instruction
        }
        else if (list[lIndex].type == lsssym)
        {
            // Get next token.
            lIndex++;

            // Generate expression from lexeme list.
            expression(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 10); // LSS instruction
        }
        else if (list[lIndex].type == leqsym)
        {
            // Get next token.
            lIndex++;

            // Generate expression from lexeme list.
            expression(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 11); // LEQ instruction
        }
        else if (list[lIndex].type == gtrsym)
        {
            // Get next token.
            lIndex++;

            // Generate expression from lexeme list.
            expression(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 12); // GTR instruction
        }
        else if (list[lIndex].type == geqsym)
        {
            // Get next token.
            lIndex++;

            // Generate expression from lexeme list.
            expression(list);

            // Add an instruction to the code array and increment the code index.
            emit(2, 0, 13); // GEQ instruction
        }
        else
        {
            printparseerror(10);
            ERROR_FLAG = 1;
            return;
        }
    }
}

// Handles statement generation.
void statement(lexeme *list)
{
    int symbol_index, jmpIdx, jpcIdx, loopIdx;

    if (ERROR_FLAG == 1)
    {
        return;
    }

    // Token for the current level is an identifier.
    if (list[lIndex].type == identsym)
    {
        // Find symbol for tokens of kind 2.
        symbol_index = find_sym(list, 2);

        // Symbol index wasn't found,
        if (symbol_index == -1)
        {
            if (find_sym(list, 1) != find_sym(list, 3))
            {
                printparseerror(16);
                ERROR_FLAG = 1;
                return;
            }
            else
            {
                printparseerror(17);
                ERROR_FLAG = 1;
                return;
            }
        }

        // Get next token.
        lIndex++;

        if (list[lIndex].type != assignsym)
        {
            printparseerror(5);
            ERROR_FLAG = 1;
            return;
        }

        // Get next token.
        lIndex++;

        expression(list);

        // Add an instruction to the code array and increment the code index.
        // STO instruction.
        emit(4, level-table[symbol_index].level, table[symbol_index].addr);

        return;
    }

    // Token for the current level is a begin symbol.
    if (list[lIndex].type == beginsym)
    {
        do
        {
            // Get next token.
            lIndex++;
            statement(list);
        } while (list[lIndex].type == semicolonsym);

        if (list[lIndex].type != endsym)
        {
            if (list[lIndex].type == identsym ||
                list[lIndex].type == beginsym ||
                list[lIndex].type == ifsym ||
                list[lIndex].type == whilesym ||
                list[lIndex].type == readsym ||
                list[lIndex].type == writesym ||
                list[lIndex].type == callsym)
            {
                printparseerror(15);
                ERROR_FLAG = 1;
                return;
            }
            else
            {
                printparseerror(16);
                ERROR_FLAG = 1;
                return;
            }
        }

        // Get next token.
        lIndex++;
        return;
    }

    // Current token is an if symbol.
    if (list[lIndex].type == ifsym)
    {
        // Get next token.
        lIndex++;

        // Handle the condition.
        condition(list);

        // Jump to top of stack index.
        jpcIdx = cIndex;

        // Add an instruction to the code array and increment the code index.
        // JPC Instruction (get from top of stack).
        emit(8, 0, jpcIdx);

        if (list[lIndex].type != thensym)
        {
            printparseerror(8);
            ERROR_FLAG = 1;
            return;
        }

        // Get next token.
        lIndex++;

        // Recursive statement generation.
        statement(list);

        if (list[lIndex].type == elsesym)
        {
            // JMP index is received from current index.
            jmpIdx = cIndex;

            // Add an instruction to the code array and increment the code index.
            // JMP Instruction.
            emit(7, 0, jmpIdx);

            // Top of stack value becomes the current instruction's 3rd slot.
            code[jpcIdx].m = (cIndex * 3);

            // Get next token.
            lIndex++;

            // Recursive statement generation.
            statement(list);

            // Jump index receives the mValue from instruction's 3rd slot.
            code[jmpIdx].m = (cIndex * 3);
        }
        else
        {
            // Top of stack value becomes the current instruction's 3rd slot.
            code[jpcIdx].m = (cIndex * 3);
        }

        return;
    }

    // Current token is while symbol.
    if (list[lIndex].type == whilesym)
    {
        // Get next token.
        lIndex++;

        // Loop index is the current index.
        loopIdx = cIndex;

        // Generation conditional statements.
        condition(list);

        // Do symbol can not be inside a while statement.
        if (list[lIndex].type != dosym)
        {
            printparseerror(9);
            ERROR_FLAG = 1;
            return;
        }

        // Get next token.
        lIndex++;

        // Jump to top of stack reference.
        jpcIdx = cIndex;

        // Add an instruction to the code array and increment the code index.
        // JPC Instruction.
        emit(8, 0, 0);

        // Recursive statement generation.
        statement(list);

        // Add an instruction to the code array and increment the code index.
        // JMP Instruction.
        emit(7, 0, (loopIdx * 3));

        // Instruction M value for top of stack becomes current code segment M value.
        code[jpcIdx].m = (cIndex * 3);

        return;
    }

    // Current token is read symbol.
    if (list[lIndex].type == readsym)
    {
        // Get next token.
        lIndex++;

        // Can not declare identifier symbol within read statement.
        if (list[lIndex].type != identsym)
        {
            printparseerror(6);
            ERROR_FLAG = 1;
            return;
        }

        // Get the symbol table index for symbol of kind 2.
        symbol_index = find_sym(list, 2);

        // No kind 2 symbols found.
        if (symbol_index == -1)
        {
            if (find_sym(list, 1) != find_sym(list, 3))
            {
                printparseerror(20);
                ERROR_FLAG = 1;
                return;
            }
            else
            {
                printparseerror(19);
                ERROR_FLAG = 1;
                return;
            }
        }

        // Get next token.
        lIndex++;

        // Add an instruction to the code array and increment the code index.
        emit(9,0,2); // READ Instruction
        emit(4, (level - table[symbol_index].level), table[symbol_index].addr); // STO Instruction

        return;
    }

    // Current token is a write symbol.
    if (list[lIndex].type == writesym)
    {
        // Get next token.
        lIndex++;

        // Generate expression statement.
        expression(list);

        // Add an instruction to the code array and increment the code index.
        emit(9,0,1); // WRITE Instruction

        return;
    }

    // Current token is a call symbol.
    if (list[lIndex].type == callsym)
    {
        // Get next token.
        lIndex++;

        // Find symbol of kind 3.
        symbol_index = find_sym(list, 3);

        // Symbol isn't found, give error.
        if (symbol_index == -1)
        {
            if (find_sym(list, 1) != find_sym(list, 2))
            {
                printparseerror(7);
                ERROR_FLAG = 1;
                return;
            }
            else
            {
                printparseerror(7);
                ERROR_FLAG = 1;
                return;
            }
        }

        // Get next token.
        lIndex++;

        // Add an instruction to the code array and increment the code index.
        emit(5, level-table[symbol_index].level, symbol_index); // CAL Instruction
    }
}

// Find the proper symbol and maximize the level value.
int find_sym(lexeme *list, int kind)
{
    int i;

    if (ERROR_FLAG == 1)
    {
        return -1;
    }

    // Go from bottom of the table to the top to maximize the lex level.
    for (i = tIndex; i >= 0; i--)
    {
        if (strcmp(table[i].name, list[lIndex].name) == 0 &&
            table[i].kind == kind &&
            table[i].mark == 0)
        {
            return i;
        }
    }

    return -1;
}

// Find existing symbol & return index.
int mult_dec_check(lexeme *list)
{
    int i;

    if (ERROR_FLAG == 1)
    {
        return -1;
    }

    // Linear search on symbol table.
    for (i = 0; i <= tIndex; i++)
    {
        if ((strcmp(table[i].name, list[lIndex].name) == 0) &&
            (table[i].level == level) &&
            (table[i].mark == UNMARKED))
        {
            return i;
        }
    }

    return -1;
}

// Handles variable declaration.
int var_dec(lexeme *list)
{
    int numVars = 0, symbol_index;

    if (ERROR_FLAG == 1)
    {
        return ERROR_FLAG;
    }

    // If there is variables in the tokens, create them.
    if (list[lIndex].type == varsym)
    {
        do
        {
            numVars++;

            // Get next token.
            lIndex++;

            // list[lIndex].type is the token.
            if (list[lIndex].type != identsym)
            {
                printparseerror(3);
                ERROR_FLAG = 1;
                return -1;
            }

            // Get symbol from multiple declaration check using the tokens.
            symbol_index = mult_dec_check(list);

            if (symbol_index != -1)
            {
                printparseerror(18);
                ERROR_FLAG = 1;
                return -1;
            }

            if (level == 0)
            {
                // Kind, name, value, level, mValue, mark.
                addToSymbolTable(2, list[lIndex].name, 0, level, (numVars - 1), UNMARKED);
            }
            else
            {
                // Kind, name, value, level, mValue, mark.
                addToSymbolTable(2, list[lIndex].name, 0, level, (numVars + 2), UNMARKED);
            }

            // Get next token.
            lIndex++;

        } while (list[lIndex].type == commasym);

        if (list[lIndex].type != semicolonsym)
        {
            if (list[lIndex].type == identsym)
            {
                printparseerror(14);
                ERROR_FLAG = 1;
                return -1;
            }
            else
            {
                printparseerror(13);
                ERROR_FLAG = 1;
                return -1;
            }
        }

        // Get next token.
        lIndex++;
    }

    return numVars;
}

// Handles procedure declaration.
void proc_dec(lexeme *list)
{
    int symbol_index;

    if (ERROR_FLAG == 1)
    {
        return;
    }

    while (list[lIndex].type == procsym)
    {
        // Get next token.
        lIndex++;

        if (list[lIndex].type != identsym)
        {
            printparseerror(4);
            ERROR_FLAG = 1;
            return;
        }

        // Multiple declaration check on the tokens to get the symbol index.
        symbol_index = mult_dec_check(list);

        if (symbol_index != -1)
        {
            printparseerror(18);
            ERROR_FLAG = 1;
            return;
        }

        // Kind, name, value, level, mValue, mark.
        addToSymbolTable(3, list[lIndex].name, 0, level, 0, UNMARKED);

        // Get next token.
        lIndex++;

        if (list[lIndex].type != semicolonsym)
        {
            printparseerror(4);
            ERROR_FLAG = 1;
            return;
        }

        // Get next token.
        lIndex++;

        block(list);

        if (list[lIndex].type != semicolonsym)
        {
            printparseerror(14);
            ERROR_FLAG = 1;
            return;
        }

        // Get next token.
        lIndex++;

        // RTN instruction.
        emit(2,0,0);
    }
}

// Mark symbols in symbol table.
void mark(void)
{
    if (ERROR_FLAG == 1)
    {
        return;
    }

    int i;

    // If table level matches the current level, mark the symbol.
    for (i = cIndex; i >= 0; i--)
    {
        if (table[i].level == level)
        {
            table[i].mark = MARKED;
        }
    }
}

// Directs the control flow.
void block(lexeme *list)
{
    if (ERROR_FLAG == 1)
    {
        return;
    }

    int proc_index, x;

    // Increment lexical level.
    level++;

    // The procedure index is same as table index.
    proc_index = tIndex - 1;

    // Constant declaration.
    const_dec(list);

    // Variable declaration.
    x = var_dec(list);

    // Procedure declaration.
    proc_dec(list);

    // Get the instruction mlevel from the code index.
    table[proc_index].addr = (cIndex * 3);

    if (level == 0)
    {
        // INC instruction.
        emit(6, 0, x);
    }
    else
    {
        // INC instruction + 3.
        emit(6, 0, (x + 3));
    }

    // Make a statement using the token.
    statement(list);

    // Mark symbols when we go back up a lexical level.
    mark();

    // Decrement level.
    level--;
}

// Analyzes token lexeme to create symbol table.
void program(lexeme *list)
{
    int i;

    if (ERROR_FLAG == 1)
    {
        return;
    }

    // Add an instruction to the code array and increment the code index.
    // JMP instruction.
    emit(7, 0, 0);

    // Kind, name, value, level, mValue, mark.
    addToSymbolTable(3, "main", 0, 0, 0, UNMARKED);

    // Lexical level is -1.
    level = -1;

    // Generate a block of code from the lexeme list.
    block(list);

    // Token is not period symbol.
    if (list[lIndex].type != periodsym)
    {
        printparseerror(1);
        ERROR_FLAG = 1;
        return;
    }

    // Add an instruction to the code array and increment the code index.
    // HALT instruction.
    emit(9, 0, 3);

    // For every line in code.
    for (i = 0; i < cIndex; i++)
    {
        // Line has OPR 5.
        if (code[i].opcode == 5)
        {
            code[i].m = table[code[i].m].addr;
        }
    }

    // Instruction mValue is the taken from the first symbol table.
    code[0].m = table[0].addr;
}

// Handles constant declaration.
void const_dec(lexeme *list)
{
    int numVar = 0, symbol_index, ident_name;

    if (ERROR_FLAG == 1)
    {
        return;
    }

    if (list[lIndex].type == constsym)
    {
        // Do a loop no matter what, then continue while current token is commasym.
        do
        {
            // Increment the number of variables.
            numVar++;

            // Increment the level index.
            lIndex++;

            if (list[lIndex].type != identsym)
            {
                printparseerror(2);
                ERROR_FLAG = 1;
                return;
            }

            // Perform a multiple declaration check with the current level token.
            // Store the result in the symbol index.
            symbol_index = mult_dec_check(list);

            if (symbol_index != -1)
            {
                printparseerror(18);
                ERROR_FLAG = 1;
                return;
            }

            // Save ident name.
            ident_name = lIndex;

            // Get next token.
            lIndex++;

            if (list[lIndex].type != assignsym)
            {
                printparseerror(5);
                ERROR_FLAG = 1;
                return;
            }

            // Get next token.
            lIndex++;

            if (list[lIndex].type != numbersym)
            {
                printparseerror(2);
                ERROR_FLAG = 1;
                return;
            }

            // Kind, name, value, level, mValue, mark.
            addToSymbolTable(1, list[ident_name].name, list[lIndex].value, level, 0, UNMARKED);

            // Get next token.
            lIndex++;

        } while (list[lIndex].type == commasym);

        // Token end statement does not contain a semi-colon.
        if (list[lIndex].type != semicolonsym)
        {
            if (list[lIndex].type == identsym)
            {
                printparseerror(14);
                ERROR_FLAG = 1;
                return;
            }
            else
            {
                printparseerror(13);
                ERROR_FLAG = 1;
                return;
            }
        }

        lIndex++;
    }
}


// Hardcode compiler values and begin parsing / code generation.
instruction *parse(lexeme *list, int printTable, int printCode)
{
    // Hardware coded values.
    level = lIndex = tIndex = cIndex = 0;

    // Allocate space in memory for the symbol table and the instructions.
    table = malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);
    code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);

    // Starts the parsing/code generatation.
    program(list);

    // This line is how the VM knows where the code ends when copying it to the PAS.
    code[cIndex].opcode = -1;

    if (printTable == 1)
    {
        printsymboltable();
    }

    if (printCode == 1)
    {
        printassemblycode();
    }

    if (table != NULL)
    {
        free(table);
    }

    return code;
}


void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}


void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	
	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);
	
	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
