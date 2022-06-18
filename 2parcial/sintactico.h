#include "lexico.h"
#include "lexico.cpp"

class Sintactico
{
   // Token analizado actualente
   Token actual;
   // Token siguiente en la lista
   Token next;
   // posición en la lista?
   int pos = 0;
   // lista de tokens
   std::vector<Token> vt;

   //Evalúa si un token es igual a un caracter
   bool matchToken();
   // evalúa si un token es igual a un tipo de dato
   bool matchType();

   // devuelve el seiguiente token en la lista
   void sigToken();
   // función para anlizar la lista de tokens del lexico
   bool analizar(std::vector<Token> &tokens);
   // regla de producción para funcion main | <int> <main>(P) {I}
   bool _main();
   // regla de producción para funciones :: F -> F f | f
   bool funciones();
   // regla de producción para función :: f -> T <id> (P) {I return}
   bool funcion();
   // regla de produccción para llamada a función :: llf -> <id>(P);
   bool llf();
   // regla de producción para parámetros :: P -> p,P | p
   bool parametros();
   // regla de producción para parámetro :: p -> id | llf | val
   bool parametro();
   // regla de producción para bloque de instrucciones :: I -> i[I]
   bool bloqueinstruccion();
   // regla de producción para una instrucción :: i -> D | A | <if> | IE | W | <return> | llf
   bool instruccion();
   // regla de producción para una declaración :: D -> T <id> ;
   bool declaracion();
   // regla de producción para un tipo de dato :: T -> <int> | <float> | <void>
   bool tipodato();
   // regla de producción para una asignación :: A -> T <id> = E | <id> | llf
   bool asignacion();
   // regla de producción para una expresión :: E -> operacionand [ || exp]
   bool expresion();
   // regla de producción para una estructura if-else :: IE -> <if> {I} <else> {I}
   bool ifelse();
   // regla de producción para una estructura while :: W -> <while> (EL) {I}
   bool _while();
};
