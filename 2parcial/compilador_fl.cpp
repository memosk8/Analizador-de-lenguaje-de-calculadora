#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

#define e1 19
#define e2 20
#define e3 21

struct Token
{
    char token[256];
    char type[10];
    unsigned int ln;
};

class Lexico
{
    int est[22][16] = {
        //    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
        //
        //    D  L  "  +  /  =  <  >  &  |  !  D  .  S  E  C
        //    I  E     -  *                    E     A  S  A
        //    G  T                             L     L  P  R
        //    I  R                             I     T  T  A
        //    T  A                             M     O  A  C
        //    O                                I        B  T
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

    char pr[7][10] = {"else", "float", "if", "int", "return", "void", "while"};

    char estados[22][10] = {"q0",        // 0
                            "entero",    // 1
                            "id",        // 2
                            "stringi",   // 3
                            "opsr",      // 4
                            "opmd",      // 5
                            "opeq",      // 6
                            "oplt",      // 7
                            "opgt",      // 8
                            "opai",      // 9
                            "opoi",      // 10
                            "opnot",     // 11
                            "del",       // 12
                            "reali",     // 13
                            "stringv",   // 14
                            "oprel",     // 15
                            "opav",      // 16
                            "opov",      // 17
                            "realv",     // 18
                            "puntoi",    // e1
                            "caracteri", // e2
                            "numeroi"};  // e3

    // 0 1 2 3 4 5 6 7 8 9101112131415161718192021
    char valid[22] = {0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0};

    bool isRW(char *word)
    {
        for (int i = 0; i < 7; ++i)
            if (strcmp(word, pr[i]) == 0)
                return true;
        return false;
    }

public:
    void analizar(std::fstream &file, std::vector<Token> &vt)
    {
        std::istreambuf_iterator<char> i(file), e;
        int col, ep = -1, ea = 0, n = 0, ln = 1;
        char s, buffer[256];

        memset(buffer, '\0', 256);
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
    }
};

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

    std::fstream file(argv[1]);

    if (!file)
    {
        std::cout << "Error: no se pudo abrir el archivo.\n\n";
        return 0;
    }

    lex.analizar(file, tokens);
    std::cout << "\n----------------------\n\n";
    for (j = 0; j < tokens.size(); ++j)
    {
        std::cout << tokens[j].ln << " : " << tokens[j].token << " : " << tokens[j].type << "\n";
    }

    file.close();
    return 0;
}
