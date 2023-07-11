CC        := gcc
CPPFLAGS  :=
CFLAGS    := -std=gnu11 -O0 -g -Wall -Wextra -pedantic
LDFLAGS   :=
LDLIBS    := -lpthread

SRC_DIR   := src
OBJ_DIR   := src
BIN_DIR   := bin

TARGET    := $(BIN_DIR)/peer $(BIN_DIR)/name_server

SRC       := $(wildcard $(SRC_DIR)/*.c)

OBJ       := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_PEER  := $(filter-out src/name_server.o, $(OBJ))
OBJ_NS    := $(filter-out src/peer.o, $(OBJ))

.PHONY: all clean

all: $(TARGET)

$(BIN_DIR)/peer: $(OBJ_PEER) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR)/name_server: $(OBJ_NS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ)/*.o vgcore*

-include $(OBJ:.o=.d)
