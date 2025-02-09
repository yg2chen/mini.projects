#include "lexer.h"
#include "token.h"

int CurTok;
std::string IdentifierString;
double NumVal;

// The actual implementation of the lexer is a single function gettok()
// It's called to return the next token from standard input
// gettok works by calling getchar() function to read chars one at a time
// Then it recognizes them and stores the last character read in LastChar
int gettok() {
    static int LastChar = ' ';

    // Skip any whitespace.
    while (isspace(LastChar))
        LastChar = getchar();

    if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        IdentifierString = LastChar;
        while (isalnum((LastChar = getchar())))
            IdentifierString += LastChar;

        if (IdentifierString == "def")
            return tok_def;
        if (IdentifierString == "extern")
            return tok_extern;
        if (IdentifierString == "if")
            return tok_if;
        if (IdentifierString == "then")
            return tok_then;
        if (IdentifierString == "else")
            return tok_else;
        if (IdentifierString == "for")
            return tok_for;
        if (IdentifierString == "in")
            return tok_in;
        if (IdentifierString == "binary")
            return tok_binary;
        if (IdentifierString == "unary")
            return tok_unary;
        return tok_identifier;
    }

    if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), nullptr);
        return tok_number;
    }

    if (LastChar == '#') {
        // Comment until end of line.
        do
            LastChar = getchar();
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return gettok();
    }

    // Check for end of file.  Don't eat the EOF.
    if (LastChar == EOF)
        return tok_eof;

    // Otherwise, just return the character as its ascii value.
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}

int getNextToken() { return CurTok = gettok(); }
