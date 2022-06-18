#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

// vector de caracteres del archivo fuente
vector<char> chars;
// vector de tokens resultante
vector<Token> tokens;

/*
Tokens:
   operadores: '+' '-' '*' '/'
   simbolos: '(' ')' '\n'
   números enteros: [0-9]
   números reales: [0-9]*'.'[0-9]+
   ignorado: 'λ' | ' ' '\t'
*/

// <linea> ::= <exp> \n
// <exp> ::= <exp real> | <exp entera>
// <exp entera> ::= entero | <exp entera> + <exp entera>
// 	| <exp entera> - <exp entera>
// 	| <exp entera> * <exp entera>
// 	| ( <exp entera> )
// <exp real> ::= real
// 	| <exp real > + <exp real >
// 	| <exp real > - <exp real >
// 	| <exp real > * <exp real >
// 	| <exp real > / <exp real >
// 	| ( <exp real > )

// ": char" hace que cada valor en la enum se represente como un char
enum class Token_type : char
{
   LEFT_PAREN = '(',
   RIGHT_PAREN = ')',
   PLUS = '+',
   BINARY_MINUS = 'm',
   UNARY_MINUS = 'u',
   TIMES = '*',
   DIVIDE = '/',
   NUMBER = 'n' //"number"
};

struct Token
{
   Token_type type;
   int value; // usado cuando type == Token_type::NUMBER
};

// abre el archivo, lo lee y guarda los chars en vector<char> caracteres
int readFile(string archivo)
{
   char byte = 0;
   ifstream input_file(archivo);
   if (!input_file.is_open())
   {
      cerr << "Could not open the file - '" << archivo << "'" << endl;
      return EXIT_FAILURE;
   }
   while (input_file.get(byte))
      chars.push_back(byte);
   input_file.close();
   return EXIT_SUCCESS;
}

// Retorna True solo si c es un caracter vacío
bool is_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// Retorna True solo si c es un dígito decimal
bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

// Returns a copy of s in double-quotes.
string quote(const string& s) {
    return "\"" + s + "\"";
}

// checa si dado tipo de token es valido
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
    }
}

bool lex(vector<char>)
{
   bool done = false;

   // crear token por cada elemento valido

   for (int i = 0; i < chars.size(); i++)
   {
      char c = chars[i];

   
   }

   return done;
}

int main(int argc, char *argv[])
{
   // error con el archivo
   if (argc < 2)
   {
      std::cout << "Necesita indicar el nombre del código fuente.\n";
      return 0;
   }

   readFile(argv[1]);
   lex(chars);

   // printChars();

   return 0;
}
