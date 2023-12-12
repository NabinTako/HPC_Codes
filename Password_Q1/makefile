all: CrackPassword  EncryptPassword 

CrackPassword: CrackPassword.c time_diff.c time_diff.h
	gcc -o crack CrackPassword.c time_diff.c -lrt -lcrypt -fopenmp

EncryptPassword: EncryptPassword.c 
	cc -o encrypt EncryptPassword.c -lcrypt 

clean:
	rm -f CrackPassword EncryptPassword*~ a.out