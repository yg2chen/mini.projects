#pragma once

#include "BinaryExprAST.h"
#include "CallExprAST.h"
#include "ExprAST.h"
#include "FunctionAST.h"
#include "IfExprAST.h"
#include "NumberExprAST.h"
#include "PrototypeAST.h"
#include "UnaryExprAST.h"
#include "VariableExprAST.h"
#include "lexer.h"
#include "logger.h"
#include "token.h"
#include <map>
#include <memory>

extern std::map<char, int> BinopPrecedence;
std::unique_ptr<ExprAST> ParseNumberExpr();
std::unique_ptr<ExprAST> ParseParenExpr();
std::unique_ptr<ExprAST> ParseIdentifierExpr();
std::unique_ptr<ExprAST> ParsePrimary();
std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                       std::unique_ptr<ExprAST> LHS);
std::unique_ptr<ExprAST> ParseExpression();
std::unique_ptr<PrototypeAST> ParsePrototype();
std::unique_ptr<FunctionAST> ParseDefinition();
std::unique_ptr<FunctionAST> ParseTopLevelExpr();
std::unique_ptr<PrototypeAST> ParseExtern();
std::unique_ptr<ExprAST> ParseIfExpr();
std::unique_ptr<ExprAST> ParseForExpr();
std::unique_ptr<ExprAST> ParseBinOpRHS();
std::unique_ptr<ExprAST> ParseUnary();
