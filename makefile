make:
	gcc -o main main.c headers/f_image.c 
run :
	make
	./main
clean:
	rm main