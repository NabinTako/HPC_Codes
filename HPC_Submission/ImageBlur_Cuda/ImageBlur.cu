#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"

/*****************

    command to compile
	nvcc -Wno-deprecated-gpu-targets -o ImageBlur ImageBlur.cu lodepng.cpp 
    // run - ./ImageBlur

 *****************/



__global__ void ImageBlur(unsigned char * imageOutPutDevice, unsigned char * imageDataHost,unsigned int w,unsigned int h, int blurAmount){

//initialization 
	int red = 0;
	int green = 0;
	int blue = 0;
	int x,y;
	int C = 0;


	int idx = blockDim.x * blockIdx.x + threadIdx.x;
	int pixel = idx*4;
    for(x = (pixel - (4 * blurAmount)); x <=  (pixel + (4 * blurAmount)); x+=4){
		if ((x > 0) && x < (h * w * 4) && ((x-4)/(4*w) == pixel/(4*w))){
			for(y = (x - (16 * w * blurAmount)); y <=  (x + (16 *w * blurAmount)); y+=(4*w)){
				if(y > 0 && y < ((h * w * 4))){
					red += imageDataHost[y];
					green += imageDataHost[1+y];
					blue += imageDataHost[2+y]; 
					C++;
				}
			}
		}
	}

	imageOutPutDevice[pixel] = red / C;
	imageOutPutDevice[1+pixel] = green / C;
	imageOutPutDevice[2+pixel] = blue / C;
	imageOutPutDevice[3+pixel] = imageDataHost[3+pixel];
}
// to check for errors during cudaFunctions
void CheckForError(cudaError_t error){
     if (error)
    {
        printf("Something went wrong!\n");
        printf("Error: %s\n",cudaGetErrorString(error));

        exit(1);
    }
}

int main(int argc, char **argv){
    cudaError_t cudaError;
    int blurAmount = 1; // To Determine the blur effects of picture
	unsigned int error;
	unsigned char* img;
	unsigned int imageWidth;
	unsigned int imageHeight;
	const char* userInputImageName = "test.png";  //the image given by user
	const char* programOutputImage = "testBlur.png";//the blur image created by program

	error = lodepng_decode32_file(&img, &imageWidth, &imageHeight, userInputImageName);
	if(error){
		printf("error %u: %s\n", error, lodepng_error_text(error));
	}

	int ARRAY_SIZE = imageWidth*imageHeight*4;
	int ARRAY_BYTES = ARRAY_SIZE * sizeof(unsigned char);

	unsigned char host_imgInput[ARRAY_SIZE * 4];
	unsigned char host_imgOutput[ARRAY_SIZE * 4];

	for (int i = 0; i < ARRAY_SIZE; i++) {
		host_imgInput[i] = img[i];
	}

	unsigned char * imageDataDevice;
	unsigned char * imageDataHost;
	

// Device memory
	cudaError = cudaMalloc((void**) &imageDataDevice, ARRAY_BYTES);
    CheckForError(cudaError);
	cudaError = cudaMalloc((void**) &imageDataHost, ARRAY_BYTES);
    CheckForError(cudaError);



	cudaError = cudaMemcpy(imageDataDevice, host_imgInput, ARRAY_BYTES, cudaMemcpyHostToDevice);
    CheckForError(cudaError);


	ImageBlur<<<imageHeight, imageWidth>>>(imageDataHost, imageDataDevice, imageWidth, imageHeight,blurAmount);

	cudaError = cudaMemcpy(host_imgOutput, imageDataHost, ARRAY_BYTES, cudaMemcpyDeviceToHost);
    CheckForError(cudaError);

	printf("The blur Image has been created!\n");
	
	error = lodepng_encode32_file(programOutputImage, host_imgOutput, imageWidth, imageHeight);
	if(error){
		printf("error occured %u: %s\n", error, lodepng_error_text(error));
	}

// free dynamic memory
	cudaError = cudaFree(imageDataDevice);
    CheckForError(cudaError);
	cudaError = cudaFree(imageDataHost);
    CheckForError(cudaError);

	return 0;
}