
#include <stdlib.h>
#include <string.h>
 //IL LEXER DEVE LEGGERE UN CARATTERE E CLASSIFICARLO
typedef enum{
  TOKEN_STRING,
  TOKEN_GRAFFA,
  TOKEN_DOUBLE_DOT,
  TOKEN_NONE,
  TOKEN_LETTERA
}TokenType;

int main(){
  return 0;
}

TokenType analizzaChar(char assegno){
	switch(assegno){
		case '{':
			return TOKEN_GRAFFA;
		case '}':
			return TOKEN_GRAFFA;
		case ':':
			return TOKEN_DOUBLE_DOT;
		case '"':
                     	return TOKEN_STRING;
                default:
			return TOKEN_LETTERA;
}
		return TOKEN_NONE;
}


