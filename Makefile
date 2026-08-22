# NOTE SO NO ONE THINKS I CHEATED
# THIS WAS PRETTY MUCH WRITTEN BY CLAUDE BECAUSE
# I MADE THIS PROJECT TO LEARN C, NOT MAKEFILE
# I WROTE AN ORIGINAL VERSION THAT DID THE JOB
# BUT THIS IS A MUCH NICER DYNAMIC VERSION THAT TAKES
# WILDCARD PATHS AND NESTING INTO ACCOUNT SO I DONT NEED
# TO EDIT IT EVERY TIME I ADD A NEW C FILE. 


ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
    CLEANUP = del /F /Q
    CLEANUP_DIR = rmdir /S /Q
    MKDIR = mkdir
  else # in a bash-like shell, like msys
    CLEANUP = rm -f
    CLEANUP_DIR = rm -rf
    MKDIR = mkdir -p
  endif
  TARGET_EXTENSION = exe
else
  CLEANUP = rm -f
  CLEANUP_DIR = rm -rf
  MKDIR = mkdir -p
  TARGET_EXTENSION = out
endif

.PHONY: clean run test main fresh

PATHU = libs/unity/
PATHS = src/
PATHT = test/
PATHB = build/
PATHD = build/depends/
PATHO = build/objs/
PATHR = build/results/

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR)

SRCT = $(wildcard $(PATHT)test-*.c)

# GNU Make's $(wildcard) only matches one directory level, so plain
# $(wildcard $(PATHS)*.c) would miss e.g. src/parser/parser.c. This
# recurses: take *.c directly in $1, then recurse into every
# subdirectory of $1 and do the same.
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
SRC = $(call rwildcard,$(PATHS),*.c)

# Object files derived from actual sources instead of a hardcoded list,
# so adding a new .c file to src/ is picked up automatically.
OBJ = $(patsubst $(PATHS)%.c,$(PATHO)%.o,$(SRC))

# All src/ objects except main.o. main.c defines its own main(), which
# would collide with test.c's main() if both were linked into the same
# binary — so the test binary excludes it and links test.o instead.
OBJ_NO_MAIN = $(filter-out $(PATHO)main.o,$(OBJ))

# One object per test-*.c file.
TEST_OBJ = $(patsubst $(PATHT)test-%.c,$(PATHO)test-%.o,$(SRCT))

COMPILE = gcc -c
LINK    = gcc -Wall -Werror
DEPEND  = gcc -MM -MG
CFLAGS  = -I. -I$(PATHU) -I$(PATHS)

RELEASE_FLAGS = -O2 -DNDEBUG
TEST_FLAGS = -DTEST
DEBUG_FLAGS = -g -O0 -DDEBUG

# A single combined test binary, so a single result/log file.
RESULTS = $(PATHR)test.txt
DEPS    = $(patsubst $(PATHT)%.c,$(PATHD)%.d,$(wildcard $(PATHT)*.c))

PASSED = `grep -s PASS $(PATHR)*.txt`
FAIL   = `grep -s FAIL $(PATHR)*.txt`
IGNORE = `grep -s IGNORE $(PATHR)*.txt`

main: CFLAGS += $(RELEASE_FLAGS)
main: $(PATHB)main.$(TARGET_EXTENSION)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: main

run: main
	./$(PATHB)main.$(TARGET_EXTENSION)

fails: CFLAGS += $(TEST_FLAGS)
fails: | $(BUILD_PATHS)
fails: $(RESULTS)
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "\nDONE"

test: CFLAGS += $(TEST_FLAGS)
test: | $(BUILD_PATHS)
test: $(RESULTS)
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------\nPASSED:\n-----------------------"
	@echo "$(PASSED)"
	@echo "\nDONE"
	@! grep -srq FAIL $(PATHR)*.txt

test-norun: $(PATHB)test.$(TARGET_EXTENSION)

# Run a compiled test binary and capture its output as the result file.
$(PATHR)%.txt: $(PATHB)%.$(TARGET_EXTENSION) | $(PATHR)
	-./$< > $@ 2>&1

# Link the production binary from every object under src/.
$(PATHB)main.$(TARGET_EXTENSION): $(OBJ) | $(PATHB)
	$(LINK) -o $@ $^ -lcurl

# Link a test binary from: test.c's object (has main(), calls into the
# test_* functions defined in each test-*.c), every test-*.o, every src/
# object except main.o (main.c's own main() would collide with test.c's),
# and unity.o.
$(PATHB)test.$(TARGET_EXTENSION): $(PATHO)test.o $(TEST_OBJ) $(OBJ_NO_MAIN) $(PATHO)unity.o | $(PATHB)
	$(LINK) -o $@ $^ -lcurl

# Compiles any .c under src/, including nested subdirectories, into a
# matching object path (src/parser/parser.c -> build/objs/parser/parser.o).
# $(PATHO) alone (the order-only prereq) only guarantees the top-level
# build/objs/ exists, not build/objs/parser/, so mkdir -p that too.
$(PATHO)%.o: $(PATHS)%.c | $(PATHO)
	@$(MKDIR) $(dir $@)
	$(COMPILE) $(CFLAGS) $< -o $@

# Compile test.c (the runner with main(), calls the test_* functions
# from each test-*.c). An explicit (non-%) rule always takes precedence
# over a pattern rule, so this can't collide with the %.o rule above.
$(PATHO)test.o: $(PATHT)test.c | $(PATHO)
	$(COMPILE) $(CFLAGS) $< -o $@

# Compile test-*.c files (the test_* function definitions themselves).
$(PATHO)test-%.o: $(PATHT)test-%.c | $(PATHO)
	$(COMPILE) $(CFLAGS) $< -o $@

# Compile Unity itself.
$(PATHO)unity.o: $(PATHU)unity.c | $(PATHO)
	$(COMPILE) $(CFLAGS) $< -o $@

# Auto-generated header dependencies for everything under test/, so
# editing a header triggers a rebuild of test.o and/or the affected
# test-*.o.
$(PATHD)%.d: $(PATHT)%.c | $(PATHD)
	$(DEPEND) -MT $(PATHO)$*.o -MF $@ $(CFLAGS) $<

-include $(DEPS)

$(PATHB):
	$(MKDIR) $(PATHB)

$(PATHD):
	$(MKDIR) $(PATHD)

$(PATHO):
	$(MKDIR) $(PATHO)

$(PATHR):
	$(MKDIR) $(PATHR)

clean:
	$(CLEANUP_DIR) $(PATHB)

fresh: 
	$(MAKE) clean
	$(MAKE) main
	./$(PATHB)main.$(TARGET_EXTENSION)

.PRECIOUS: $(PATHD)%.d
.PRECIOUS: $(PATHO)%.o
.PRECIOUS: $(PATHR)%.txt