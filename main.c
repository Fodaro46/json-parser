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
  TOKEN_LETTERA,
  TOKEN_SPAZIO,
  TOKEN_NUMERO,
  TOKEN_VIRGOLA,
  TOKEN_LETTERALE, // true, false o null
  TOKEN_FINE, // fine del testo
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
		case '\0':
			return TOKEN_FINE;
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
		case TOKEN_FINE:          return "FINE";
		case TOKEN_LETTERA:       return "LETTERA";
		case TOKEN_LETTERALE:     return "LETTERALE";
		case TOKEN_SPAZIO:        return "SPAZIO";
		case TOKEN_NUMERO:        return "NUMERO";
		default:                  return "SCONOSCIUTO";
	}
}
// Stringa JSON di test hardcoded
#define TEST_JSON "{\"chiave\": \"valore\"}"

#define PROFONDITA_MAX 1024

// Stampa a video il carattere e il token associato
void stampaToken(char c, TokenType token, int riga, int colonna){
    if (c == '\0') {
        printf("[riga %d, col %d] Carattere: '\\0' (ASCII: %3d) --> %s\n", riga, colonna, c, nomeToken(token));
    } else {
        printf("[riga %d, col %d] Carattere: '%c' (ASCII: %3d) --> %s\n", riga, colonna, c, c, nomeToken(token));
    }
}

// Stampa a video un numero intero (piu caratteri) come singolo token
void stampaNumero(const char *testo, int lunghezza, int riga, int colonna){
    printf("[riga %d, col %d] Numero: '%.*s' --> %s\n", riga, colonna, lunghezza, testo, nomeToken(TOKEN_NUMERO));
}

// Stampa a video una stringa intera (virgolette comprese) come singolo token
void stampaStringa(const char *testo, int lunghezza, int riga, int colonna){
    printf("[riga %d, col %d] Stringa: %.*s --> %s\n", riga, colonna, lunghezza, testo, nomeToken(TOKEN_STRING));
}

// Stampa a video un letterale (true, false, null) come singolo token
void stampaLetterale(const char *testo, int lunghezza, int riga, int colonna){
    printf("[riga %d, col %d] Letterale: %.*s --> %s\n", riga, colonna, lunghezza, testo, nomeToken(TOKEN_LETTERALE));
}

// Vero solo se il testo e' esattamente "true", "false" o "null".
// Rifiuta "tru" e "nulll".
int letteraleValido(const char *testo, int lunghezza){
    const char *ammessi[] = {"true", "false", "null"};
    for (int k = 0; k < 3; k++) {
        if ((int)strlen(ammessi[k]) == lunghezza && strncmp(testo, ammessi[k], lunghezza) == 0) {
            return 1;
        }
    }
    return 0;
}

// Controlla le sequenze di escape dentro una stringa (virgolette comprese):
// ammessi \" \\ \/ \b \f \n \r \t e \uXXXX con quattro cifre esadecimali.
// Restituisce -1 se e' tutto valido, altrimenti la posizione (da 0, a partire
// dalla virgoletta di apertura) del backslash che apre l'escape sbagliato.
int primoEscapeNonValido(const char *testo, int lunghezza){
    for (int i = 1; i < lunghezza - 1; i++) {
        if (testo[i] != '\\') {
            continue;
        }
        int inizio = i;
        i++;
        switch (testo[i]) {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                break;
            case 'u':
                for (int k = 1; k <= 4; k++) {
                    char h = (i + k < lunghezza - 1) ? testo[i + k] : '\0';
                    int esadecimale = (h >= '0' && h <= '9') || (h >= 'a' && h <= 'f') || (h >= 'A' && h <= 'F');
                    if (!esadecimale) {
                        return inizio;
                    }
                }
                i += 4;
                break;
            default:
                return inizio;
        }
    }
    return -1;
}

// Vero per i caratteri che possono comparire dentro un numero JSON.
int carattereNumero(char c){
    return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E';
}

