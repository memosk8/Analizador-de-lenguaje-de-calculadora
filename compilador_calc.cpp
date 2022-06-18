
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

struct Token
{
	char type;
	string value; // usado cuando type == Token_type::NUMBER
	int ln;		  // número de línea
};

// vector de caracteres del archivo fuente
vector<char> chars;

// vector de tokens resultante
vector<Token> tokens;

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
	{
		chars.push_back(byte); // obtener un vector con los caracteres
		cout << byte;
	}

	input_file.close();
	return EXIT_SUCCESS;
}

// Retorna True solo si c es un caracter vacío
bool is_space(char c)
{
	return c == ' ' || c == '\r' || c == '\t';
}

// Retorna True solo si c es un dígito decimal
bool is_digit(char c)
{
	return '0' <= c && c <= '9';
}

// checa si dado tipo de token es válido
bool is_op(char tt)
{
	switch (tt)
	{
	case '*':
		return true;
	case '/':
		return true;
	case '+':
		return true;
	case '-':
		return true;
	// case Token_type::UNARY_MINUS:
	// 	return true;
	default:
		return false;
	}
}

int main(int argc, char *argv[])
{
	// error si no existe ruta de archivo como parametro
	if (argc < 2)
	{
		std::cout << "Necesita indicar el nombre del código fuente.\n";
		return 0;
	}

	if (readFile(argv[1]) == 0)
		cout << "archivo leido" << endl;

	int ln = 1; // número de línea

	for (int i = 0; i < chars.size(); i++)
	{
		char actual_char = chars[i];

		if (is_space(actual_char))
		{
			i++;
			if (actual_char == 10)
				ln++;
		}

		else if (is_digit(actual_char))
		{
			string num = string(1, actual_char);
			i++;
			while (i < chars.size() && is_digit(chars[i + 1]))
			{
				num += string(1, actual_char);
				i++;
			}

			if (i < chars.size())
				i--;

			tokens.push_back(Token{'n', num, ln});
		}

		else if (actual_char == '(')
		{
			tokens.push_back(Token{actual_char, string(1, 0), ln});
		}

		else if (actual_char == ')')
			tokens.push_back(Token{actual_char, string(1, 0), ln});

		else if (is_op(actual_char))
			tokens.push_back(Token{actual_char, string(1, 0), ln});

		else
			cout << "caracter invalido" << endl;
	} // for

	cout << tokens.size() << "____" << endl;

	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].value.size() > 0)
			cout << " | Tipo: " << string(1, tokens[i].type) << " | Val: " << tokens[i].value << endl;
		else
			cout << " | Tipo: " << string(1, tokens[i].type) << endl;
	}

	return 0;
}
