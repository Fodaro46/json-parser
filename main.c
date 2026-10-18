
#include <stdio.h>
 //IL LEXER DEVE LEGGERE UN CARATTERE E CLASSIFICARLO
typedef enum{
  TOKEN_STRING,
  TOKEN_GRAFFA_APERTA,
  TOKEN_GRAFFA_CHIUSA,
  TOKEN_DOUBLE_DOT,
  TOKEN_QUADRA_APERTA,
  TOKEN_QUADRA_CHIUSA,
  TOKEN_LETTERA,
  TOKEN_SPAZIO,
  TOKEN_NUMERO,
  TOKEN_VIRGOLA
}TokenType;

// Classifica un singolo carattere: restituisce il token di struttura
// corrispondente, TOKEN_STRING per le virgolette, TOKEN_LETTERA per il resto.
// Cifre, '-' e '.' contano come TOKEN_NUMERO.
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
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			return TOKEN_SPAZIO;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
		case '.':
			return TOKEN_NUMERO;
		default:
			return TOKEN_LETTERA;
	}
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
		case TOKEN_SPAZIO:        return "SPAZIO";
		case TOKEN_NUMERO:        return "NUMERO";
		default:                  return "SCONOSCIUTO";
	}
}
// Stringa JSON di test hardcoded
#define TEST_JSON "{\"chiave\": \"valore\"}"

int main(void) {
    const char *test_json = TEST_JSON;
    
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




