#AAAAAAA
CC = gcc
CFLAGS += -D_POSIX_C_SOURCE=200809L -g -I. -std=c11 -pedantic -Wall -Wextra -Wconversion -fno-omit-frame-pointer -fsanitize=address,undefined,leak -fstack-protector-strong
SRC = p_and_p.c
OUT = p_and_p

all: $(OUT)

$(OUT): $(SRC) p_and_p.h
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
