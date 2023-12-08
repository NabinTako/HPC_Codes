#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <omp.h>
#include "time_diff.h"

int count = 0;          // A counter used to track the number of combinations explored so far
omp_lock_t lock; // a lock for critical sections

int passwordMatchFound = 0;
void substr(char *dest, char *src, int start, int length)
{
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

void crack(char *salt_and_encrypted)
{
  int x, y, z;    // Loop counters
  char salt[11];  // String used in hashing the password. Need space for \0 // incase you have modified the salt value, then should modifiy the number accordingly
  char plain[7]; // The combination of letters currently being checked // Please modifiy the number when you enlarge the encrypted password.
  char passToDecrypt[98];
  char *enc;
  
  strncpy(passToDecrypt,salt_and_encrypted,98);

  substr(salt, salt_and_encrypted, 0, 10);
  salt[11]='\0';


#pragma omp parallel for collapse(3) private(x, y, z, plain, enc) shared(count, passwordMatchFound)
  for (x = 'A'; x <= 'Z'; x++)
  { 
    
    for (y = 'A'; y <= 'Z'; y++)
    { 
      
      for (z = 0; z <= 99; z++)
      {
        if (passwordMatchFound == 1) {
           #pragma omp cancel for
            continue;
        }

        
        omp_set_lock(&lock);
        snprintf(plain, sizeof(plain), "%c%c%02d", x, y, z);
        enc = (char *)crypt(plain, salt);
        
        count++;
        
        // printf("#%-8d%s %s %s__%d\n", count, plain,salt, enc, omp_get_thread_num());

        if (strcmp(salt_and_encrypted, enc) == 0)
        {
          passwordMatchFound = 1;
          printf("#%-8d%s %s__%d\n", count, plain, enc, omp_get_thread_num());
          // printf("\n\n\n\n\n\n\n");
          omp_unset_lock(&lock);
          #pragma omp cancel for
          continue;

        } 
        omp_unset_lock(&lock);
        
      }
    }
  }
}

int main()
{
  int threadNnumber;
  printf("Number of THreads to use: ");scanf("%d",&threadNnumber);

  struct timespec start, finish;
  long long int difference;
  clock_gettime(CLOCK_MONOTONIC, &start);

  omp_init_lock(&lock);

  omp_set_num_threads(threadNnumber);

  // passing the encrypted password to the crack function to decode it


  // crack("$6$WhyN0t$VdZOpOx2hPYncxxdpQ1d4F1pRJnQ0lsLLKts3oKIPxfD5CVEMDMq43CIUjiBCgOcHE1/HRoOIMeTiBDLYrhWt/"); // TN09

  crack("$6$WhyN0t$cPCekX8XV8LZtcl593QCRdxxo.GwC/LQTVayU5dz/yXSaEBJJLim9nKr4XBrCw9hiodp8fl3ilRXMM.HyfWW70"); // NA99

  // crack("$6$WhyN0t$.12IafO67JQFVKeJDk51b5sNLXhWTM/wjrIUs.3IuMQM6E5sqmjFLDTzjbo6kbUm4qmzla4.As8JLco0KI5tm1"); // WC23

  

  printf("%d solutions explored\n", count);

  omp_destroy_lock(&lock);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  time_difference(&start, &finish, &difference);
  printf("run lasted %9.5lfs\n", difference / 1000000000.0);
  return 0;
}