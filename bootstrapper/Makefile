CC := gcc -std=c99
CFLAGS := -O2 -Wall -Wextra -D_XOPEN_SOURCE=700
ifeq ($(NO_WERROR),)
	CFLAGS += -Werror
endif
LDFLAGS := --static

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
PROG := bootstrapper

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(notdir $(basename $(SRCS)))))

INCS :=
LIBS :=

all: $(BIN_DIR)/$(PROG)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

$(BIN_DIR)/$(PROG): $(OBJS) $(LIBS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$(PROG) $(OBJS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCS) | $(OBJ_DIR)
	cd $(OBJ_DIR); $(CC) $(CFLAGS) -c -save-temps $(addprefix -I../, $(INCS)) "../$<"

$(OBJ_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

.PHONY: all clean
