// Lexical analysis simulator.

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "compiler.h"

#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

#define INVALID_SYM_ERR 1
#define INVALID_IDENT_ERR 2
#define EXCESS_NUM_ERR 3
#define EXCESS_IDENT_ERR 4
#define UNREC_ERR 5

#define DEBUG 0

// Create the max number of tokens by default (token space may go unused).
lexeme *list;

// Lexeme index begins with zero by default.
int lex_index = 0;

void printlexerror(int type);
void printtokens();
void tokenize(char *buffer);
void make_lexeme(char *name, int value, token_type type);

// Free the buffer.
char *destroyBuffer(char *buffer)
{
    if (buffer != NULL)
    {
        free(buffer);
    }

    return NULL;
}

// Analyze raw lexis to tokenize.
lexeme *lexanalyzer(char *input)
{
    int counter = 0, inputLen, wordCount = 0, j, i, flag = 0;
    char *buffer;

    inputLen = strlen(input);

    // Initialize the list component.
    list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);

    // Input types: Alphabetical, Numerical, Identifier, Comment, Symbol, Invalid.
    // Loop through raw lexis and categorize input by type.
    for (i = 0; i < inputLen; (flag == 1) ? i : i++)
    {
        // Used to keep track of token length.
        counter = 0;

        // Dynamically allocate buffer memory (only enough space for 1 token).
        buffer = malloc(sizeof(char) * (MAX_IDENT_LEN + 1));

        if (DEBUG == 1)
        {
            printf("INPUT: %c --- I: %d --- \n", input[i], i);
        }

        // Decides whether to increase i or not.
        flag = 0;

        if (isalpha(input[i]) > 0) // Input is alphabetical
        {
            // Keep looping either until we get an error or run into a number.
            while ((isalpha(input[i]) > 0) || (isdigit(input[i]) > 0))
            {
                // Transfer the contents of the input to the buffer.
                buffer[counter] = input[i];

                // Increment the buffer counter.
                counter++;

                i++;

                // Identifiers must not be greater than 11 in length.
                if (counter >= MAX_IDENT_LEN)
                {
                    if (DEBUG == 1)
                    {
                        printf("isalpha\n");
                    }

                    printlexerror(EXCESS_IDENT_ERR);
                    return NULL;
                }
            }

            // Doing this to prevent skipping over an end colon.
            flag = 1;

            // Properly form the token string with sentinal.
            buffer[counter] = '\0';

            if (DEBUG == 1)
            {
                printf("\nBUFFER BEFORE TOKENIZATION (ISALPHA): %s\n", buffer);
            }

            // Turn the identifier into a lexeme.
            tokenize(buffer);

            if (DEBUG == 1)
            {
                printf("BUFFER: %s, TYPE: %d, VAL: %d\n\n", list[lex_index].name, list[lex_index].type, list[lex_index].value);
            }
        }
        else if (isdigit(input[i]) > 0) // Input is a digit
        {
            // Keep looping either until we get an error or run into a number.
            while (isdigit(input[i]) > 0)
            {
                // Transfer the contents of the input to the buffer.
                buffer[counter] = input[i];

                // Increment the buffer counter and input counter.
                counter++;

                i++;

                // Digits must not be greater than 5 in length.
                if (counter > MAX_NUMBER_LEN)
                {
                    if (DEBUG == 1)
                    {
                        printf("isdigit\n");
                    }

                    printlexerror(EXCESS_NUM_ERR);
                    return NULL;
                }
            }

            // Identifier can not be declared directly behind digit.
            if (isalpha(input[i]) > 0)
            {
                printlexerror(INVALID_IDENT_ERR);
                return NULL;
            }

            // Doing this to prevent skipping over an end colon.
            flag = 1;

            // Properly form the token string with sentinal.
            buffer[counter] = '\0';

            if (DEBUG == 1)
            {
                printf("\nBUFFER BEFORE TOKENIZATION (ISDIGIT): %s\n", buffer);
            }

            // Turn the digit into a lexeme.
            tokenize(buffer);

            if (DEBUG == 1)
            {
                printf("BUFFER: %s, TYPE: %d, VAL: %d\n\n", list[lex_index].name, list[lex_index].type, list[lex_index].value);
            }
        }
        else if (iscntrl(input[i]) > 0 ||
                 input[i] == ' ' ||
                 input[i] == '\n' ||
                 input[i] == '\t' ||
                 input[i] == '\0') // Input is a space
        {
            continue;
        }
        else if (input[i] == '/') // Input is a comment
        {
            if (i >= inputLen - 1)
            {
                if (DEBUG == 1)
                {
                    printf("comment\n");
                }

                // The input string has an unfinished comment as the last character.
                printlexerror(INVALID_SYM_ERR);
                return NULL;
            }

            i++;

            // A comment is initiated.
            if (input[i] == '/')
            {
                // All input skipped in this line is a comment.
                while (input[i] != '\n')
                {
                    if (DEBUG == 1)
                    {
                        printf("inside comment i: %d\n", i);
                    }
                    i++;
                }

                if (DEBUG == 1)
                {
                    printf("\n");
                }
            }
            else
            {
                if (DEBUG == 1)
                {
                    printf("Single / character error.\n");
                }

                // The input string has invalid symbol.
                printlexerror(INVALID_SYM_ERR);
                return NULL;
            }

            continue;
        }
        else // Input is a special symbol
        {
            if (input[i] == ':') // Assign Symbol
            {
                if (i >= inputLen - 1)
                {
                    if (DEBUG == 1)
                    {
                        printf("lol8\n");
                    }

                    // The input string has invalid symbol.
                    printlexerror(INVALID_SYM_ERR);
                    return NULL;
                }

                if (input[i+1] == '=')
                {
                    tokenize(":=");
                }
                else
                {
                    if (DEBUG == 1)
                    {
                        printf("lol7\n");
                    }

                    // The input string has invalid symbol.
                    printlexerror(INVALID_SYM_ERR);
                    return NULL;
                }
                i++;
            }
            else if (input[i] == '+') // Add Symbol
            {
                tokenize("+");
            }
            else if (input[i] == '-') // Subtract Symbol
            {
                tokenize("-");
            }
            else if (input[i] == '*') // Multiplication Symbol
            {
                tokenize("*");
            }
            else if (input[i] == '/') // Division Symbol
            {
                tokenize("/");
            }
            else if (input[i] == '%') // Mod Symbol
            {
                tokenize("%");
            }
            else if (input[i] == '=') // Equal Symbol
            {
                if (i >= inputLen - 1)
                {
                    // The input string has invalid symbol.
                    printlexerror(INVALID_SYM_ERR);
                    return NULL;
                }

                if (input[i+1] == '=')
                {
                    tokenize("==");
                }
                else
                {
                    if (DEBUG == 1)
                    {
                        printf("lol6\n");
                    }

                    // The input string has invalid symbol.
                    printlexerror(INVALID_SYM_ERR);
                    return NULL;
                }
                i++;
            }
            else if (input[i] == '!') // Not equal Symbol
            {
                if (i >= inputLen - 1)
                {
                    // The input string has invalid symbol.
                    printlexerror(INVALID_SYM_ERR);
                    printf("comment\n");
                }

                if (input[i+1] == '=')
                {
                    tokenize("!=");
                }
                else
                {
                    if (DEBUG == 1)
                    {
                        printf("lol5\n");
                    }

                    // The input string has invalid symbol.
                    printlexerror(INVALID_SYM_ERR);
                    return NULL;
                }
                i++;
            }
            else if (input[i] == '<') // Less than or equal to Symbol
            {
                if (i >= inputLen - 1)
                {
                    if (DEBUG == 1)
                    {
                        printf("lol1\n");
                    }

                    // The input string has invalid symbol.
                    printlexerror(INVALID_SYM_ERR);
                    return NULL;
                }

                if (input[i+1] == '=')
                {
                    tokenize("<=");
                    i++;
                }
                else
                {
                    tokenize("<");
                }
            }
            else if (input[i] == '>') // Greater than or equal to Symbol
            {
                if (i >= inputLen - 1)
                {
                    if (DEBUG == 1)
                    {
                        printf("lol3\n");
                    }

                    // The input string has invalid symbol.
                    printlexerror(INVALID_SYM_ERR);
                    return NULL;
                }

                if (input[i+1] == '=')
                {
                    tokenize(">=");
                    i++;
                }
                else
                {
                    tokenize(">");
                }
            }
            else if (input[i] == '(') // Parenthesis Open Symbol
            {
                tokenize("(");
            }
            else if (input[i] == ')') // Parenthesis Close Symbol
            {
                tokenize(")");
            }
            else if (input[i] == ',') // Comma Symbol
            {
                tokenize(",");
            }
            else if (input[i] == '.') // Period Symbol
            {
                tokenize(".");
            }
            else if (input[i] == ';') // Semicolon Symbol
            {
                tokenize(";");
            }
            else if (input[i] == '\n')
            {
                continue;
            }
            else if (input[i] == ' ')
            {
                continue;
            }
            else if (input[i] == '\t')
            {
                continue;
            }
            else // Other Symbol
            {
                if (DEBUG == 1)
                {
                    printf("input: |%c|\n", input[i]);
                    printf("invalid\n");
                }

                printlexerror(INVALID_SYM_ERR);
                return NULL;
            }

            if (DEBUG == 1)
            {
                printf("BUFFER: %s, TYPE: %d, VAL: %d\n\n", list[lex_index].name, list[lex_index].type, list[lex_index].value);
            }
        }

        // Increment the lexeme index.
        lex_index++;

        // Reset the buffer on each iteration.
        buffer = destroyBuffer(buffer);
    }

    printtokens();

	return list;
}

