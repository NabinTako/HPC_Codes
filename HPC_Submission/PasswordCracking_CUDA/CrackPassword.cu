#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime_api.h>

// Compile: nvcc -Wno-deprecated-gpu-targets -o crackPassword CrackPassword.cu -lrt  or just use make
// Execute: ./crackPassword

/** 
 * The Program Cracks password combinations of <smallalphabet><smallalphabet><number><number>. Example: aa12,nt99,wc23 etc.
 * The Program uses the power of GPU to run many threads parallelly n GPU. CPU first transferres data using the cudaRunTme library,
 * also while transferring the data, it also checks for errors whle transferring the datas in the dynamic memories using 
 * CheckForErrors function that takes type cudaError_t (a cuda error type) as a parameter and displays the error messages.
 * 
 * The man GPU function to decryptPassword is run by CPU thats why it has __global__ attribute and other functoin 
 * with __device__ attribute in run by the GPU function. Among the gpu run functions, 
 * CudaCrypt is used to encrypt the newly generated plain password then be passed as parameter to function ComparePassword where
 * the newly encryptedPassword is compared with the userGivenEncryptedPassword and is true the function returns 1. 
 * If the comparePassword Function returns one CopyDecryptedPassword function is called by the main function of GPU which
 * copies the data of plainPassword and stores t on the gpu assigned pointer variable. 
 * The data stored in Assigned pointer variable is then copied back to hte CPU pointer data usng cudaMemcpy function And dsplayed to the user.
**/

// Variables to track program run time
struct timespec start, finish;
long long int difference;

// Functions To be run on Device/ GPU.
__device__ char * CopyDecryptedPassword(char *dest,  char *src){
  int i = 0;
  do {
    dest[i] = src[i];}
  while (src[i++] != 0);
  return dest;
}
// compares the given two character array adn returns either true or false.
__device__ int ComparePassword( char *str_a,  char *str_b, unsigned len = 255){
	int numberOfCharactermatched = 0;
	int i = 0;
	int outOfRange = 0;
	while ((i < len) && !outOfRange) {
		if ((str_a[i] == 0) || (str_b[i] == 0)) {
			outOfRange = 1;
		}
		else if (str_a[i] == str_b[i]) {
			numberOfCharactermatched++;
		}
		i++;
	}
// as numberOfCharactermatched is increased every loop when the password character matches 
// and loop runs extra one time, so if match equals the number of loop minus 1, 
// every password characters matched as variable numberOfCharactermatched is increased by 1 in every loop except the last one.

	if(numberOfCharactermatched == i-1){ 
		return 1;
	}else{
		return 0;
	}

}

// used to encrypt the plaintext to encryptedText inside GPU.
__device__ char* CudaCrypt(char* passwordToEncrypt){

	char * encryptedPassword = (char *) malloc(sizeof(char) * 11);

    int z_AsciiValue = 122; // As ASCii value of Z is 90.
    int a_AsciiValue = 97; // As ASCii value of A is 65.
 
	encryptedPassword[0] = passwordToEncrypt[0] + 2;
	encryptedPassword[1] = passwordToEncrypt[0] - 2;
	encryptedPassword[2] = passwordToEncrypt[0] + 1;
	encryptedPassword[3] = passwordToEncrypt[1] + 3;
	encryptedPassword[4] = passwordToEncrypt[1] - 3;
	encryptedPassword[5] = passwordToEncrypt[1] - 1;
	encryptedPassword[6] = passwordToEncrypt[2] + 2;
	encryptedPassword[7] = passwordToEncrypt[2] - 2;
	encryptedPassword[8] = passwordToEncrypt[3] + 4;
	encryptedPassword[9] = passwordToEncrypt[3] - 4;
	encryptedPassword[10] = '\0';

	for(int i =0; i<10; i++){
		if(i >= 0 && i < 6){ //checking all lower case letter limits
			if(encryptedPassword[i] > z_AsciiValue){
				encryptedPassword[i] = (encryptedPassword[i] - z_AsciiValue) + a_AsciiValue;
			}else if(encryptedPassword[i] < a_AsciiValue){
				encryptedPassword[i] = (a_AsciiValue - encryptedPassword[i]) + a_AsciiValue;
			}
		}else{ //checking number section
			if(encryptedPassword[i] > 57){
				encryptedPassword[i] = (encryptedPassword[i] - 57) + 48;
			}else if(encryptedPassword[i] < 48){
				encryptedPassword[i] = (48 - encryptedPassword[i]) + 48;
			}
		}
	}
	return encryptedPassword;
}

