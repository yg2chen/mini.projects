#pragma once

#include <cstdlib>
#include <string>

extern int CurTok;
extern std::string IdentifierString;
extern double NumVal;

int getToken();
int getNextToken();
