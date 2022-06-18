#include <string.h>

struct Token
{
   char token[1];
   char type[1];
   int nl;

   Token &operator=(const Token &other)
   {
      if (this == other)
         return *this;
      strcpy(token, other.token);
      strcpy(type, other.type);
      nl = other.nl;
      return *this;
   }
};