// Called by the HOST/ CPU and run nsde the GPU/DEVICE.
// Used to Crack  the encrypted password given by the user.
__global__ void CrackPassword(char * alphabet, char * numbers, char * encryptedPassword) {

	char rawPasswordToCheck[4];

	rawPasswordToCheck[0] = alphabet[blockIdx.x];
	rawPasswordToCheck[1] = alphabet[blockIdx.y];

	rawPasswordToCheck[2] = numbers[threadIdx.x];
	rawPasswordToCheck[3] = numbers[threadIdx.y];


	if (ComparePassword(CudaCrypt(rawPasswordToCheck), encryptedPassword) == 1) {
		CopyDecryptedPassword(encryptedPassword, rawPasswordToCheck);
	}
}

// Functions To be run on Host/ CPU.
// to calculate the program total run time
int time_difference(struct timespec *start, struct timespec *finish, long long int *difference) {


  long long int ds =  finish->tv_sec - start->tv_sec; 
  long long int dn =  finish->tv_nsec - start->tv_nsec; 

  if(dn < 0 ) {
    ds--;
    dn += 1000000000; 
  } 
  *difference = ds * 1000000000 + dn;
  
  return !(*difference > 0);
}

void CheckForError(cudaError_t error){
     if (error)
    {
        printf("Something went wrong!\n");
        printf("Error: %s\n",cudaGetErrorString(error));

        clock_gettime(CLOCK_MONOTONIC, &finish);

        time_difference(&start,&finish,&difference);
        printf("Program run time: %lf\n", difference /1000000000.0 );

        exit(1);
    }
}

int main(){
    clock_gettime(CLOCK_MONOTONIC,&start);
    cudaError_t error;
    
    
	char alphabetArrayHost[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	char numberArrayHost[10] = {'0','1','2','3','4','5','6','7','8','9'};

	char encryptedPassword[11] = "tpsmgi6291";

	char *decryptedPassword;
	decryptedPassword = (char *)malloc(sizeof(char) * 26);

	char * alphabetDataDevice;
	error = cudaMalloc( (void**) &alphabetDataDevice, sizeof(char) * 26); 
    CheckForError(error);
    
	error = cudaMemcpy(alphabetDataDevice, alphabetArrayHost, sizeof(char) * 26, cudaMemcpyHostToDevice);
    CheckForError(error);

    char * numberDataDevice;
	cudaMalloc( (void**) &numberDataDevice, sizeof(char) * 10); 
    CheckForError(error);

	cudaMemcpy(numberDataDevice, numberArrayHost, sizeof(char) * 10, cudaMemcpyHostToDevice);
    CheckForError(error);
	
    char *passwordToDecryptDevice;
	cudaMalloc( (void**) &passwordToDecryptDevice, sizeof(char) * 26);
    CheckForError(error);

	cudaMemcpy(passwordToDecryptDevice, encryptedPassword, sizeof(char) * 26, cudaMemcpyHostToDevice);
    CheckForError(error);

    dim3 numberOfBlocksToUse = dim3(26,26,1); 
    dim3 threadsPerBlocks = dim3(10,10,1); 

    CrackPassword<<<numberOfBlocksToUse, threadsPerBlocks >>>( alphabetDataDevice, numberDataDevice, passwordToDecryptDevice );
	cudaDeviceSynchronize();  
    
	cudaMemcpy(decryptedPassword, passwordToDecryptDevice, sizeof(char) * 26, cudaMemcpyDeviceToHost);
    CheckForError(error);

	printf("\nUser Given Encrypted Password: %s,\tDecrypted Password: %s\n\n", encryptedPassword, decryptedPassword);

    // free the pointer memory
	free(decryptedPassword);
	error = cudaFree(alphabetDataDevice);
    CheckForError(error);

	error = cudaFree(numberDataDevice);
    CheckForError(error);

	error = cudaFree(passwordToDecryptDevice);
    CheckForError(error);

    clock_gettime(CLOCK_MONOTONIC, &finish);

    time_difference(&start,&finish,&difference);
    printf("Program run time: %lf\n\n", difference /1000000000.0 );

	return 0;
}

