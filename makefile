all: CrackPassword CrackPassword2 EncryptPassword

CrackPassword: CrackPassword.c time_diff.c time_diff.h
	gcc -o crack CrackPassword.c time_diff.c -lrt -lcrypt -fopenmp

CrackPassword2: CrackPass.c time_diff.c time_diff.h
	cc -o crack02 CrackPass.c time_diff.c -lrt -lcrypt -fopenmp

EncryptPassword: EncryptPassword.c 
	cc -o encrypt EncryptPassword.c -lcrypt 
# penny02: penny02.c time_diff.c time_diff.h
# 	cc -o penny02 penny02.c time_diff.c -lrt -pthread
# penny03: penny03.c time_diff.c time_diff.h
# 	cc -o penny03 penny03.c time_diff.c -lrt -pthread
# penny04: penny04.c time_diff.c time_diff.h
# 	cc -o penny04 penny04.c time_diff.c -lrt -pthread
# penny05: penny05.c time_diff.c time_diff.h
# 	cc -o penny05 penny05.c time_diff.c -lrt -pthread
# penny06: penny06.c time_diff.c time_diff.h
# 	cc -o penny06 penny06.c time_diff.c -lrt -pthread
# penny07: penny07.c time_diff.c time_diff.h
# 	cc -o penny07 penny07.c time_diff.c -lrt -pthread
# penny08: penny08.c time_diff.c time_diff.h
# 	cc -o penny08 penny08.c time_diff.c -lrt -pthread
# penny09: penny09.c time_diff.c time_diff.h
# 	cc -o penny09 penny09.c time_diff.c -lrt -pthread
clean:
	rm -f CrackPassword *~ a.out