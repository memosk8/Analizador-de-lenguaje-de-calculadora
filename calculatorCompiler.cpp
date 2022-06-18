// calculator-notes.cpp

//
// This program implements a basic integer calculator. It consists of three
// main parts:
//
// - A lexical scanner that converts a string, like "(1 + 2)*3", into a
//   sequence of tokens such as {{"(", OPEN_PAREN, 0}, {"1", NUM, 1}, {"+",
//   PLUS, 0}, {"2", NUM, 2}, {")", CLOSE_PAREN, 0}, {"*", TIMES, 0}, {"3",
//   NUM, 3}}.
//
// - A postfix evaluator that evaluates a sequence of tokens in postfix
//   format.
//
// - A transformer that converts a sequence of tokens in infix format into an
//   equivalent sequence of tokens in postfix format. This uses the Shunting
//   Yard algorithm, a non-recursive "precedence climbing" parsing algorithm
//   designed specifically for parsing arithmetic expressions.
//
// When implementing this calculator, it seems best to write the scanner
// first, followed by the postfix evaluator, and then, finally, infix-to-
// postfix transformer.
//
// The main purpose of this program is to demonstrate some basic features of
// C++. As a calculator, it's just a toy. But it could be the starting point
// for a more robust and fully-featured calculator.
//

// #include "cmpt_error.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// Helper functions
//
//////////////////////////////////////////////////////////////////////////

