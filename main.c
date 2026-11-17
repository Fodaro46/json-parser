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

// Un token che si porta dietro tipo, testo (non copiato: punta nel buffer) e posizione
typedef struct{
    TokenType tipo;
    const char *testo;
    int lunghezza;
    int riga;
    int colonna;
}Token;

// Stampa a video un token con tipo, testo e posizione
void stampaTokenTesto(Token t){
    printf("[riga %d, col %d] %s: %.*s\n", t.riga, t.colonna, nomeToken(t.tipo), t.lunghezza, t.testo);
}

// Aggiunge un token in fondo al vettore, raddoppiando la capienza quando serve.
// Se la memoria finisce il programma si ferma.
void aggiungiToken(Token **vettore, int *quanti, int *capienza, Token t){
    if (*quanti == *capienza) {
        int nuova = (*capienza == 0) ? 64 : *capienza * 2;
        Token *nuovo = realloc(*vettore, nuova * sizeof(Token));
        if (nuovo == NULL) {
            fprintf(stderr, "Memoria esaurita\n");
            exit(1);
        }
        *vettore = nuovo;
        *capienza = nuova;
    }
    (*vettore)[(*quanti)++] = t;
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

// Albero dei valori: ogni valore JSON e' di uno di questi tipi
typedef enum{
    VALORE_NULL,
    VALORE_BOOL,
    VALORE_NUMERO,
    VALORE_STRINGA,
    VALORE_ARRAY,
    VALORE_OGGETTO
}TipoValore;

typedef struct Valore Valore;
struct Valore{
    TipoValore tipo;
    union{
        int booleano;
        double numero;
        char *stringa; // allocata con malloc, senza virgolette
        struct{
            Valore **elementi;
            int quanti;
        } array;
        struct{
            char **chiavi; // chiavi[k] corrisponde a valori[k]
            Valore **valori;
            int quanti;
        } oggetto;
    } dati;
};

// Crea un valore del tipo dato, con i dati a zero. Restituisce NULL se la memoria finisce.
Valore *creaValore(TipoValore tipo){
    Valore *v = calloc(1, sizeof(Valore));
    if (v != NULL) {
        v->tipo = tipo;
    }
    return v;
}

// Libera un valore e, ricorsivamente, tutto quello che contiene.
void liberaValore(Valore *v){
    if (v == NULL) {
        return;
    }
    switch (v->tipo) {
        case VALORE_STRINGA:
            free(v->dati.stringa);
            break;
        case VALORE_ARRAY:
            for (int k = 0; k < v->dati.array.quanti; k++) {
                liberaValore(v->dati.array.elementi[k]);
            }
            free(v->dati.array.elementi);
            break;
        case VALORE_OGGETTO:
            for (int k = 0; k < v->dati.oggetto.quanti; k++) {
                free(v->dati.oggetto.chiavi[k]);
                liberaValore(v->dati.oggetto.valori[k]);
            }
            free(v->dati.oggetto.chiavi);
            free(v->dati.oggetto.valori);
            break;
        default:
            break;
    }
    free(v);
}

// Converte il testo di un token stringa (virgolette comprese, escape gia' validati)
// in una stringa allocata con malloc, senza virgolette e con gli escape risolti.
// \uXXXX diventa UTF-8; le coppie surrogate non vengono unite.
// Restituisce NULL se la memoria finisce. Il chiamante deve fare free().
char *decodificaStringa(const char *testo, int lunghezza){
    // il risultato non e' mai piu lungo del testo: \uXXXX (6 caratteri) vale al massimo 3 byte
    char *uscita = malloc(lunghezza);
    if (uscita == NULL) {
        return NULL;
    }
    int n = 0;
    for (int i = 1; i < lunghezza - 1; i++) {
        char c = testo[i];
        if (c != '\\') {
            uscita[n++] = c;
            continue;
        }
        i++;
        switch (testo[i]) {
            case 'b': uscita[n++] = '\b'; break;
            case 'f': uscita[n++] = '\f'; break;
            case 'n': uscita[n++] = '\n'; break;
            case 'r': uscita[n++] = '\r'; break;
            case 't': uscita[n++] = '\t'; break;
            case 'u': {
                unsigned int codice = 0;
                for (int k = 1; k <= 4; k++) {
                    char h = testo[i + k];
                    codice = codice * 16 + (h <= '9' ? h - '0' : (h | 32) - 'a' + 10);
                }
                i += 4;
                if (codice < 0x80) {
                    uscita[n++] = (char)codice;
                } else if (codice < 0x800) {
                    uscita[n++] = (char)(0xC0 | (codice >> 6));
                    uscita[n++] = (char)(0x80 | (codice & 0x3F));
                } else {
                    uscita[n++] = (char)(0xE0 | (codice >> 12));
                    uscita[n++] = (char)(0x80 | ((codice >> 6) & 0x3F));
                    uscita[n++] = (char)(0x80 | (codice & 0x3F));
                }
                break;
            }
            default: // \" \\ \/
                uscita[n++] = testo[i];
                break;
        }
    }
    uscita[n] = '\0';
    return uscita;
}

// Costruisce il valore di un token semplice: letterale, numero o stringa.
// Restituisce NULL per gli altri token (strutture, FINE) o se la memoria finisce.
Valore *valoreDaToken(Token t){
    Valore *v = NULL;
    switch (t.tipo) {
        case TOKEN_LETTERALE:
            if (t.testo[0] == 'n') {
                v = creaValore(VALORE_NULL);
            } else {
                v = creaValore(VALORE_BOOL);
                if (v != NULL) {
                    v->dati.booleano = (t.testo[0] == 't');
                }
            }
            break;
        case TOKEN_NUMERO: {
            char *copia = malloc(t.lunghezza + 1);
            if (copia == NULL) {
                return NULL;
            }
            memcpy(copia, t.testo, t.lunghezza);
            copia[t.lunghezza] = '\0';
            v = creaValore(VALORE_NUMERO);
            if (v != NULL) {
                v->dati.numero = strtod(copia, NULL);
            }
            free(copia);
            break;
        }
        case TOKEN_STRING:
            v = creaValore(VALORE_STRINGA);
            if (v != NULL) {
                v->dati.stringa = decodificaStringa(t.testo, t.lunghezza);
                if (v->dati.stringa == NULL) {
                    free(v);
                    return NULL;
                }
            }
            break;
        default:
            break;
    }
    return v;
}

// Segnala sul stderr un token diverso da quello atteso
void erroreToken(const char *atteso, Token t){
    fprintf(stderr, "[riga %d, col %d] Atteso %s, trovato %s '%.*s'\n",
            t.riga, t.colonna, atteso, nomeToken(t.tipo), t.lunghezza, t.testo);
}

// Aggiunge un elemento in fondo a un array. Restituisce 0 se la memoria finisce
// (in quel caso l'elemento resta del chiamante).
int aggiungiElemento(Valore *array, Valore *elemento){
    int n = array->dati.array.quanti;
    Valore **nuovi = realloc(array->dati.array.elementi, (n + 1) * sizeof(Valore *));
    if (nuovi == NULL) {
        return 0;
    }
    nuovi[n] = elemento;
    array->dati.array.elementi = nuovi;
    array->dati.array.quanti = n + 1;
    return 1;
}

// Aggiunge una coppia chiave/valore a un oggetto. Restituisce 0 se la memoria finisce
// (in quel caso chiave e valore restano del chiamante).
int aggiungiMembro(Valore *oggetto, char *chiave, Valore *valore){
    int n = oggetto->dati.oggetto.quanti;
    char **chiavi = realloc(oggetto->dati.oggetto.chiavi, (n + 1) * sizeof(char *));
    if (chiavi == NULL) {
        return 0;
    }
    oggetto->dati.oggetto.chiavi = chiavi;
    Valore **valori = realloc(oggetto->dati.oggetto.valori, (n + 1) * sizeof(Valore *));
    if (valori == NULL) {
        return 0;
    }
    chiavi[n] = chiave;
    valori[n] = valore;
    oggetto->dati.oggetto.valori = valori;
    oggetto->dati.oggetto.quanti = n + 1;
    return 1;
}

Valore *parsaValore(const Token *token, int *pos);

// Parser di un array: *pos punta alla '[' e a fine lettura punta dopo la ']'.
Valore *parsaArray(const Token *token, int *pos){
    Valore *array = creaValore(VALORE_ARRAY);
    if (array == NULL) {
        fprintf(stderr, "Memoria esaurita\n");
        return NULL;
    }
    (*pos)++;
    if (token[*pos].tipo == TOKEN_QUADRA_CHIUSA) {
        (*pos)++;
        return array;
    }
    while (1) {
        Valore *elemento = parsaValore(token, pos);
        if (elemento == NULL) {
            liberaValore(array);
            return NULL;
        }
        if (!aggiungiElemento(array, elemento)) {
            fprintf(stderr, "Memoria esaurita\n");
            liberaValore(elemento);
            liberaValore(array);
            return NULL;
        }
        if (token[*pos].tipo == TOKEN_VIRGOLA) {
            (*pos)++;
        } else if (token[*pos].tipo == TOKEN_QUADRA_CHIUSA) {
            (*pos)++;
            return array;
        } else {
            erroreToken("',' o ']'", token[*pos]);
            liberaValore(array);
            return NULL;
        }
    }
}

// Parser di un oggetto: *pos punta alla '{' e a fine lettura punta dopo la '}'.
Valore *parsaOggetto(const Token *token, int *pos){
    Valore *oggetto = creaValore(VALORE_OGGETTO);
    if (oggetto == NULL) {
        fprintf(stderr, "Memoria esaurita\n");
        return NULL;
    }
    (*pos)++;
    if (token[*pos].tipo == TOKEN_GRAFFA_CHIUSA) {
        (*pos)++;
        return oggetto;
    }
    while (1) {
        if (token[*pos].tipo != TOKEN_STRING) {
            erroreToken("una chiave (stringa)", token[*pos]);
            liberaValore(oggetto);
            return NULL;
        }
        char *chiave = decodificaStringa(token[*pos].testo, token[*pos].lunghezza);
        if (chiave == NULL) {
            fprintf(stderr, "Memoria esaurita\n");
            liberaValore(oggetto);
            return NULL;
        }
        (*pos)++;
        if (token[*pos].tipo != TOKEN_DOUBLE_DOT) {
            erroreToken("':'", token[*pos]);
            free(chiave);
            liberaValore(oggetto);
            return NULL;
        }
        (*pos)++;
        Valore *valore = parsaValore(token, pos);
        if (valore == NULL) {
            free(chiave);
            liberaValore(oggetto);
            return NULL;
        }
        if (!aggiungiMembro(oggetto, chiave, valore)) {
            fprintf(stderr, "Memoria esaurita\n");
            free(chiave);
            liberaValore(valore);
            liberaValore(oggetto);
            return NULL;
        }
        if (token[*pos].tipo == TOKEN_VIRGOLA) {
            (*pos)++;
        } else if (token[*pos].tipo == TOKEN_GRAFFA_CHIUSA) {
            (*pos)++;
            return oggetto;
        } else {
            erroreToken("',' o '}'", token[*pos]);
            liberaValore(oggetto);
            return NULL;
        }
    }
}

// Parser ricorsivo: legge un valore dal vettore di token (che finisce con TOKEN_FINE),
// partendo da *pos, e lascia *pos sul primo token dopo il valore.
// Restituisce NULL dopo aver scritto l'errore su stderr.
// La profondita' e' gia' limitata dal controllo sulle parentesi del lexer.
Valore *parsaValore(const Token *token, int *pos){
    Token t = token[*pos];
    if (t.tipo == TOKEN_QUADRA_APERTA) {
        return parsaArray(token, pos);
    }
    if (t.tipo == TOKEN_GRAFFA_APERTA) {
        return parsaOggetto(token, pos);
    }
    Valore *v = valoreDaToken(t);
    if (v == NULL) {
        if (t.tipo == TOKEN_STRING || t.tipo == TOKEN_NUMERO || t.tipo == TOKEN_LETTERALE) {
            fprintf(stderr, "Memoria esaurita\n");
        } else {
            erroreToken("un valore", t);
        }
        return NULL;
    }
    (*pos)++;
    return v;
}

// Cerca una chiave in un oggetto. Restituisce il valore (che resta dell'albero,
// non va liberato) oppure NULL se la chiave non c'e' o se v non e' un oggetto.
// Con chiavi ripetute vale la prima.
Valore *cercaChiave(const Valore *v, const char *chiave){
    if (v == NULL || v->tipo != VALORE_OGGETTO) {
        return NULL;
    }
    for (int k = 0; k < v->dati.oggetto.quanti; k++) {
        if (strcmp(v->dati.oggetto.chiavi[k], chiave) == 0) {
            return v->dati.oggetto.valori[k];
        }
    }
    return NULL;
}

// Cerca un elemento di un array per indice (da 0). Restituisce NULL se l'indice
// e' fuori dall'array o se v non e' un array.
Valore *cercaIndice(const Valore *v, int indice){
    if (v == NULL || v->tipo != VALORE_ARRAY) {
        return NULL;
    }
    if (indice < 0 || indice >= v->dati.array.quanti) {
        return NULL;
    }
    return v->dati.array.elementi[indice];
}

// Nome del tipo di un valore, per la stampa
const char *nomeValore(TipoValore tipo){
    switch (tipo) {
        case VALORE_NULL:    return "NULL";
        case VALORE_BOOL:    return "BOOL";
        case VALORE_NUMERO:  return "NUMERO";
        case VALORE_STRINGA: return "STRINGA";
        case VALORE_ARRAY:   return "ARRAY";
        case VALORE_OGGETTO: return "OGGETTO";
        default:             return "SCONOSCIUTO";
    }
}

int main(int argc, char *argv[]) {
    const char *test_json = TEST_JSON;
    char *da_file = NULL;
    Token *token_letti = NULL;
    int quanti = 0;
    int capienza = 0;

    if (argc > 1) {
        da_file = leggiFile(argv[1]);
        if (da_file == NULL) {
            fprintf(stderr, "Impossibile leggere il file '%s'\n", argv[1]);
            return 1;
        }
        if (da_file[0] == '\0') {
            fprintf(stderr, "Il file '%s' e' vuoto\n", argv[1]);
            free(token_letti);
            free(da_file);
            return 1;
        }
        test_json = da_file;
        printf("Avvio test sul file %s:\n%s\n\n", argv[1], test_json);
    } else {
        printf("Avvio test hardcoded sulla stringa:\n%s\n\n", test_json);
    }

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
                free(token_letti);
                free(da_file);
                return 1;
            }
            lunghezza++;
            int sbagliato = primoEscapeNonValido(test_json + i, lunghezza);
            if (sbagliato >= 0) {
                fprintf(stderr, "[riga %d, col %d] Escape non valido nella stringa\n", riga, colonna + sbagliato);
                free(token_letti);
                free(da_file);
                return 1;
            }
            aggiungiToken(&token_letti, &quanti, &capienza, (Token){TOKEN_STRING, test_json + i, lunghezza, riga, colonna});
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
                    free(token_letti);
                    free(da_file);
                    return 1;
                }
                fprintf(stderr, "[riga %d, col %d] Letterale non valido: '%.*s'\n", riga, colonna, lunghezza, test_json + i);
                free(token_letti);
                free(da_file);
                return 1;
            }
            aggiungiToken(&token_letti, &quanti, &capienza, (Token){TOKEN_LETTERALE, test_json + i, lunghezza, riga, colonna});
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
                free(token_letti);
                free(da_file);
                return 1;
            }
            aggiungiToken(&token_letti, &quanti, &capienza, (Token){TOKEN_NUMERO, test_json + i, lunghezza, riga, colonna});
            colonna += lunghezza;
            i += lunghezza - 1;
            continue;
        }

        // ogni parentesi chiusa deve corrispondere all'ultima aperta
        if (token == TOKEN_GRAFFA_APERTA || token == TOKEN_QUADRA_APERTA) {
            if (profondita == PROFONDITA_MAX) {
                fprintf(stderr, "[riga %d, col %d] Troppo annidato (massimo %d livelli)\n", riga, colonna, PROFONDITA_MAX);
                free(token_letti);
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
                free(token_letti);
                free(da_file);
                return 1;
            }
            if (aperte[profondita - 1] != atteso) {
                fprintf(stderr, "[riga %d, col %d] '%c' non corrisponde a '%c' aperta a riga %d, col %d\n",
                        riga, colonna, c, aperte[profondita - 1],
                        righe_aperte[profondita - 1], colonne_aperte[profondita - 1]);
                free(token_letti);
                free(da_file);
                return 1;
            }
            profondita--;
        }

        // gli spazi non producono token: si salta la stampa ma la posizione avanza
        if (token != TOKEN_SPAZIO) {
            aggiungiToken(&token_letti, &quanti, &capienza, (Token){token, test_json + i, 1, riga, colonna});
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
        free(token_letti);
        free(da_file);
        return 1;
    }

    aggiungiToken(&token_letti, &quanti, &capienza, (Token){TOKEN_FINE, "", 0, riga, colonna});

    for (int k = 0; k < quanti; k++) {
        stampaTokenTesto(token_letti[k]);
    }

    int conteggio[TOKEN_TOTALE] = {0};
    for (int k = 0; k < quanti; k++) {
        conteggio[token_letti[k].tipo]++;
    }

    printf("\nToken trovati:\n");
    for (int t = 0; t < TOKEN_TOTALE; t++) {
        if (conteggio[t] > 0) {
            printf("  %-14s %d\n", nomeToken((TokenType)t), conteggio[t]);
        }
    }

    int posizione = 0;
    Valore *radice = parsaValore(token_letti, &posizione);
    if (radice == NULL) {
        free(token_letti);
        free(da_file);
        return 1;
    }
    if (token_letti[posizione].tipo != TOKEN_FINE) {
        erroreToken("la fine del testo", token_letti[posizione]);
        liberaValore(radice);
        free(token_letti);
        free(da_file);
        return 1;
    }
    printf("\nAlbero costruito: radice di tipo %s\n", nomeValore(radice->tipo));
    liberaValore(radice);

    free(token_letti);
    free(da_file);
    return 0;
}