// Controlla la grammatica JSON dei numeri:
// '-' facoltativo, parte intera (0 oppure cifre senza zero iniziale),
// parte decimale facoltativa (. e almeno una cifra),
// esponente facoltativo (e/E, segno facoltativo, almeno una cifra).
// Rifiuta "-", "--1", "1-", "01", ".5", "1." e "1e".
int numeroValido(const char *testo, int lunghezza){
    int i = 0;
    if (testo[i] == '-') {
        i++;
    }
    if (i >= lunghezza) {
        return 0;
    }
    if (testo[i] == '0') {
        i++;
    } else if (testo[i] >= '1' && testo[i] <= '9') {
        while (i < lunghezza && testo[i] >= '0' && testo[i] <= '9') {
            i++;
        }
    } else {
        return 0;
    }

    if (i < lunghezza && testo[i] == '.') {
        i++;
        int cifre = 0;
        while (i < lunghezza && testo[i] >= '0' && testo[i] <= '9') {
            i++;
            cifre++;
        }
        if (cifre == 0) {
            return 0;
        }
    }

    if (i < lunghezza && (testo[i] == 'e' || testo[i] == 'E')) {
        i++;
        if (i < lunghezza && (testo[i] == '+' || testo[i] == '-')) {
            i++;
        }
        int cifre = 0;
        while (i < lunghezza && testo[i] >= '0' && testo[i] <= '9') {
            i++;
            cifre++;
        }
        if (cifre == 0) {
            return 0;
        }
    }

    return i == lunghezza;
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
        if (da_file[0] == '\0') {
            fprintf(stderr, "Il file '%s' e' vuoto\n", argv[1]);
            free(da_file);
            return 1;
        }
        test_json = da_file;
        printf("Avvio test sul file %s:\n%s\n\n", argv[1], test_json);
    } else {
        printf("Avvio test hardcoded sulla stringa:\n%s\n\n", test_json);
    }

    int conteggio[TOKEN_TOTALE] = {0};
    int riga = 1;
    int colonna = 1;

    // pila delle parentesi aperte, con la posizione per i messaggi di errore
    char aperte[PROFONDITA_MAX];
    int righe_aperte[PROFONDITA_MAX];
    int colonne_aperte[PROFONDITA_MAX];
    int profondita = 0;

    for (int i = 0; test_json[i] != '\0'; i++) {
        char c = test_json[i];
        TokenType token = analizzaChar(c);

        // dalla virgoletta di apertura a quella di chiusura e' un solo token;
        // la virgoletta preceduta da backslash non chiude la stringa
        if (token == TOKEN_STRING) {
            int lunghezza = 1;
            while (test_json[i + lunghezza] != '\0' && test_json[i + lunghezza] != '"') {
                if (test_json[i + lunghezza] == '\\' && test_json[i + lunghezza + 1] != '\0') {
                    lunghezza++;
                }
                lunghezza++;
            }
            if (test_json[i + lunghezza] == '\0') {
                fprintf(stderr, "[riga %d, col %d] Stringa non terminata\n", riga, colonna);
                free(da_file);
                return 1;
            }
            lunghezza++;
            int sbagliato = primoEscapeNonValido(test_json + i, lunghezza);
            if (sbagliato >= 0) {
                fprintf(stderr, "[riga %d, col %d] Escape non valido nella stringa\n", riga, colonna + sbagliato);
                free(da_file);
                return 1;
            }
            conteggio[token]++;
            stampaStringa(test_json + i, lunghezza, riga, colonna);
            for (int k = 0; k < lunghezza; k++) {
                if (test_json[i + k] == '\n') {
                    riga++;
                    colonna = 1;
                } else {
                    colonna++;
                }
            }
            i += lunghezza - 1;
            continue;
        }

        // le lettere consecutive devono formare true, false o null
        if (token == TOKEN_LETTERA) {
            int lunghezza = 1;
            while (analizzaChar(test_json[i + lunghezza]) == TOKEN_LETTERA) {
                lunghezza++;
            }
            if (!letteraleValido(test_json + i, lunghezza)) {
                // un letterale inizia solo con t, f o n: altrimenti il primo carattere e' inatteso
                if (c != 't' && c != 'f' && c != 'n') {
                    fprintf(stderr, "[riga %d, col %d] Carattere inatteso: '%c' (ASCII: %d)\n", riga, colonna, c, c);
                    free(da_file);
                    return 1;
                }
                fprintf(stderr, "[riga %d, col %d] Letterale non valido: '%.*s'\n", riga, colonna, lunghezza, test_json + i);
                free(da_file);
                return 1;
            }
            conteggio[TOKEN_LETTERALE]++;
            stampaLetterale(test_json + i, lunghezza, riga, colonna);
            colonna += lunghezza;
            i += lunghezza - 1;
            continue;
        }

        // le cifre consecutive formano un solo token numerico
        if (token == TOKEN_NUMERO) {
            int lunghezza = 1;
            while (carattereNumero(test_json[i + lunghezza])) {
                lunghezza++;
            }
            if (!numeroValido(test_json + i, lunghezza)) {
                fprintf(stderr, "[riga %d, col %d] Numero non valido: '%.*s'\n", riga, colonna, lunghezza, test_json + i);
                free(da_file);
                return 1;
            }
            conteggio[token]++;
            stampaNumero(test_json + i, lunghezza, riga, colonna);
            colonna += lunghezza;
            i += lunghezza - 1;
            continue;
        }

        // ogni parentesi chiusa deve corrispondere all'ultima aperta
        if (token == TOKEN_GRAFFA_APERTA || token == TOKEN_QUADRA_APERTA) {
            if (profondita == PROFONDITA_MAX) {
                fprintf(stderr, "[riga %d, col %d] Troppo annidato (massimo %d livelli)\n", riga, colonna, PROFONDITA_MAX);
                free(da_file);
                return 1;
            }
            aperte[profondita] = c;
            righe_aperte[profondita] = riga;
            colonne_aperte[profondita] = colonna;
            profondita++;
        } else if (token == TOKEN_GRAFFA_CHIUSA || token == TOKEN_QUADRA_CHIUSA) {
            char atteso = (token == TOKEN_GRAFFA_CHIUSA) ? '{' : '[';
            if (profondita == 0) {
                fprintf(stderr, "[riga %d, col %d] '%c' senza parentesi aperta\n", riga, colonna, c);
                free(da_file);
                return 1;
            }
            if (aperte[profondita - 1] != atteso) {
                fprintf(stderr, "[riga %d, col %d] '%c' non corrisponde a '%c' aperta a riga %d, col %d\n",
                        riga, colonna, c, aperte[profondita - 1],
                        righe_aperte[profondita - 1], colonne_aperte[profondita - 1]);
                free(da_file);
                return 1;
            }
            profondita--;
        }

        // gli spazi non producono token: si salta la stampa ma la posizione avanza
        if (token != TOKEN_SPAZIO) {
            conteggio[token]++;
            stampaToken(c, token, riga, colonna);
        }

        if (c == '\n') {
            riga++;
            colonna = 1;
        } else {
            colonna++;
        }
    }

    if (profondita > 0) {
        fprintf(stderr, "[riga %d, col %d] '%c' aperta a riga %d, col %d e mai chiusa\n",
                riga, colonna, aperte[profondita - 1],
                righe_aperte[profondita - 1], colonne_aperte[profondita - 1]);
        free(da_file);
        return 1;
    }

    TokenType fine = analizzaChar('\0');
    conteggio[fine]++;
    stampaToken('\0', fine, riga, colonna);

    printf("\nToken trovati:\n");
    for (int t = 0; t < TOKEN_TOTALE; t++) {
        if (conteggio[t] > 0) {
            printf("  %-14s %d\n", nomeToken((TokenType)t), conteggio[t]);
        }
    }

    free(da_file);
    return 0;
}
