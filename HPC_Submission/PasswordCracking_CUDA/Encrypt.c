#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Compile: gcc Encrypt.c -o EncryptPassword  or just use make
// Execute: ./EncryptPassword

char* Crypt(char* rawPassword){

	char * newPassword = (char *) malloc(sizeof(rawPassword));

    int z_AsciiValue = 122; // As ASCii value of Z is 122.
    int a_AsciiValue = 97; // As ASCii value of A is 97.
 
	newPassword[0] = rawPassword[0] + 2;
	newPassword[1] = rawPassword[0] - 2;
	newPassword[2] = rawPassword[0] + 1;
	newPassword[3] = rawPassword[1] + 3;
	newPassword[4] = rawPassword[1] - 3;
	newPassword[5] = rawPassword[1] - 1;
	newPassword[6] = rawPassword[2] + 2;
	newPassword[7] = rawPassword[2] - 2;
	newPassword[8] = rawPassword[3] + 4;
	newPassword[9] = rawPassword[3] - 4;
	newPassword[10] = '\0';

	for(int i =0; i<10; i++){
		if(i >= 0 && i < 6){ 
			if(newPassword[i] > z_AsciiValue){
				newPassword[i] = (newPassword[i] - z_AsciiValue) + a_AsciiValue;
			}else if(newPassword[i] < a_AsciiValue){
				newPassword[i] = (a_AsciiValue- newPassword[i]) + a_AsciiValue;
			}
		}else{ //checking number section
			if(newPassword[i] > 57){
				newPassword[i] = (newPassword[i] - 57) + 48;
			}else if(newPassword[i] < 48){
				newPassword[i] = (48 - newPassword[i]) + 48;
			}
		}
	}
	return newPassword;
}

int main(int argc, char  *argv[])
{
    
    char * encrypedPassword= Crypt(argv[1]);

    printf("%s\n",encrypedPassword);
    free(encrypedPassword);

	return 0;
}
