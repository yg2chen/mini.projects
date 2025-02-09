#pragma once

enum Token {
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5,

    // if else block
    tok_if = -6,
    tok_then = -7,
    tok_else = -8,

    // for loop block
    tok_for = -9,
    tok_in = -10,

    // binary
    tok_binary = -11,
    tok_unary = -12,
};
