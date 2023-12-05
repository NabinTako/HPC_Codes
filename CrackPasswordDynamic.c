#include<stdio.h>
#include<crypt.h>
#include<string.h>


#define SALT "$6$Why_N0t$"

int count=0;     // A counter used to track the number of combinations explored so far

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}

/**
 This function can crack the kind of password explained above. All combinations
 that are tried are displayed and when the password is found, #, is put at the 
 start of the line. Note that one of the most time consuming operations that 
 it performs is the output of intermediate results, so performance experiments 
 for this kind of program should not include this. i.e. comment out the printfs.
*/

void crack(char *salt_and_encrypted){
  int x, y, z;     // Loop counters
  char salt[11];    // String used in hashing the password. Need space for \0 // incase you have modified the salt value, then should modifiy the number accordingly
  char plain[11];   // The combination of letters currently being checked // Please modifiy the number when you enlarge the encrypted password.
  char *enc;   
  char passToDecrypt[98];
  
  strncpy(passToDecrypt,salt_and_encrypted,98);// Pointer to the encrypted password

  substr(salt, salt_and_encrypted, 0, 11);


  for(x='A'; x<='Z'; x++){
    for(y='A'; y<='Z'; y++){
      for(z=0; z<=99; z++){
        sprintf(plain, "%c%c%02d", x, y, z); 
        enc = (char *) crypt(plain, salt);
        count++;
	    printf("#%-8d%s %s\n", count, plain, enc);

        if(strcmp(passToDecrypt, enc) == 0){
	    printf("#%-8d%s %s\n", count, plain, enc);
		return;	
        }
      }
    }
  }
}

void main(int argc, char *argv[]){

  char* encryptedPass = (char *) crypt(argv[1], SALT) + '\0';
  crack(encryptedPass);		
  printf("%d solutions explored\n", count);

}