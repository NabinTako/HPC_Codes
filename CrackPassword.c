#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <omp.h>
#include "time_diff.h"

int count = 0;   // A counter used to track the number of combinations explored so far
omp_lock_t lock; // a lock for critical sections

struct timespec start, finish;
long long int difference;

void substr(char *dest, char *src, int start, int length)
{
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}

/**
 All combinations that are tried are displayed and when the password is found,
 #, is put at the start of the line. Note that one of the most time consuming 
 operations that it performs is the output of intermediate results, so performance 
 experiments for this kind of program should not include this. i.e. comment out the printfs.
*/

void crack(char *encryptedPassword)
{
  int x, y, z;   // Loop counters
  char salt[11]; // String used in hashing the password.
  char plain[7]; // The combination of letters currently being checked
  char *enc;

  substr(salt, encryptedPassword, 0, 10);

#pragma omp parallel for collapse(3) private(x, y, z, plain, enc) shared(count)
  for (x = 'A'; x <= 'Z'; x++)
  {

    for (y = 'A'; y <= 'Z'; y++)
    {

      for (z = 0; z <= 99; z++)
      {

        omp_set_lock(&lock);
        snprintf(plain, sizeof(plain), "%c%c%02d", x, y, z);
        enc = (char *)crypt(plain, salt);

        count++;

        if (strcmp(encryptedPassword, enc) == 0)
        {
          
          printf("#%-8d%s %s__%d\n", count, plain, enc, omp_get_thread_num());
          omp_unset_lock(&lock);

          omp_destroy_lock(&lock);

          clock_gettime(CLOCK_MONOTONIC, &finish);
          
          // canculating the time taken by the program to find the unique password.
          time_difference(&start, &finish, &difference);

          printf("%d solutions explored\n", count);

          printf("program Run Time %9.5lfs\n", difference / 1000000000.0);

          // Ending the program when the password is found
          exit(0);
        }
        omp_unset_lock(&lock);
      }
    }
  }
}

int main()
{
  int threadNnumber;
  printf("Number of Threads to use: ");
  scanf("%d", &threadNnumber);

  clock_gettime(CLOCK_MONOTONIC, &start);

  omp_init_lock(&lock);

  omp_set_num_threads(threadNnumber);

  // passing the encrypted password to the crack function to decode it

  // crack("$6$WhyN0t$VdZOpOx2hPYncxxdpQ1d4F1pRJnQ0lsLLKts3oKIPxfD5CVEMDMq43CIUjiBCgOcHE1/HRoOIMeTiBDLYrhWt/");

  crack("$6$WhyN0t$YHWmjxJ49Ob0xHhxZQuYsaJB5V8uukKIUYFnO.RSWKZOCB2H/i28hsPa2ibDXVicSHwAZUAkREqpc2P3066Fr1");

  return 0;
}