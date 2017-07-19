
# Debug or release build?
BUILD_TYPE=DEBUG

# With or without coverage flags?
WITH_COVERAGE=NO

# Include gprof support?
WITH_GPROF=YES

# Include OpenMP support?
WITH_OPENMP=YES

#-----------------------------------------------------------------------------

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

# The default test file to run when using the 'run-test' target.
TEST=tests/000-simple.txt

#-----------------------------------------------------------------------------

# Building for debug or release?
ifeq ("$(BUILD_TYPE)" , "DEBUG")
    CFLAGS+=-g -O0
else ifeq ("$(BUILD_TYPE)" , "RELEASE")
    CFLAGS+=-O2
else
    $(error BUILD_TYPE must be 'DEBUG' or 'RELEASE'. Got '$(BUILD_TYPE)')
endif

# Build with coverage collection support?
ifeq ("$(WITH_COVERAGE)" , "YES")
    CFLAGS+=--coverage -O0
else ifeq ("$(WITH_COVERAGE)" , "NO")
else
    $(error WITH_COVERAGE must be 'YES' or 'NO'. Got '$(WITH_COVERAGE)')
endif

# Build with OpenMP Support?
ifeq ("$(WITH_OPENMP)" , "YES")
    CFLAGS+=-fopenmp
else ifeq ("$(WITH_OPENMP)" , "NO")
else
    $(error WITH_OPENMP must be 'YES' or 'NO'. Got '$(WITH_OPENMP)')
endif

# Build with profiling support?
ifeq ("$(WITH_GPROF)" , "YES")
    CFLAGS+=-pg
else ifeq ("$(WITH_GPROF)" , "NO")
else
    $(error WITH_GPROF must be 'YES' or 'NO'. Got '$(WITH_GPROF)')
endif


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

run-test: $(BIN_FILE)
	$(BIN_FILE) $(TEST)

clean-tests:
	rm -rf ./build/test_vectors
	rm -rf ./build/test_logs

test-vectors:
	./bin/test-harness.py

run-tests: test-vectors
	./bin/run-tests.sh
