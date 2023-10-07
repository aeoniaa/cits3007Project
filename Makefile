CC = gcc
CFLAGS += -D_POSIX_C_SOURCE=200809L -g -I. -std=c11 -pedantic -Wall -Wextra -Wconversion -fno-omit-frame-pointer -fsanitize=address,undefined,leak,thread,memory -fstack-protector-strong
SRC = p_and_p.c  # Updated source file name
OUT = p_and_p    # Updated output executable name

all: $(OUT)

$(OUT): $(SRC) p_and_p.h
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
