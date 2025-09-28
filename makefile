GC = gcc
CFLAGS = -Wall -Werror -ansi -pedantic
TARGET = /out/test.exe

$(TARGET): main.c
	$(CC) main.c -o $(TARGET) $(CFLAGS)

clean:
	del $(TARGET)