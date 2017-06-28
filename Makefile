CC = gcc

backlight: backlight.c
	$(CC) -o backlight backlight.c

clean:
	rm backlight

phony: clean
