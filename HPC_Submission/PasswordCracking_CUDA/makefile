all: Encrypt  CrackPassword

Encrypt: Encrypt.c 
	gcc Encrypt.c -o EncryptPassword 
	
CrackPassword: CrackPassword.cu
	nvcc -Wno-deprecated-gpu-targets -o crackPassword CrackPassword.cu -lrt 

clean:
	rm -f CrackPassword   *~ a.out