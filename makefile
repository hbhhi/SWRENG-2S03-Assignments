CC = gcc
# NUM is the Fib fractal length
NUM = 20
# INPUT is the file name of the image to be processed
INPUT = jerry
filter:
	$(CC) -g -o filter filter.c
	filter $(INPUT).ppm kernel output.ppm
	convert -compress none output.ppm output.png
	rm filter output.ppm
grey:
	$(CC) -g -o grey greyscale.c
	grey $(INPUT).ppm output.ppm
	convert -compress none output.ppm output.png
	rm grey output.ppm
fry:
	$(CC) -g -o fry deepfry.c
	fry $(INPUT).ppm output.ppm
	convert -compress none output.ppm output.png
	rm fry output.ppm
fib:
	$(CC) -g -O2 -ansi -Wall -Werror -pg -o fib fib.c main_fib.c timing.c bmp.c
run: fib
	fib $(NUM) 2 2 2 11484 4760 fib$(NUM).bmp
	# for 34, 2 2 2 27724x27722
	convert -compress none fib$(NUM).bmp fib$(NUM).jpg
	@rm -rf *.bmp fib gmon.out
clean :
	@rm -rf fib *.o *.bmp *.jpg *.out *.ppm *.png