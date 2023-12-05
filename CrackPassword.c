#include<stdio.h>
#include<crypt.h>
#include<string.h>
#include <unistd.h>

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

  substr(salt, salt_and_encrypted, 0, 11);


  for(x='A'; x<='Z'; x++){
    for(y='A'; y<='Z'; y++){
      for(z=0; z<=99; z++){
        sprintf(plain, "%c%c%02d", x, y, z); 
        enc = (char *) crypt(plain, salt);
        count++;

        if(strcmp(salt_and_encrypted, enc) == 0){
	    printf("#%-8d%s %s\n", count, plain, enc);
		return;	
        }
      }
    }
  }
}

void main(int argc, char *argv[]){

  // passing the encrypted password to the crack function to decode it	
  crack("$6$Why_N0t$Wd.BpNdkJl8F9Q5szPGPvChN52rrtB5nOXro2rXaYuxggkKQ.UWTHwgTQu6ZA3FWOYIGD2YkgjKdx91GFvpr4/");	
  printf("%d solutions explored\n", count);

}