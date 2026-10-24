#include <stdio.h>
#include <stdlib.h>
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
  TOKEN_VIRGOLA,
  TOKEN_TOTALE // non è un token, serve solo a contarli
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

// Stampa a video il carattere e il token associato
void stampaToken(char c, TokenType token){
    printf("Carattere: '%c' (ASCII: %3d) --> %s\n", c, c, nomeToken(token));
}

// Legge tutto il file in un buffer allocato con malloc, terminato da '\0'.
// Restituisce NULL se il file non si apre o la memoria non basta.
// Il chiamante deve fare free() sul risultato.
char *leggiFile(const char *percorso){
    FILE *f = fopen(percorso, "rb");
    if (f == NULL) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long dimensione = ftell(f);
    rewind(f);
    if (dimensione < 0) {
        fclose(f);
        return NULL;
    }

    char *buffer = malloc(dimensione + 1);
    if (buffer == NULL) {
        fclose(f);
        return NULL;
    }

    size_t letti = fread(buffer, 1, dimensione, f);
    // l'a capo finale che molti editor aggiungono non fa parte del json
    while (letti > 0 && (buffer[letti - 1] == '\n' || buffer[letti - 1] == '\r')) {
        letti--;
    }
    buffer[letti] = '\0';
    fclose(f);
    return buffer;
}

int main(int argc, char *argv[]) {
    const char *test_json = TEST_JSON;
    char *da_file = NULL;

    if (argc > 1) {
        da_file = leggiFile(argv[1]);
        if (da_file == NULL) {
            fprintf(stderr, "Impossibile leggere il file '%s'\n", argv[1]);
            return 1;
        }
        test_json = da_file;
        printf("Avvio test sul file %s:\n%s\n\n", argv[1], test_json);
    } else {
        printf("Avvio test hardcoded sulla stringa:\n%s\n\n", test_json);
    }

    int conteggio[TOKEN_TOTALE] = {0};

    for (int i = 0; test_json[i] != '\0'; i++) {
        char c = test_json[i];
        TokenType token = analizzaChar(c);
        conteggio[token]++;
        stampaToken(c, token);
    }

    printf("\nToken trovati:\n");
    for (int t = 0; t < TOKEN_TOTALE; t++) {
        if (conteggio[t] > 0) {
            printf("  %-14s %d\n", nomeToken((TokenType)t), conteggio[t]);
        }
    }

    free(da_file);
    return 0;
}
