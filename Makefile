# Ghenescu Stefan 312CA

CC=gcc
CFLAGS=-Wall -Wextra -std=c99

TARGETS = image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) $(CFLAGS) image_editor.c -o image_editor -lm

pack:
	zip -FSr 312CA_GhenescuStefan_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean