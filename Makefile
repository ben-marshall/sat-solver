
# Debug or release build?
BUILD_TYPE=DEBUG

# With or without coverage flags?
WITH_COVERAGE=YES

# Root directory we do all compilation in.
BUILD_ROOT=./build

# Where all the source files are.
SRC_ROOT=./src/c

# Parser generated files
FLEX_IN=$(SRC_ROOT)/parser/sat-expression-scanner.l
FLEX_COUT=$(BUILD_ROOT)/sat-expression-scanner.c
FLEX_HOUT=$(BUILD_ROOT)/sat-expression-scanner.h

BISON_IN=$(SRC_ROOT)/parser/sat-expression-parser.y
BISON_OUT=$(BUILD_ROOT)/sat-expression-parser.c

INC_DIRS=-I$(SRC_ROOT) -I$(BUILD_ROOT)

# Executable Source Files.
SRC_FILES=$(FLEX_COUT) \
          $(BISON_OUT) \
          $(BUILD_ROOT)/sat-expression.c \
          $(BUILD_ROOT)/imp-matrix.c \
          $(BUILD_ROOT)/main.c

OBJ_FILES=$(SRC_FILES:.c=.o)

# Executable output file
BIN_FILE=$(BUILD_ROOT)/sats

CC=gcc

CFLAGS+=-Wall $(INC_DIRS)

#-----------------------------------------------------------------------------

all: $(FLEX_HOUT) $(FLEX_COUT) $(BISON_OUT) $(OBJ_FILES) $(BIN_FILE)

setup:
	mkdir -pf $(BUILD_ROOT)

#-----------------------------------------------------------------------------

#
# Rule: Builds C source files from flex input files.
#
$(FLEX_HOUT) $(FLEX_COUT) : $(FLEX_IN)
	flex -o $(FLEX_COUT) --header-file=$(FLEX_HOUT) $(FLEX_IN)

#
# Rule: Builds C source files from bison input files.
#
$(BISON_OUT) : $(BISON_IN) $(FLEX_HOUT)
	bison -o $@ $<

#
# Rule: Copy files from source folder to build folder.
#
build/%.c : src/c/%.c
	cp $< $@

#
# Rule: Build object file from C source file.
#
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $< -lm

#
# Rule: Link object files into executable.
#
$(BIN_FILE) : $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES) -lm

#-----------------------------------------------------------------------------

docs:
	doxygen doxyfile
	mkdocs build --clean --site-dir ./build/docs


#-----------------------------------------------------------------------------

clean-tests:
	rm -rf ./build/test_vectors
	rm -rf ./build/test_logs

test-vectors:
	./bin/test-harness.py

run-tests: test-vectors
	./bin/run-tests.sh
