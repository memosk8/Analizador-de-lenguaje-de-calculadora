/**
 * @file lexico.cpp
 * @version 1.0
 * @date 10/05/2022
 * @brief Archivo de ejecución del análisis léxico. Genera un vector para almacenar los tokens y abre el
 * archivo pasado por parámetro, y muestra la lista de tokens generados
 * @return EXIT_FAILURE si existe algún error en el análisis y termina la ejecución
 */

#include "lexico.h"

using namespace std;

struct Token
{
    /** Valor del token ej. int, float, ||, &&, etc. */
    char token[256];
    /** Tipo del token.  */
    char type[10];
    /** Número de línea en el que se encuentra el token. */
    unsigned int ln;
    /**
     * @brief Sobrecarga del operador '=' 
     *
     * @param other
     * @return Token&
     */
    Token &operator=(const Token &other)
    {
        if (this == &other)
            return *this;
        strcpy(token, other.token);
        strcpy(type, other.type);
        ln = other.ln;
        return *this;
    }
};

class Lexico
{
    /**
     * @brief Tabla de transición de estados que define los estados posibles
     * para cada caracter del archivo.
     */
    int est[22][16] = {
        //    0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
        //    D   L   "   +   /   =   <   >   &   |   !   D   .   S   E   C
        //    I   E       -   *                           E       A   S   A
        //    G   T                                       L       L   P   R
        //    I   R                                       I       T   T   A
        //    T   A                                       M       O   A   C
        //    O                                           I           B   T
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, e1, 0, 0, e2},            // 0 q0 invalido
        {1, e3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, -1, -1, e2},  // 1 entero
        {2, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},   // 2 id
        {3, 3, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, -1, 3, 3},               // 3 string invalido
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 4 opsr
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 5 opmd
        {-1, -1, -1, -1, -1, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 6 opeq
        {-1, -1, -1, -1, -1, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 7 oplt
        {-1, -1, -1, -1, -1, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 8 opgt
        {-1, -1, -1, -1, -1, -1, -1, -1, 16, -1, -1, -1, -1, -1, -1, e2}, // 9 opai invalido
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, 17, -1, -1, -1, -1, -1, e2}, // 10 opoi invalido
        {-1, -1, -1, -1, -1, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 11 opnot
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 12 del
        {18, e3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 13 reali invalido
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 14 stringv
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 15 oprel
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 16 opav
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 17 opov
        {18, e3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // 18 realv
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // e1 puntoi invalido
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}, // e2 caracteri invalido
        {e3, e3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, e2}  // e3 numeroi invalido
    };

    /// Lista de palabras reservadas dentro de nuestro lenguaje
    char pr[7][10] = {"else", "float", "if", "int", "return", "void", "while"};

    /// Lista de tipos de estados posibles
    char estados[22][10] = {
        "q0", "entero", "id", "stringi",
        "opsr", "opmd", "opeq", "oplt",
        "opgt", "opai", "opoi", "opnot",
        "del", "reali", "stringv", "oprel",
        "opav", "opov", "realv", "puntoi",
        "caracteri", "numeroi"};

    /// lista de estados válidos y no válidos
    char valid[22] = {0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0};

    bool isRW(char *word)
    {
        for (int i = 0; i < 7; ++i)
            if (strcmp(word, pr[i]) == 0)
                return true;
        return false;
    }

public:

    bool analizar(fstream &file, vector<Token> &vt)
    {
        // conteo de errores
        int errCount = 0;
        // iterador del archivo a leer
        std::istreambuf_iterator<char> i(file), e;
        // columna
        int col;
        // estado actual
        int ea = 0;
        // estado previo
        int ep = -1;
        // cursor en buffer
        int n = 0;
        // numero de linea
        int ln = 1;
        // caracter individual
        char s;
        // arreglo de caracteres del buffer
        char buffer[256];

        /// se establece el buffer con carace vacios
        memset(buffer, '\0', 256);

        /// mientras existan caracteres en el archivo...
        while (i != e)
        {
            s = *i;

            if (s >= '0' && s <= '9')
            {
                col = 0;
            }
            else if ((s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z'))
            {
                col = 1;
            }
            else if (s == '\"')
            {
                if (ea == 3 && buffer[n - 1] == '\\')
                    col = 15;
                else
                    col = 2;
            }
            else if (s == '+' || s == '-')
            {
                col = 3;
            }
            else if (s == '*' || s == '/')
            {
                col = 4;
            }
            else if (s == '=')
            {
                col = 5;
            }
            else if (s == '<')
            {
                col = 6;
            }
            else if (s == '>')
            {
                col = 7;
            }
            else if (s == '&')
            {
                col = 8;
            }
            else if (s == '|')
            {
                col = 9;
            }
            else if (s == '!')
            {
                col = 10;
            }
            else if (s == '{' || s == '}' || s == '(' || s == ')' || s == ',' || s == ';')
            {
                col = 11;
            }
            else if (s == '.')
            {
                col = 12;
            }
            else if (s == '\n')
            {
                col = 13;
            }
            else if (s == '\t' || s == ' ')
            {
                col = 14;
            }
            else
            {
                col = 15;
            }

            ep = ea;
            ea = est[ep][col];

            if (ea == -1)
            {
                Token t;
                strcpy(t.token, buffer);
                strcpy(t.type, estados[ep]);
                t.ln = ln;

                if (ep == 2)
                {
                    if (isRW(buffer))
                        strcpy(t.type, "pr");
                }

                if (!valid[ep])
                {
                    std::cout << t.ln << " : " << t.token << " : " << t.type << "\n";
                    errCount++;
                }
                else
                {
                    vt.push_back(t);
                }

                ep = -1;
                ea = 0;
                n = 0;
                memset(buffer, '\0', 256);
            }
            else
            {
                if (ea == 0 && (s == '\t' || s == ' '))
                {
                    ++i;
                }
                else if (s == '\n')
                {
                    ++i;
                    ++ln;
                }
                else
                {
                    buffer[n] = s;
                    ++n;
                    ++i;
                }
            }
        }
        return errCount == 0 ? true : false;
    }
};


