all: qrgen

main.o: main.c
	clang -c main.c -I/opt/homebrew/Cellar/qrencode/4.1.1/include -o main.o

qrgen: main.o
	clang main.o -L/opt/homebrew/Cellar/qrencode/4.1.1/lib -lqrencode -o qrgen

clean:
	rm -f main.o qrgen