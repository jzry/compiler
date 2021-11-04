// Jerzy Dobrowolski, HW2, je942968.
// Lexical analysis simulator.

/*
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.
	
	If you choose to alter the printing functions or delete list or lex_index, 
	you MUST make a note of that in you readme file, otherwise you will lose 
	5 points.
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

#define INVALID_SYM_ERR 1
#define INVALID_IDENT_ERR 2
#define EXCESS_IDENT_ERR 3
#define EXCESS_NUM_ERR 4
#define UNREC_ERR 5

#define DEBUG 1

// Create the max number of tokens by default (token space may go unused).
lexeme *list;

// Lexeme index begins with zero by default.
int lex_index = 0;

void printlexerror(int type);
void printtokens();
void tokenize(char *buffer);

// Reset buffer to all zeros.
char *reset(char *buffer)
{
    for (int i = 0; i < MAX_IDENT_LEN; i++) { buffer[i] = '0'; } return buffer;
}

// Analyze raw lexis to tokenize.
lexeme *lexanalyzer(char *input)
{
    int counter = 0, inputLen, wordCount = 0, j, i;
    char *buffer;

    inputLen = strlen(input);

    // Dynamically allocate buffer memory.
    buffer = malloc(sizeof(char) * (MAX_IDENT_LEN + 1));

    // Initialize the list component.
    list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);

    // Loop through raw lexis
    for (i = 0; i < inputLen; i++)
    {
        // Used to keep track of token length.
        counter = 0;

        if (isalpha(input[i]) > 0) // Input is alphabetical
        {
            // Keep looping either until we get an error or run into a number.
            for (j = 0; j < (j + MAX_IDENT_LEN); j++)
            {
                // Identifiers must not be greater than 11 in length.
                if (counter >= MAX_IDENT_LEN)
                {
                    printlexerror(EXCESS_IDENT_ERR);
                    return NULL;
                }

                // Identifier can be either a number or a letter.
                if ((isalpha(input[counter]) > 0) || (isdigit(input[counter]) > 0))
                {
                    if (DEBUG == 1)
                    {
                        printf("%c\n", buffer[counter]);
                    }

                    // Transfer the contents of the input to the buffer.
                    buffer[counter] = input[counter];

                    // Increment the buffer counter and input counter.
                    counter++;

                    // Increment overall count.
                    i++;
                }
                else
                {
                    // Non numerical symbol.
                    break;
                }
            }
        }
        else if (isdigit(input[i]) > 0) // Input is a digit
        {
            // Keep looping either until we get an error or run into a number.
            for (j = 0; j < (j + MAX_NUMBER_LEN); j++)
            {
                // Identifiers must not be greater than 11 in length.
                if (counter >= MAX_NUMBER_LEN)
                {
                    printlexerror(EXCESS_NUM_ERR);
                    return NULL;
                }

                // Identifier can only be a number.
                if (isdigit(input[counter]) > 0)
                {
                    if (DEBUG == 1)
                    {
                        printf("%c\n", buffer[counter]);
                    }

                    // Transfer the contents of the input to the buffer.
                    buffer[counter] = input[counter];

                    // Increment the buffer counter and input counter.
                    counter++;

                    // Increment overall count.
                    i++;
                }
                else if (isalpha(input[counter] > 0))
                {
                    // Letter can not contain any letters.
                    printlexerror(INVALID_IDENT_ERR);
                    return NULL;
                }
                else
                {
                    // Non numerical symbol.
                    break;
                }
            }
        }
        else if (iscntrl(input[i]) > 0) // Input is a space
        {
            continue;
        }
        else // Input is invalid
        {
            printlexerror(INVALID_IDENT_ERR);
        }

        // Properly form the token string with sentinal.
        buffer[counter] = '\0';

        // Create a token from the buffer lexeme.
        tokenize(buffer);

        // Reset buffer
        buffer = reset(buffer);
    }

    if (buffer != NULL)
    {
        free(buffer);
    }

	return list;
}

//typedef struct lexeme {
//    char name[12];
//    int value;
//    token_type type;
//} lexeme;

// Check if a string is all numerical.
int isAllDigit(char *buffer)
{
    int i, strLen, flag = 0, result;

    strLen = strlen(buffer);
    for (i = 0; i < strLen; i++)
    {
        // If result is 0, a character is not alphabetical.
        if (isdigit(buffer[i]) <= 0)
        {
            flag = 1;
        }
    }

    return flag;
}

// Build the lexeme.
void make_lexeme(char *name, int value, token_type type)
{
    strcpy(list[lex_index].name, name);
    list[lex_index].value = value;
    list[lex_index].type = type;
}

// Tokenize input from buffer.
void tokenize(char *buffer)
{
    // Categorize the input and tokenize it.
    if (strcmp(buffer, "const") == 0)
    {
        make_lexeme("const", 1, constsym);
    }
    else if (strcmp(buffer, "var") == 0)
    {
        make_lexeme("var", 2, varsym);
    }
    else if (strcmp(buffer, "procedure") == 0)
    {
        make_lexeme("procedure", 3, procsym);
    }
    else if (strcmp(buffer, "begin") == 0)
    {
        make_lexeme("begin", 4, beginsym);
    }
    else if (strcmp(buffer, "end") == 0)
    {
        make_lexeme("end", 5, endsym);
    }
    else if (strcmp(buffer, "while") == 0)
    {
        make_lexeme("while", 6, whilesym);
    }
    else if (strcmp(buffer, "do") == 0)
    {
        make_lexeme("do", 7, dosym);
    }
    else if (strcmp(buffer, "if") == 0)
    {
        make_lexeme("if", 8, ifsym);
    }
    else if (strcmp(buffer, "then") == 0)
    {
        make_lexeme("then", 9, thensym);
    }
    else if (strcmp(buffer, "else") == 0)
    {
        make_lexeme("else", 10, elsesym);
    }
    else if (strcmp(buffer, "call") == 0)
    {
        make_lexeme("call", 11, callsym);
    }
    else if (strcmp(buffer, "write") == 0)
    {
        make_lexeme("write", 12, writesym);
    }
    else if (strcmp(buffer, "read") == 0)
    {
        make_lexeme("read", 13, readsym);
    }
    else if (strcmp(buffer, ":=") == 0)
    {
        make_lexeme(":=", 16, assignsym);
    }
    else if (strcmp(buffer, "+") == 0)
    {
        make_lexeme("+", 17, addsym);
    }
    else if (strcmp(buffer, "-") == 0)
    {
        make_lexeme("-", 18, subsym);
    }
    else if (strcmp(buffer, "*") == 0)
    {
        make_lexeme("*", 19, multsym);
    }
    else if (strcmp(buffer, "/") == 0)
    {
        make_lexeme("/", 20, divsym);
    }
    else if (strcmp(buffer, "%") == 0)
    {
        make_lexeme("%", 21, modsym);
    }
    else if (strcmp(buffer, "==") == 0)
    {
        make_lexeme("==", 22, eqlsym);
    }
    else if (strcmp(buffer, "!=") == 0)
    {
        make_lexeme("!=", 23, neqsym);
    }
    else if (strcmp(buffer, "<") == 0)
    {
        make_lexeme("<", 24, lsssym);
    }
    else if (strcmp(buffer, "<=") == 0)
    {
        make_lexeme("<=", 25, leqsym);
    }
    else if (strcmp(buffer, ">") == 0)
    {
        make_lexeme(">", 26, gtrsym);
    }
    else if (strcmp(buffer, ">=") == 0)
    {
        make_lexeme(">=", 27, geqsym);
    }
    else if (strcmp(buffer, "odd") == 0)
    {
        make_lexeme("odd", 28, oddsym);
    }
    else if (strcmp(buffer, "(") == 0)
    {
        make_lexeme("(", 29, lparensym);
    }
    else if (strcmp(buffer, ")") == 0)
    {
        make_lexeme(")", 30, rparensym);
    }
    else if (strcmp(buffer, ",") == 0)
    {
        make_lexeme(",", 31, commasym);
    }
    else if (strcmp(buffer, ".") == 0)
    {
        make_lexeme(".", 32, periodsym);
    }
    else if (strcmp(buffer, ";") == 0)
    {
        make_lexeme(";", 33, semicolonsym);
    }
    else if (isAllDigit(buffer) == 0)
    {
        make_lexeme(buffer, 15, numbersym);
    }
    else // buffer is an identifier.
    {
        make_lexeme(buffer, 14, identsym);
    }

    // Increment the lexeme index.
    lex_index++;
}

// Print the available tokens (identifiers, keywords,
// separators, operators, literal, comments) from a lexis.
void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

// Input lexis has syntactical errors.
void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

    if (list != NULL)
    {
        free(list);
    }
	return;
}
