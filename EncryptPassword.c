#include<stdio.h>
#include<crypt.h>

// #define SALT "$6$ND$"
// #define SALT "$6$AS$"
#define SALT "$6$WhyN0t$"

int main(int argc, char *argv[]){
  
  printf("%s\n",argv[1]);
  printf("%s\n", crypt(argv[1], SALT));

  return 0;
}
