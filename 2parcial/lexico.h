
/**
 * @file    lexico.h
 * @version 1.0
 * @date    09/05/2022
 * @title   Cabeceras para analizador léxico
 * @brief   Declaración de la estructura del token, tabla de transiciones de estados
 *          palabras reservadas, estados válidos y obtención de columnas
 *
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

#define e1 19 // puntoi invalido
#define e2 20 // caracteri invalido
#define e3 21 // numeroi invalido

using namespace std;

/**
 * @brief Estructura que define cada token en el archivo analizado.
 */
struct Token;

/**
 * @brief Clase que contiene tabla de transiciones de estados, palabras reservadas,
 * estados válidos y obtención de columnas.
 */
// class Lexico
// {

//     ///@brief Tabla de transición de estados que define los estados posibles para cada caracter del archivo.
//     int est[22][16];

//     /// Lista de palabras reservadas dentro de nuestro lenguaje
//     char pr[7][10];

//     /// Lista de tipos de estados posibles
//     char estados[22][10];

//     /// lista de estados válidos y no válidos
//     char valid[22];

//     /**
//      * @brief Verifica si una palabra es reservada.
//      *
//      * @param word Palabra por analizar
//      * @return true si la palabra es reservada | false si la palabra no es reservada
//      */
//     bool isRW(char *word);

// public:
//     /**
//      *  @brief Se encarga de analizar cada uno de los caracteres del archivo para obtener su respectiva columna
//      *  y por ende el estado respectivo
//      *
//      *  @param file Archivo por analizar
//      *  @param vt Vector de tipo Token para almacenar los tokens generados
//      *  @return False si existe algun error en el analisis | True si no hubo ningun error
//      */
//     bool analizar(fstream &file, vector<Token> &vt);
// };
