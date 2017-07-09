
.PHONY: clean

all: sats

setup:
	cd ./build/ ; \
    cmake ../src/c

clean:
	rm -rf ./build/*

code-docs:
	doxygen doxyfile

project-docs:
	mkdocs build --clean --site-dir ./build/docs

parser:
	flex -o src/c/sat_expression_scanner.c \
	     --header-file=src/c/sat_expression_scanner.h \
            src/c/sat_expression_scanner.l
	bison -o src/c/sat_expression_parser.c  src/c/sat_expression_parser.y

libsatsolver: parser
	$(MAKE) -C ./build satsolver

sats: libsatsolver
	$(MAKE) -C ./build sats