// Returns true if, and only if, c is a whitespace character.
bool is_whitespace(char c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// Returns true if, and only if, c is a decimal digit.
bool is_digit(char c)
{
    return '0' <= c && c <= '9';
}

// Returns a copy of s in double-quotes.
string quote(const string &s)
{
    return "\"" + s + "\"";
}

//////////////////////////////////////////////////////////////////////////
//
// Token_type enumeration
//
//////////////////////////////////////////////////////////////////////////

// ": char" causes each value of this enum to be represented as a char.
enum class Token_type : char
{
    LEFT_PAREN = '(',
    RIGHT_PAREN = ')',
    PLUS = '+',
    BINARY_MINUS = 'm',
    UNARY_MINUS = 'u',
    TIMES = '*',
    DIVIDE = '/',
    NUMBER = 'n' // n for "number"
};

// Overload operator<< so that we can easily print Token_type values.
ostream &operator<<(ostream &os, const Token_type &tt)
{
    os << "'" << char(tt) << "'";
    return os;
}

// Returns true if tt is an operator, and false otherwise.
bool is_op(Token_type tt)
{
    switch (tt)
    {
    case Token_type::TIMES:
        return true;
    case Token_type::DIVIDE:
        return true;
    case Token_type::PLUS:
        return true;
    case Token_type::BINARY_MINUS:
        return true;
    case Token_type::UNARY_MINUS:
        return true;
    default:
        return false;
    } // switch
}

// Returns the precedence of an operator (needed for parsing).
// * has higher precedence than +, so the expression
// "1 + 2 * 3" is evaluated as "1 + (2 * 3)"
int precedence(Token_type tt)
{
    switch (tt)
    {
    case Token_type::UNARY_MINUS:
        return 4;
    case Token_type::TIMES:
        return 3;
    case Token_type::DIVIDE:
        return 3;
    case Token_type::PLUS:
        return 2;
    case Token_type::BINARY_MINUS:
        return 2;
        // default: cmpt::error("precedence default case reached");
    }          // switch
    return -1; // can never be reached!
}

//////////////////////////////////////////////////////////////////////////
//
// Tokens
//
//////////////////////////////////////////////////////////////////////////

// A Token consists of a type, and, for numbers, the value of the number.
struct Token
{
    Token_type type;
    int value; // only used when type == Token_type::NUMBER
};

// Overload operator<< so that we can easily print a single Token.
ostream &operator<<(ostream &os, const Token &t)
{
    if (t.type == Token_type::NUMBER)
    {
        os << "<" << t.value << ">";
    }
    else
    {
        os << "<" << char(t.type) << ">";
    }
    return os;
}

// Sequence is a type synonym for vector<Token>, i.e. Sequence is another name
// for the type vector<Token>.
typedef vector<Token> Sequence;

// Print a vector of Tokens in a nice format.
ostream &operator<<(ostream &os, const Sequence &tokens)
{
    int n = tokens.size();
    if (n == 0)
    {
        os << "{}";
    }
    else if (n == 1)
    {
        os << "{" << tokens[0] << "}";
    }
    else
    {
        os << "{" << tokens[0];
        for (int i = 1; i < tokens.size(); ++i)
        {
            os << ", " << tokens[i];
        }
        os << "}";
    }
    return os;
}

//////////////////////////////////////////////////////////////////////////
//
// Scanning functions
//
//////////////////////////////////////////////////////////////////////////

// Replace non-printing characters like '\\n' with the 2-character string
// "\\n". Also, can replace spaces with '.'s (or some other char) to make them
// easier to see.
string raw(const string &s, const string &dot_char = ".")
{
    string result;
    for (char c : s)
    {
        switch (c)
        {
        case '\n':
            result += "\\n";
            break; // break is needed to stop
        case '\t':
            result += "\\t";
            break; // the flow of control
        case '\r':
            result += "\\r";
            break; // from "falling through"
        case ' ':
            result += dot_char;
            break; // to the next case
        default:
            result += c;
        } // switch
    }     // for
    return result;
}

// The result of a scan is either a Sequence object, or an error. If the value
// is a Sequence, then okay() returns true; if it's an error, then okay()
// returns false.
struct Scan_result
{
    Sequence value;
    string error_msg;

    bool okay() { return error_msg.empty(); }
}; // struct Scan_result

ostream &operator<<(ostream &os, const Scan_result &sr)
{
    os << "Scan_result{" << sr.value << ", " << quote(sr.error_msg) << "}";
    return os;
}

// Convert s into a vector of tokens. Calls cmpt::error on unknown characters.
// Throws std::out_of_range if an out-of-range int is encountered.
// A '-' is always treat as a Token_type::BINARY_MINUS (unary minuses will
// be fixed later in the process)
Scan_result scan(const string &s)
{
    Sequence result;
    for (int i = 0; i < s.size(); i++)
    {
        if (is_whitespace(s[i]))
        {
            // skip all whitespace
            i++;
            while (i < s.size() && is_whitespace(s[i]))
                i++;
            // invariant: i >= s.size() || !is_whitespace(s[i])

            // If we're not at the end of the string, then decrement i because
            // we had to look one character ahead to see that it wasn't
            // whitespace.
            if (i < s.size())
                i--;
        }
        else if (is_digit(s[i]))
        {
            string num = string(1, s[i]);
            i++;
            while (i < s.size() && is_digit(s[i]))
            {
                num += string(1, s[i]);
                i++;
            }
            // invariant: i >= s.size() || !is_digit(s[i])

            // If we're not at the end of the string, then decrement i because
            // we had to look one character ahead to see that it wasn't a
            // digit.
            if (i < s.size())
                i--;
            result.push_back(Token{Token_type::NUMBER, stoi(num)});
        }
        else if (s[i] == '(')
        {
            result.push_back(Token{Token_type::LEFT_PAREN, 0});
        }
        else if (s[i] == ')')
        {
            result.push_back(Token{Token_type::RIGHT_PAREN, 0});
        }
        else if (s[i] == '+')
        {
            result.push_back(Token{Token_type::PLUS, 0});
        }
        else if (s[i] == '-')
        {
            result.push_back(Token{Token_type::BINARY_MINUS, 0});
        }
        else if (s[i] == '*')
        {
            result.push_back(Token{Token_type::TIMES, 0});
        }
        else if (s[i] == '/')
        {
            result.push_back(Token{Token_type::DIVIDE, 0});
        }
        else
        {
            string msg = "scanner encountered unknown character '" + string(1, s[i]) + "'";
            return Scan_result{Sequence{}, msg};
        }
    } // for
    return Scan_result{result, ""};
} // scan

// Distinguishing between unary - (as in -5) and binary - (as in 1 - 2).
// Examples of unary -:
//
//   -5
//   1 + -3
//   -1 + 3
//  -(1 + 2)
//  3 - -2
//  (-2 * 6)
//  -1--2
//
//  A - is unary if:
//
//    * it is the first token of an expression, e.g. -5, -(1 + 2)
//    * the previous token is an operator or a (, e.g. 1 + -3, (-2 * 6)
//
// All other instances of - are assumed to be binary.
//
void minus_fix(Sequence &seq)
{
    // a - at the start of a sequence is always considered unary
    if (seq[0].type == Token_type::BINARY_MINUS)
    {
        seq[0].type = Token_type::UNARY_MINUS;
    }
    for (int i = 1; i < seq.size(); ++i)
    {
        Token_type prev = seq[i - 1].type;
        if (seq[i].type == Token_type::BINARY_MINUS)
        {
            if (prev == Token_type::LEFT_PAREN || is_op(prev))
            {
                seq[i].type = Token_type::UNARY_MINUS;
            }
        }
    } // for
}

//////////////////////////////////////////////////////////////////////////
//
// Postfix evaluator
//
//////////////////////////////////////////////////////////////////////////

// Removes an item from the end of a vector and returns it. It is a template
// function, which means it works a vector<T>, where T is any type.
template <class T>
T pop(vector<T> &stack)
{
    T result = stack.back();
    stack.pop_back();
    return result;
}

// The result of a scan is either a Sequence object, or an error. If the value
// is a Sequence, then okay() returns true; if it's an error, then okay()
// returns false.
struct Int_result
{
    int value;
    string error_msg;

    bool okay() { return error_msg.empty(); }
}; // struct Int_result

ostream &operator<<(ostream &os, const Int_result &ir)
{
    os << "Int_result{" << ir.value << ", " << quote(ir.error_msg) << "}";
    return os;
}

// Assumes tokens form a valid postfix expression.
Int_result postfix_eval(const Sequence &tokens)
{
    vector<int> stack;
    bool ok = true; // error flag  that is set to false if there are not
                    // enough numbers on the stack to evaluate an operator

    // Scan through every token. Push numbers onto the stack. For operators,
    // pop off the necessary number of items from the stack, evaluate them,
    // and push the result.
    for (Token tok : tokens)
    {
        if (tok.type == Token_type::NUMBER)
        {
            stack.push_back(tok.value);
        }
        else if (tok.type == Token_type::UNARY_MINUS)
        {
            // Unary operators take one input. Thus, if the stack has no
            // numbers on it we immediately set an error flag and jump out of
            // the loop.
            if (stack.size() < 1)
            {
                ok = false;
                break; // jump out of loop
            }
            int a = pop(stack);
            stack.push_back(-a);
        }
        else
        {
            // Binary operators take two inputs. Thus, if the stack has less
            // than 2 numbers on it we immediately set an error flag and jump
            // out of the loop.
            if (stack.size() < 2)
            {
                ok = false;
                break; // jump out of loop
            }
            int a = pop(stack);
            int b = pop(stack);
            switch (tok.type)
            {
            case Token_type::PLUS:
                stack.push_back(a + b);
                break;
            case Token_type::BINARY_MINUS:
                stack.push_back(b - a);
                break;
            case Token_type::TIMES:
                stack.push_back(a * b);
                break;
            case Token_type::DIVIDE:
                if (a == 0.0)
                {
                    return Int_result{0, "division by 0"};
                }
                stack.push_back(b / a);
                break;
                // default:
                // An unknown token type is a *really* bad error that
                // means there's a mistake in the program's design.
                // cmpt::error("postfix_eval error: unknown token type '"
                //             + string(1, char(tok.type)) + "'");
            } // switch
        }     // else
    }         // for
    if (ok && stack.size() > 0)
    {
        return Int_result{stack[0], ""};
    }
    else
    {
        return Int_result{0, "not enough numbers to pop"};
    }
} // postfix_eval

Int_result postfix_eval(const string &expr)
{
    Scan_result tokens = scan(expr);
    if (tokens.okay())
    {
        Int_result result = postfix_eval(tokens.value);
        return result;
    }
    else
    {
        return Int_result{0, tokens.error_msg};
    }
}

//////////////////////////////////////////////////////////////////////////
//
// Infix evaluator
//
//////////////////////////////////////////////////////////////////////////

//
// Use the shunting-yard algorithm to convert infix to postfix.
//
// Assumes tokens vector forms a infix expression.
// See: https://en.wikipedia.org/wiki/Shunting-yard_algorithm
//
// Some invalid expressions, like "(1 + 2", are caught in this function, while
// others, like "1(+)2" or "1 + + 2", are caught only in the postfix
// evaluation. That's pretty confusing for the user! Consistent, helpful error
// messages would be a good improvement to this program.
Scan_result infix_to_postfix(const Sequence &input)
{
    Sequence output;
    Sequence stack;
    for (const Token &tok : input)
    {
        switch (tok.type)
        {
        case Token_type::NUMBER:
            // numbers are always immediately pushed to output
            output.push_back(tok);
            break;
        case Token_type::PLUS:
        case Token_type::UNARY_MINUS:
        case Token_type::BINARY_MINUS:
        case Token_type::TIMES:
        case Token_type::DIVIDE:
            // pop higher-precedence operators off the stack
            while (!stack.empty() && (is_op(stack.back().type) && precedence(stack.back().type) >= precedence(tok.type)))
            {
                output.push_back(pop(stack));
            } // while
            stack.push_back(tok);
            break;
        case Token_type::LEFT_PAREN:
            // left parentheses are always immediately pushed onto the stack
            stack.push_back(tok);
            break;
        case Token_type::RIGHT_PAREN:
            // pop operators until the first left parenthesis is reached
            // (if no parenthesis is found, then there's a mis-matched
            // parenthesis error)
            while (!stack.empty() && (stack.back().type != Token_type::LEFT_PAREN))
            {
                output.push_back(pop(stack));
            } // while
            if (stack.empty())
            {
                return Scan_result{Sequence{}, "mis-matched parenthesis"};
            }
            else if (stack.back().type == Token_type::LEFT_PAREN)
            {
                // discard the left parenthesis on the top
                pop(stack);
            }
            else
            {
                // If the program ever gets to this line then there is a
                // serious problem with it's design.
                // cmpt::error("logic error in infix_to_postfix RIGHT_PAREN case!");
            }
            break;
        default:
            cout << "tok.type: '" << char(tok.type) << "'\n";
            // cmpt::error("reached default in infix_to_postfix!");
        } // switch
    }     // for

    // The output is all processed, but there might be tokens on the stack. So
    // move them all to output.
    while (!stack.empty())
    {
        Token t = pop(stack);
        if (t.type == Token_type::LEFT_PAREN || t.type == Token_type::RIGHT_PAREN)
        {
            return Scan_result{Sequence{}, "mis-matched parenthesis"};
        }
        output.push_back(t);
    } // while
    return Scan_result{output, ""};
} // infix_to_postfix

Int_result infix_eval(Sequence input)
{
    minus_fix(input);
    Scan_result postfix = infix_to_postfix(input);
    if (postfix.okay())
    {
        return postfix_eval(postfix.value);
    }
    else
    {
        return Int_result{0, postfix.error_msg};
    }
}

Int_result infix_eval(const string &input)
{
    Scan_result tokens = scan(input);
    if (tokens.okay())
    {
        return infix_eval(tokens.value);
    }
    else
    {
        return Int_result{0, tokens.error_msg};
    }
}

//////////////////////////////////////////////////////////////////////////
//
// Testing functions
//
//////////////////////////////////////////////////////////////////////////

void repl_postfix()
{
    for (;;)
    { // loop forever
        cout << "postfix> ";

        // getline reads the entire line of input, including spaces;
        // cin >> input would just read the characters before the first space
        // (e.g. the first word)
        string input;
        getline(cin, input);
        cout << "input=" << quote(raw(input)) << "\n";

        auto result = postfix_eval(input);
        if (result.okay())
        {
            cout << "val = " << result.value << "\n";
        }
        else
        {
            cout << "Error: " << result.error_msg << "\n";
        }
    } // for
}

void repl_infix()
{
    for (;;)
    { // loop forever
        cout << "\ninfix> ";
        string input;

        // getline reads the entire line of input, including spaces;
        // cin >> input would just read the characters before the first space
        // (e.g. the first word)
        getline(cin, input);
        cout << "input=" << quote(raw(input)) << "\n";

        Int_result result = infix_eval(input);
        if (result.okay())
        {
            cout << "val = " << result.value << "\n";
        }
        else
        {
            cout << "Error: " << result.error_msg << "\n";
        }
    } // for
}

//////////////////////////////////////////////////////////////////////////
//
// Testing
//
//////////////////////////////////////////////////////////////////////////

int test_count = 0;

void test(const string &expr, int expected_result)
{
    ++test_count;
    Int_result result = infix_eval(expr);
    if (result.okay() && result.value == expected_result)
    {
        // do nothing --- test passed
        cout << test_count << ": " << quote(expr) << " passed\n";
    }
    else
    {
        cout << "!! Test failed: " << quote(expr)
             << "\n!! result=" << result
             << ", expected=" << expected_result << "\n";
        // cmpt::error("test failed");
    }
}

void infix_eval_test()
{
    cout << "Testing infix_eval ...\n";
    test_count = 0;
    test("0", 0);
    test("10", 10);
    test("  1    +2", 3);
    test("4*2", 8);
    test("  10/ 5", 2);
    test("1+2", 3);
    test("(1)+2", 3);
    test("1+(2)", 3);
    test("(1+2)", 3);
    test("286", 286);
    test("(286)", 286);
    test("((286))", 286);
    test("(((286)))", 286);
    test("1+2+3 + (4 + 5) + 7", 22);
    test("2 + 3 * 4", 14);
    test("(2 + 3) * 4", 20);
    test("2 * 3 + 4", 10);
    test("(10+20) /  15 ", 2);
    test("6 * 5 - (1 * 2 + 3 * 4 )", 16);
    test("32 / 2 ", 16);
    test("32 / 2 / 2", 8);
    test("(8 - 6 / 2) / (8-6/2)", 1);
    test("(1 - 1 + 4) * 2", 8);
    test("(155 + 2 + 3 - 155 - 2 - 3 + 4) * 2", 8);
    test("32 / 2 * 2", 32);

    test("1-2  ", -1);
    test("-(1 + 2)", -3);
    test("-5", -5);
    test("1 + -3", -2);
    test("-1 + 3", 2);
    test("-(1 + 2)", -3);
    test("3 - -2", 5);
    test("-2 * 6", -12);
    test("-(-2 * -6)", -12);

    cout << "\n... all infix_eval tests passed!\n";
}

//////////////////////////////////////////////////////////////////////////
//
// Main program
//
//////////////////////////////////////////////////////////////////////////

int main()
{
    infix_eval_test();
    // repl_postfix();
    repl_infix();
} // main