// Convert a string to an integer.
int stringToInt(char *str)
{
    int digit = 0, i, strLen, val;

    strLen = strlen(str);

    // String is a single digit.
    if (strLen == 1)
    {
        return (str[0] - '0');
    }

    // Algorithm to convert string to digit.
    for (i = 1; i <= strLen; i++)
    {
        val = (atoi(&str[strLen - i]) / pow(10, i-1));
        digit += (val * pow(10, i-1));
    }

    return digit;
}

// Build the lexeme.
void make_lexeme(char *name, int value, token_type type)
{
    strcpy(list[lex_index].name, name);
    list[lex_index].value = value;
    list[lex_index].type = type;
}

// Tokenize input buffer.
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
    else if (isalpha(buffer[0]) > 0)
    {
        // Buffer is an identifier.
        make_lexeme(buffer, 14, identsym);
    }
    else if (isdigit(buffer[0]) > 0)
    {
        // Buffer is a digit.
        make_lexeme(buffer, stringToInt(buffer), numbersym);
    }
    else
    {
        if (DEBUG == 1)
        {
            printf("invalid in tokenizer\n");
        }

        printlexerror(INVALID_IDENT_ERR);
    }
}

// Print the available tokens (identifiers, keywords,
// separators, operators, literal, comments) from a lexis.
void printtokens(void)
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
