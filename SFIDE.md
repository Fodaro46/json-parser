# Sfide

Niente soluzioni qui, solo il testo. Si risolvono sul progetto o in file a parte.
Spuntare quando fatte.

## Giornaliere (circa 20-40 minuti)

- [ ] **Spazi bianchi.** Il lexer deve saltare spazi, tab e a capo senza produrre token. Input: `{ "a" :   1 }` deve dare gli stessi token di `{"a":1}`.
- [ ] **Stringa intera.** Invece di classificare un carattere alla volta, restituire l'intera stringa tra virgolette come un solo token. Cosa succede con `"ciao\"mondo"`?
- [ ] **Numeri.** Riconoscere interi e negativi: `0`, `42`, `-7`. Scrivere i casi che devono fallire (`-`, `--1`, `1-`).
- [ ] **Decimali ed esponenti.** Estendere i numeri a `3.14`, `-0.5`, `1e10`, `2.5E-3`. Attenzione a `01` e `.5`, che in JSON non sono validi.
- [ ] **Letterali.** Riconoscere `true`, `false`, `null`. `tru` e `nulll` devono essere errori.
- [ ] **Posizione.** Ogni token deve portarsi dietro riga e colonna. Servirà per i messaggi di errore.
- [ ] **Bilanciamento.** Dato un testo, dire se `{}` e `[]` sono bilanciati e annidati correttamente, ignorando quelli dentro le stringhe. `[{]}` non è valido.
- [ ] **Escape.** Gestire `\n`, `\t`, `\\`, `\"` e `\uXXXX` dentro le stringhe.
- [ ] **Lettura da file.** Caricare tutto il contenuto di un file in un buffer allocato con `malloc`, con la dimensione ricavata dal file. Gestire file inesistente e file vuoto.
- [ ] **Messaggi di errore.** Quando il lexer trova un carattere inatteso, stampare riga, colonna e il carattere trovato, poi uscire con codice diverso da 0.

## Settimanali (qualche ora)

- [ ] **Settimana 1: lexer completo.** Una funzione che prende il testo e produce un array di token (tipo, testo, posizione). Deve passare su tutti i file di una cartella `tests/` con casi validi e non validi, scritti da te.
- [ ] **Settimana 2: parser ricorsivo.** Dai token costruire un albero di valori (oggetto, array, stringa, numero, bool, null). Definire la struttura dati e la `free` ricorsiva. Verificare l'assenza di leak con `valgrind` o gli address sanitizer.
- [ ] **Settimana 3: accesso ai dati.** Funzioni per cercare per chiave in un oggetto e per indice in un array, con percorsi tipo `a.b[2].c`. Cosa restituire se il percorso non esiste?
- [ ] **Settimana 4: stampa.** Serializzare l'albero di nuovo in JSON, sia compatto sia indentato. Il risultato, riletto dal parser, deve produrre un albero uguale.
- [ ] **Settimana 5: robustezza.** Limite di profondità per evitare lo stack overflow su `[[[[[...` molto annidati. Gestire file grandi senza caricarli due volte. Provare input casuali e cercare i crash.
- [ ] **Settimana 6: confronto.** Prendere un file di qualche MB, misurare il tempo del tuo parser e confrontarlo con un parser esistente. Trovare dove perdi tempo.

## Extra

- [ ] Accettare i commenti `//` e `/* */` come opzione, tenendo il default rigoroso.
- [ ] Parsing in streaming: non costruire l'albero, chiamare una funzione per ogni valore letto.
- [ ] Scrivere un piccolo `jq` che stampa solo il campo richiesto da riga di comando.
