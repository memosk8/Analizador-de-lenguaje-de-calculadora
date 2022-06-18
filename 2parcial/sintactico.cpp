#include "sintactico.h"

void Sintactico::sigToken()
{
   ++pos;
   actual = vt[pos];
   if (pos + 1 == vt.size())
      next = vt[pos + 1];
   else
   {
      strcpy(next.token, " ");
      strcpy(next.type, "eof");
   }
}

bool Sintactico::analizar(std::vector<Token> &tokens)
{
   vt = tokens;
   pos = 0;
   actual = vt[pos];
   next = vt[pos + 1];
   if (programa())
      return true;
   else
   {
      cout << "Errores\n";
      return false;
   }
}

bool Sintactico::asignacion()
{
   if (actual.type == "int" || actual.type == "float" || actual.type == "void")
   {
      if(next.type == "id"){
         
      }
   }
   else return false;
}
