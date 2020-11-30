#
# Makefile
#
SRC=application
CC=gcc
all: client.out

$(SRC)/%.o: $(SRC)/%.c 
	gcc -g3  -Wall -c $< -o $@

server.out: $(SRC)/led_process.o
	$(CC) ${SRC}/led_process.o -o server.out

client.out: $(SRC)/button_process.o
	$(CC) $(SRC)/button_process.o -o client.out

.PHONY: clean
clean:
	rm $(SRC)/*.o *.out
