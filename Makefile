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

.PHONY: clean run test test-norun fails main debug fresh

PATHU = libs/unity/
PATHS = src/
PATHT = test/
PATHB = build/
PATHD = build/depends/
PATHR = build/results/

# Objects live in separate per-variant directories (release/debug/test)
# instead of one shared build/objs/. Make only checks timestamps, not
# which flags a .o was built with, so if release and debug objects
# shared a path, switching between `make main` and `make debug` without
# a clean would silently reuse stale objects built with the wrong
# flags (-O2 -DNDEBUG vs -g -O0 -DDEBUG). Separate directories mean
# each variant's objects are tracked independently and rebuilt only
# when their own source/flags actually change.
PATHO_RELEASE = build/objs/release/
PATHO_DEBUG   = build/objs/debug/
PATHO_TEST    = build/objs/test/

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHR) $(PATHO_RELEASE) $(PATHO_DEBUG) $(PATHO_TEST)

SRCT = $(wildcard $(PATHT)test-*.c)

# GNU Make's $(wildcard) only matches one directory level, so plain
# $(wildcard $(PATHS)*.c) would miss e.g. src/parser/parser.c. This
# recurses: take *.c directly in $1, then recurse into every
# subdirectory of $1 and do the same.
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
SRC = $(call rwildcard,$(PATHS),*.c)

# One object list per variant, all derived from the same SRC list, so
# adding a new .c file to src/ is picked up automatically everywhere.
OBJ_RELEASE = $(patsubst $(PATHS)%.c,$(PATHO_RELEASE)%.o,$(SRC))
OBJ_DEBUG   = $(patsubst $(PATHS)%.c,$(PATHO_DEBUG)%.o,$(SRC))
OBJ_TEST    = $(patsubst $(PATHS)%.c,$(PATHO_TEST)%.o,$(SRC))

# main.c defines its own main(), which would collide with test.c's
# main() if both were linked into the same binary - so the test binary
# excludes it and links test.o instead.
OBJ_TEST_NO_MAIN = $(filter-out $(PATHO_TEST)main.o,$(OBJ_TEST))

# One object per test-*.c file, built with the test variant's flags.
TEST_OBJ = $(patsubst $(PATHT)test-%.c,$(PATHO_TEST)test-%.o,$(SRCT))

COMPILE = gcc -c -Wall -Werror
LINK    = gcc
DEPEND  = gcc -MM -MG
CFLAGS  = -I. -I$(PATHU) -I$(PATHS)

RELEASE_FLAGS = -O2 -DNDEBUG
TEST_FLAGS = -DTEST
DEBUG_FLAGS = -g -O0 -DDEBUG

# A single combined test binary, so a single result/log file.
RESULTS = $(PATHR)test.txt

# Header dependencies for both test/ and src/, so editing a header
# triggers a rebuild of everything that includes it - not just the
# test/ files.
DEPS := $(patsubst $(PATHT)%.c,$(PATHD)%.d,$(wildcard $(PATHT)*.c))
DEPS += $(patsubst $(PATHS)%.c,$(PATHD)%.d,$(SRC))

PASSED = `grep -s PASS $(PATHR)*.txt`
FAIL   = `grep -s FAIL $(PATHR)*.txt`
IGNORE = `grep -s IGNORE $(PATHR)*.txt`

main: $(PATHB)main.$(TARGET_EXTENSION)

run: main
	./$(PATHB)main.$(TARGET_EXTENSION)

# Builds a separately named binary from its own object directory, so
# it can't collide with (or be shadowed by) a release build, and
# doesn't need to route through `main`'s recipe/prereqs at all - which
# is what previously risked -DNDEBUG and -DDEBUG both being active at
# once.
debug: $(PATHB)main-debug.$(TARGET_EXTENSION)

fails: | $(BUILD_PATHS)
fails: $(RESULTS)
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@echo "$(FAIL)"
	@echo "\nDONE"

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
	./$< > $@ 2>&1

# Link the release production binary from every release object under src/.
$(PATHB)main.$(TARGET_EXTENSION): $(OBJ_RELEASE) | $(PATHB)
	$(LINK) -o $@ $^ -lcurl

# Link the debug binary from every debug object under src/.
$(PATHB)main-debug.$(TARGET_EXTENSION): $(OBJ_DEBUG) | $(PATHB)
	$(LINK) -o $@ $^ -lcurl

