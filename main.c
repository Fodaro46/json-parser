
#include <stdlib.h>
#include <string.h>
 //IL LEXER DEVE LEGGERE UN CARATTERE E CLASSIFICARLO
typedef enum{
  TOKEN_STRING,
  TOKEN_GRAFFA_APERTA,
  TOKEN_GRAFFA_CHIUSA,
  TOKEN_DOUBLE_DOT,
  TOKEN_QUADRA_APERTA,
  TOKEN_QUADRA_CHIUSA,
  TOKEN_NONE,
  TOKEN_LETTERA
  TOKEN_VIRGOLA
}TokenType;

TokenType analizzaChar(char assegno){
	switch(assegno){
		case '{':
			return TOKEN_GRAFFA_APERTA;
		case '}':
			return TOKEN_GRAFFA_CHIUSA;
		case '[':
			return TOKEN_QUADRA_APERTA;
		case ']':
			return TOKEN_QUADRA_CHIUSA;
		case ':':
			return TOKEN_DOUBLE_DOT;
		case ',':
			return TOKEN_VIRGOLA;
		case '"':
                     	return TOKEN_STRING;

                default:
			return TOKEN_LETTERA;
}
		return TOKEN_NONE;
}
int main(){
  //da applicare hardcoded costante per testing ma useless 
  // applicare la malloc per leggere direttamente i file
  return analizzaChar('{');
}



