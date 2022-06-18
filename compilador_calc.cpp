
/*
Tokens:
	operadores: '+' '-' '*' '/'
	simbolos: '(' ')' '\n'
	números enteros: [0-9]
	números reales: [0-9]*'.'[0-9]+
	ignorado: '\t',' ','\r'
*/

/*
Reglas de derivación:
	 <linea> ::= <exp> \n
	 <exp> ::= <exp real> | <exp entera>
	 <exp entera> ::= entero | <exp entera> + <exp entera>
		  | <exp entera> - <exp entera>
		  | <exp entera> * <exp entera>
		  | ( <exp entera> )
	 <exp real> ::= real
		  | <exp real > + <exp real >
		  | <exp real > - <exp real >
		  | <exp real > * <exp real >
		  | <exp real > / <exp real >
		  | ( <exp real > )
	 */

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostream;
using std::string;
using std::vector;

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
	NUMBER = 'n' // number
};

struct Token
{
	Token_type type;
	int value; // usado cuando type == Token_type::NUMBER
	int ln;	  // número de línea
};

// vector de caracteres del archivo fuente
vector<char> chars;

// vector de tokens resultante
vector<Token> tokens;

// sobrecarga el operator<< para imprimir un simple token facilmente
// ostream &operator<<(ostream &os, const Token &t)
// {
// 	if (t.type == Token_type::NUMBER)
// 	{
// 		os << "<" << t.value << ">";
// 	}
// 	else
// 	{
// 		os << "<" << char(t.type) << ">";
// 	}
// 	return os;
// }

// abre el archivo, lo lee y guarda los chars en vector<char> chars
int readFile(string filepath)
{
	char byte = 0;
	ifstream input_file(filepath);
	if (!input_file.is_open())
	{
		cerr << "Error al abrir el archivo -> '" << filepath << "'" << endl;
		return EXIT_FAILURE;
	}
	while (input_file.get(byte))
		chars.push_back(byte); // obtener un vector con los caracteres
	input_file.close();
	return EXIT_SUCCESS;
}

// Retorna True solo si c es un caracter vacío
bool is_space(char c)
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// Retorna True solo si c es un dígito decimal
bool is_digit(char c)
{
	return '0' <= c && c <= '9';
}

// Retorna una copia de s(string) en comillas dobles
string quote(const string &s)
{
	return "\"" + s + "\"";
}

// checa si dado tipo de token es válido
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

// retorna la precedencia de un operador (necesario para el parseo (sintáctico))
// * tiene mayor precedencia que +, entonces la expresión :
// "1 + 2 * 3" es evaluada como "1 + (2 * 3)"
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
	}
	return -1; // no puede ser alcanzado
}

// un tipo de sinónimo para vector<Token>
typedef vector<Token> Sequence;

// El resultado del análisis léxico es o un objeto Sequence o un error.
// Si es una secuencia, entonces la función okay() retorna true
// Si es un error retorna false
struct Sintactic_result
{
	Sequence value;
	string error_msg;
	bool okay() { return error_msg.empty(); }
};

Sintactic_result lex(vector<char> s)
{
	int ln;			  // número de línea
	Sequence result; // secuencia resultante
	for (int i = 0; i < s.size(); i++)
	{
		char actual_char = s[i];
		if (is_space(actual_char))
		{
			// saltar todo el espacio blanco
			i++;
			while (i < s.size() && is_space(actual_char))
				i++;

			// si no estamos al final de la cadena, entonces decrementar i
			// porque se checó un caracter adelante para ver si no era espacio vacío
			if (i < s.size())
				i--;
		}
		else if (is_digit(actual_char))
		{
			string num = string(1, actual_char);
			i++;
			while (i < s.size() && is_digit(actual_char))
			{
				num += string(1, actual_char);
				i++;
			}

			// si no estamos al final de la cadena, entonces decrementar i
			// porque se checó un caracter adelante para ver si no era dígito
			if (i < s.size())
				i--;
			result.push_back(Token{Token_type::NUMBER, stoi(num), ln});
		}
		else if (actual_char == '(')
			result.push_back(Token{Token_type::LEFT_PAREN, 0, ln});

		else if (actual_char == ')')
			result.push_back(Token{Token_type::RIGHT_PAREN, 0, ln});

		else if (actual_char == '+')
			result.push_back(Token{Token_type::PLUS, 0, ln});

		else if (actual_char == '-')
			result.push_back(Token{Token_type::BINARY_MINUS, 0, ln});

		else if (actual_char == '*')
			result.push_back(Token{Token_type::TIMES, 0, ln});

		else if (actual_char == '/')
			result.push_back(Token{Token_type::DIVIDE, 0, ln});

		else if (actual_char == '\n')
			ln++;
		else
		{
			string msg = "scanner encountered unknown character '" + string(1, actual_char) + "'";
			return Sintactic_result{Sequence{}, msg};
		}
	} // for
	return Sintactic_result{result, ""};
} // scan

int main(int argc, char *argv[])
{
	// error si no existe ruta de archivo como parametro
	if (argc < 2)
	{
		std::cout << "Necesita indicar el nombre del código fuente.\n";
		return 0;
	}

	readFile(argv[1]);
	Sintactic_result seq = lex(chars);
	for (int i = 0; i < seq.value.size(); i++)
	{
		//TODO: mostrar por consola la lista de tokens generada 
	}

	return 0;
}
