# json-parser

Progetto per capire come funziona un parser JSON, scritto in C.

## Come funziona un parser JSON

Un parser prende del testo e lo trasforma in una struttura dati che il programma può usare.
Di solito il lavoro si divide in due fasi.

### 1. Lexer (analisi lessicale)

Legge il testo carattere per carattere e lo spezza in **token**, cioè i pezzi minimi che hanno un significato:

- simboli di struttura: `{` `}` `[` `]` `:` `,`
- stringhe tra virgolette
- numeri
- le parole `true`, `false`, `null`

Gli spazi e gli a capo vengono saltati.

Esempio: `{"a": 1}` diventa `{`, `"a"`, `:`, `1`, `}`.

### 2. Parser (analisi sintattica)

Prende la sequenza di token e controlla che rispetti la grammatica del JSON, costruendo man mano l'albero dei valori.
Un valore JSON può essere:

- un oggetto (coppie chiave/valore)
- un array
- una stringa
- un numero
- `true`, `false` o `null`

Oggetti e array possono contenere altri valori, quindi il parser è tipicamente **ricorsivo**: per leggere un oggetto legge una chiave, poi un valore, che a sua volta può essere un oggetto, e così via.

### Errori

Se i token non rispettano la grammatica (una virgola di troppo, una graffa non chiusa, una stringa senza chiusura) il parser deve fermarsi e segnalare l'errore, meglio se con la posizione nel testo.

## Compilare

```
gcc -Wall -o parser main.c
./parser
```

## Struttura

- `main.c` sorgente
- `test.json` file di prova
- `SFIDE.md` esercizi per andare avanti
