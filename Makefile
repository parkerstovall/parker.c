ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
    CLEANUP = del /F /Q
    MKDIR = mkdir
  else # in a bash-like shell, like msys
    CLEANUP = rm -f
    MKDIR = mkdir -p
  endif
  TARGET_EXTENSION = exe
else
  CLEANUP = rm -f
  MKDIR = mkdir -p
  TARGET_EXTENSION = out
endif

.PHONY: clean run

PATHS = src/
PATHB = build/
PATHO = build/objs/

SRC = $(wildcard $(PATHS)*.c)
OBJ = $(PATHO)main.o $(PATHO)parser.o

COMPILE = gcc -c
LINK = gcc

main: $(PATHB)main.$(TARGET_EXTENSION)

run: main
	$(PATHB)main.$(TARGET_EXTENSION)

$(PATHB)main.$(TARGET_EXTENSION): $(OBJ) | $(PATHB)
	$(LINK) -o $@ $^ -lcurl

$(PATHO)%.o: $(PATHS)%.c | $(PATHO)
	$(COMPILE) $< -o $@

$(PATHB):
	$(MKDIR) $(PATHB)

$(PATHO):
	$(MKDIR) $(PATHO)

clean:
	$(CLEANUP) $(PATHO)*.o
	$(CLEANUP) $(PATHB)*.$(TARGET_EXTENSION)