# Link a test binary from: test.c's object (has main(), calls into the
# test_* functions defined in each test-*.c), every test-*.o, every src/
# test-variant object except main.o (main.c's own main() would collide
# with test.c's), and unity.o.
$(PATHB)test.$(TARGET_EXTENSION): $(PATHO_TEST)test.o $(TEST_OBJ) $(OBJ_TEST_NO_MAIN) $(PATHO_TEST)unity.o | $(PATHB)
	$(LINK) -o $@ $^ -lcurl

# Compiles any .c under src/, including nested subdirectories, into a
# matching release object path (src/parser/parser.c ->
# build/objs/release/parser/parser.o). The order-only prereq only
# guarantees build/objs/release/ exists, not build/objs/release/parser/,
# so mkdir -p that too.
$(PATHO_RELEASE)%.o: $(PATHS)%.c | $(PATHO_RELEASE)
	@$(MKDIR) $(dir $@)
	$(COMPILE) $(CFLAGS) $(RELEASE_FLAGS) $< -o $@

# Same, but into build/objs/debug/ with debug flags.
$(PATHO_DEBUG)%.o: $(PATHS)%.c | $(PATHO_DEBUG)
	@$(MKDIR) $(dir $@)
	$(COMPILE) $(CFLAGS) $(DEBUG_FLAGS) $< -o $@

# Same, but into build/objs/test/ with test flags.
$(PATHO_TEST)%.o: $(PATHS)%.c | $(PATHO_TEST)
	@$(MKDIR) $(dir $@)
	$(COMPILE) $(CFLAGS) $(TEST_FLAGS) $< -o $@

# Compile test.c (the runner with main(), calls the test_* functions
# from each test-*.c). An explicit (non-%) rule always takes precedence
# over a pattern rule, so this can't collide with the %.o rule above.
$(PATHO_TEST)test.o: $(PATHT)test.c | $(PATHO_TEST)
	$(COMPILE) $(CFLAGS) $(TEST_FLAGS) $< -o $@

# Compile test-*.c files (the test_* function definitions themselves).
$(PATHO_TEST)test-%.o: $(PATHT)test-%.c | $(PATHO_TEST)
	$(COMPILE) $(CFLAGS) $(TEST_FLAGS) $< -o $@

# Compile Unity itself.
$(PATHO_TEST)unity.o: $(PATHU)unity.c | $(PATHO_TEST)
	$(COMPILE) $(CFLAGS) $(TEST_FLAGS) $< -o $@

# Auto-generated header dependencies for test/*.c, so editing a header
# triggers a rebuild of test.o and/or the affected test-*.o.
$(PATHD)%.d: $(PATHT)%.c | $(PATHD)
	$(DEPEND) -MT $(PATHO_TEST)$*.o -MF $@ $(CFLAGS) $<

# Auto-generated header dependencies for src/*.c. One source file
# compiles into three different objects (release/debug/test), so this
# tags the dependency rule with all three object paths via multiple
# -MT flags - editing a header now correctly invalidates whichever
# variant(s) you're currently building.
$(PATHD)%.d: $(PATHS)%.c | $(PATHD)
	@$(MKDIR) $(dir $@)
	$(DEPEND) -MT $(PATHO_RELEASE)$*.o -MT $(PATHO_DEBUG)$*.o -MT $(PATHO_TEST)$*.o -MF $@ $(CFLAGS) $<

-include $(DEPS)

$(PATHB):
	$(MKDIR) $(PATHB)

$(PATHD):
	$(MKDIR) $(PATHD)

$(PATHO_RELEASE):
	$(MKDIR) $(PATHO_RELEASE)

$(PATHO_DEBUG):
	$(MKDIR) $(PATHO_DEBUG)

$(PATHO_TEST):
	$(MKDIR) $(PATHO_TEST)

$(PATHR):
	$(MKDIR) $(PATHR)

clean:
	$(CLEANUP_DIR) $(PATHB)

fresh:
	$(MAKE) clean
	$(MAKE) main
	./$(PATHB)main.$(TARGET_EXTENSION)

.PRECIOUS: $(PATHD)%.d
.PRECIOUS: $(PATHO_RELEASE)%.o
.PRECIOUS: $(PATHO_DEBUG)%.o
.PRECIOUS: $(PATHO_TEST)%.o
.PRECIOUS: $(PATHR)%.txt