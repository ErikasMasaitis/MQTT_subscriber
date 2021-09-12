SRC:=$(wildcard *.c)
LDFLAGS:=-lmosquitto -lsqlite3 -ljson-c -luci -lcurl
BIN:=mqtt

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm  $(OBJS)
