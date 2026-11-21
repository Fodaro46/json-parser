CC = gcc
CFLAGS = -Wall

build:
	$(CC) $(CFLAGS) -o parser main.c

run: build
	./parser

clean:
	rm -f parser parser.exe

# Esegue il parser su tutti i json di tests/ e controlla il codice di uscita:
# i file con nome da errore devono fallire, gli altri passare.
test: build
	@fail=0; \
	for f in tests/*.json; do \
		case $$f in \
			tests/numero_*|tests/letterale_*|tests/carattere_*|tests/parser_*|tests/vuoto.json|tests/stringa_non_chiusa.json|tests/escape_sconosciuto.json|tests/escape_unicode_*|tests/bilanciamento_incrociato.json|tests/bilanciamento_non_chiusa.json|tests/bilanciamento_chiusa_in_piu.json) atteso=1;; \
			*) atteso=0;; \
		esac; \
		./parser $$f >/dev/null 2>&1; codice=$$?; \
		if [ $$codice -ne $$atteso ]; then echo "FALLITO: $$f (uscita $$codice, attesa $$atteso)"; fail=1; fi; \
	done; \
	if [ $$fail -eq 0 ]; then echo "Tutti i test passati"; fi; \
	exit $$fail
	@./parser tests/annidato.json 'a.b[2].c' | grep -q 'Trovato: STRINGA d' || { echo "FALLITO: percorso a.b[2].c"; exit 1; }
	@./parser tests/annidato.json 'a.b[1]' | grep -q 'Trovato: NUMERO 2' || { echo "FALLITO: percorso a.b[1]"; exit 1; }
	@if ./parser tests/annidato.json 'a.x' >/dev/null 2>&1; then echo "FALLITO: a.x doveva non trovare"; exit 1; fi
	@if ./parser tests/annidato.json 'a.b[9]' >/dev/null 2>&1; then echo "FALLITO: a.b[9] doveva non trovare"; exit 1; fi
	@if ./parser tests/annidato.json 'a..b' >/dev/null 2>&1; then echo "FALLITO: a..b doveva essere errato"; exit 1; fi
	@./parser tests/annidato.json --compatto | grep -qxF '{"a":{"b":[1,2,{"c":"d"}]}}' || { echo "FALLITO: --compatto su annidato.json"; exit 1; }
	@./parser tests/spazi.json --compatto | grep -qxF '{"a":1}' || { echo "FALLITO: --compatto su spazi.json"; exit 1; }
	@./parser tests/escape_validi.json --compatto | grep -qxF '["a\nb\tc\\d\"e/f\bg\fh\ri","èèA"]' || { echo "FALLITO: --compatto su escape_validi.json"; exit 1; }
