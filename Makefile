all : bin main

bin:
	mkdir -p bin

main : bin/main.o bin/bmp.o
	gcc $^ -o main -std=c99 -Wall -Werror

bin/%.o : src/%.c
	gcc -c -I include $< -o $@


.PHONY: clean

clean :
	rm -r bin 
	rm main