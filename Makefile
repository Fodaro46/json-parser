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
			tests/numero_*|tests/letterale_*|tests/carattere_*|tests/vuoto.json|tests/stringa_non_chiusa.json|tests/escape_sconosciuto.json|tests/escape_unicode_*|tests/bilanciamento_incrociato.json|tests/bilanciamento_non_chiusa.json|tests/bilanciamento_chiusa_in_piu.json) atteso=1;; \
			*) atteso=0;; \
		esac; \
		./parser $$f >/dev/null 2>&1; codice=$$?; \
		if [ $$codice -ne $$atteso ]; then echo "FALLITO: $$f (uscita $$codice, attesa $$atteso)"; fail=1; fi; \
	done; \
	if [ $$fail -eq 0 ]; then echo "Tutti i test passati"; fi; \
	exit $$fail
