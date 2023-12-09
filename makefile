all: CrackPassword  EncryptPassword MatrixMultiply

CrackPassword: CrackPassword.c time_diff.c time_diff.h
	gcc -o crack CrackPassword.c time_diff.c -lrt -lcrypt -fopenmp

MatrixMultiply: MatrixMultiply.c 
	gcc MatrixMultiply.c -o MatrixMultiply

EncryptPassword: EncryptPassword.c 
	cc -o encrypt EncryptPassword.c -lcrypt 

clean:
	rm -f CrackPassword *~ a.out