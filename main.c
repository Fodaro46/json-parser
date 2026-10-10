
#include <stdio.h>
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
  TOKEN_LETTERA,
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
const char *nomeToken(TokenType t){
	switch(t){
		case TOKEN_STRING:        return "STRING";
		case TOKEN_GRAFFA_APERTA: return "GRAFFA_APERTA";
		case TOKEN_GRAFFA_CHIUSA: return "GRAFFA_CHIUSA";
		case TOKEN_DOUBLE_DOT:    return "DOUBLE_DOT";
		case TOKEN_QUADRA_APERTA: return "QUADRA_APERTA";
		case TOKEN_QUADRA_CHIUSA: return "QUADRA_CHIUSA";
		case TOKEN_VIRGOLA:       return "VIRGOLA";
		case TOKEN_LETTERA:       return "LETTERA";
		default:                  return "NONE";
	}
}
int main() {
    // Stringa JSON di test hardcoded
    const char *test_json = "{\"chiave\": \"valore\"}";
    
    printf("Avvio test hardcoded sulla stringa:\n%s\n\n", test_json);
    
    for (int i = 0; test_json[i] != '\0'; i++) {
        char c = test_json[i];
        TokenType token = analizzaChar(c);
        
        // Stampa a video del carattere e del token associato
        printf("Carattere: '%c' (ASCII: %3d) --> %s\n", c, c, nomeToken(token));
    }
    
    return 0;
}
// TODO: applicare la malloc per leggere direttamente i file




