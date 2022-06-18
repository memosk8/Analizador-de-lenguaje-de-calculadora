#include "lexico.h"
#include "lexico.cpp"

int main(int argc, char *argv[])
{
    Lexico lex;
    std::vector<Token> tokens;
    int j;

    if (argc < 2)
    {
        std::cout << "Necesita indicar el nombre del código fuente.\n";
        return 0;
    }

    // se abre el archivo que se pasa por parametro
    std::fstream file(argv[1]);

    if (!file)
    {
        std::cout << "Error: no se pudo abrir el archivo.\n\n";
        return 0;
    }

    // retorna false si existe algun error en el analisis y termina la ejecución
    if (!lex.analizar(file, tokens))
    {
        cout << "hubo errores en el analisis Léxico";
        return EXIT_FAILURE;
    }

    cout << "\n----------------------\n\n";
    for (j = 0; j < tokens.size(); ++j)
    {
        std::cout << tokens[j].ln << " : " << tokens[j].token << " : " << tokens[j].type << "\n";
    }

    file.close();
    return 0;
}