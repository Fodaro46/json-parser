
#include <stdlib.h>
#include <string.h>
 //IL LEXER DEVE LEGGERE UN CARATTERE E CLASSIFICARLO
typedef enum{
  TOKEN_STRING,
  TOKEN_GRAFFA,
  TOKEN_DOUBLE_DOT,
  TOKEN_NONE,
}TokenType;

int main(){
  return 0;
}

TokenType analizzaChar(char assegno){
	switch(assegno):
		case "{":
			return TOKEN_GRAFFA
		case ":":
			return TOKEN_DOUBLE_DOT
		case typedef(string):
                     	return TOKEN_STRING
                default:
			retun TOKEN_NONE
}
                return TOKEN_NONE
}
