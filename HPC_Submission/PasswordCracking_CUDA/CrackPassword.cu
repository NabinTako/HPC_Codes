#include <stdio.h>
#include <string.h>
#include <cuda_runtime_api.h>
#include <crypt.h>
#include "time_diff.h"

// compile the program by, nvcc -Wno-deprecated-gpu-targets -o crackPassword CrackPassword.cu time_diff.c -lrt
// or simply use make

struct timespec start, finish;
long long int difference;

__global__ void CrackPassword(char *encryptedPass,int* passwordFound)
{

    int x, y, z;                 // loop counters
    char salt[11];               // String used in hashing the password.
    char passwordCombination[7]; // The combination of letters currently being checked
    char *enc;
    int threadId = blockIdx.x * blockDim.x + threadIdx.x; // finding the threidId.
    memcpy(salt, encryptedPass + 0, sizeof(salt));
    salt[10] = '\0';

    for (x = 'A' + threadId; x <= 'Z'; x += blockDim.x * gridDim.x)
    {

        for (y = 'A'; y <= 'Z'; y++)
        {

            for (z = 0; z <= 99; z++)
            {

                printf(passwordCombination, sizeof(passwordCombination), "%c%c%02d", x, y, z);
                enc = (char *)crypt(passwordCombination, salt);

                if (memcmp(encryptedPass, enc, 97) == 0)
                {
                    *passwordFound = 1; // set the flag variable to tell other threads that password is found and break the operation.
                    break; // Break out of the innermost loop
                }
            }

            if (*passwordFound == 1)
            {
                break;
            }
        }

        if (*passwordFound == 1)
        {
            break;
        }
    }

    memcpy(encryptedPass, passwordCombination + 0, sizeof(passwordCombination));
    encryptedPass[10] = '\0';
}

int main()
{
    int blockNumber, threadNumber;
    printf("Number of Blocks to use: ");
    scanf("%d", &blockNumber);
    printf("Number of Threads per Block: ");
    scanf("%d", &threadNumber); // get the number of threads to use

    clock_gettime(CLOCK_MONOTONIC, &start);
    cudaError_t error;

    // a shared flag to terminate the program when the password combination is found.
    int passwordFoundHost = 0;
    int *passwordFoundDevice;

    char *encryptedPasswordDevice;

    char encryptedPasswordHost[97] = "$6$WhyN0t$YHWmjxJ49Ob0xHhxZQuYsaJB5V8uukKIUYFnO.RSWKZOCB2H/i28hsPa2ibDXVicSHwAZUAkREqpc2P3066Fr1";

    error = cudaMalloc(&encryptedPasswordDevice, sizeof(encryptedPasswordHost));
    if (error)
    {
        fprintf(stderr, "cudaMalloc on encryptedPasswordDevice returned %d %s\n", error,
                cudaGetErrorString(error));
        exit(1);
    }

    error = cudaMemcpy(encryptedPasswordDevice, &encryptedPasswordHost, sizeof(encryptedPasswordHost), cudaMemcpyHostToDevice);
    if (error)
    {
        fprintf(stderr, "cudaMemcpy to encryptedPasswordDevice returned %d %s\n", error,
                cudaGetErrorString(error));
    }

    // Copying the value of passwordfoundhost to be used in device.
    error = cudaMalloc(&passwordFoundDevice, sizeof(int));
    if (error)
    {
        fprintf(stderr, "cudaMalloc on encryptedPasswordDevice returned %d %s\n", error,
                cudaGetErrorString(error));
        exit(1);
    }

    error = cudaMemcpy(passwordFoundDevice, &passwordFoundHost, sizeof(int), cudaMemcpyHostToDevice);
    if (error)
    {
        fprintf(stderr, "cudaMemcpy to encryptedPasswordDevice returned %d %s\n", error,
                cudaGetErrorString(error));
    }

    CrackPassword<<<blockNumber, threadNumber>>>(encryptedPasswordDevice,passwordFoundDevice);
    cudaDeviceSynchronize();

    error = cudaMemcpy(&passwordFoundHost, passwordFoundDevice, sizeof(int), cudaMemcpyDeviceToHost);
    if (error)
    {
        fprintf(stderr, "cudaMemcpy to passwordFoundHost returned %d %s\n", error,
        cudaGetErrorString(error));
    }
    if(passwordFoundHost == 1){
        error = cudaMemcpy(encryptedPasswordHost, encryptedPasswordDevice, sizeof(encryptedPasswordHost), cudaMemcpyDeviceToHost);
        if (error)
        {
            fprintf(stderr, "cudaMemcpy to encryptedPasswordHost returned %d %s\n", error,
            cudaGetErrorString(error));
        }
    }else{
        printf("Password not found!!!\n");
    }

    // free the device pointers
    error = cudaFree(encryptedPasswordDevice);
    if (error)
    {
        fprintf(stderr, "cudaFree on encryptedPasswordDevice returned %d %s\n", error,
        cudaGetErrorString(error));
        exit(1);
    }
    error = cudaFree(passwordFoundDevice);
    if (error)
    {
        fprintf(stderr, "cudaFree on passwordFoundDevice returned %d %s\n", error,
        cudaGetErrorString(error));
        exit(1);
    }

    printf("result: encryptedPasswordHost = %s\n", encryptedPasswordHost);

    clock_gettime(CLOCK_MONOTONIC, &finish);

    // canculating the time taken by the program to find the unique password.
    time_difference(&start, &finish, &difference);

    printf("program Run Time %9.5lfs\n", difference / 1000000000.0);
    return 0;
}

// to calculate the total run time of the program
int time_difference(struct timespec *start, struct timespec *finish, long long int *difference)
{
    long long int ds = finish->tv_sec - start->tv_sec;
    long long int dn = finish->tv_nsec - start->tv_nsec;

    if (dn < 0)
    {
        ds--;
        dn += 1000000000;
    }
    *difference = ds * 1000000000 + dn;

    return !(*difference > 0);
}