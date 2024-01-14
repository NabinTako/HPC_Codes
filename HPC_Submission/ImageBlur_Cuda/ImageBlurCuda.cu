#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime_api.h>
#include "lodepng.h"

//compile : nvcc -Wno-deprecated-gpu-targets -o ImageBlur ImageBlurCuda.cu lodepng.cpp  or just use make
// Execute :./ImageBlur

	
/**
 * Program takes a PNG image type as input then gives out a blured image of hte original image in the same location 
 * as blurImage.png image.
 * 
 * ImageBlur function is called by HOST and run on DEVICE. This function is responsible for making hte blur image of the user given image.
 * the function takes 5 parameter that includes the data to be processed and the pointer to save the new blur data along with the 
 * width and height of the image and by how much to blurl the original image. 
**/



__global__ void ImageBlur(unsigned char * imageOutPutDevice, unsigned char * imageDataHost,unsigned int w,unsigned int h, int blurAmount){

//initialization 
	int red = 0;
	int green = 0;
	int blue = 0;
	int a = 0;
	int x,y;


	int idx = blockDim.x * blockIdx.x + threadIdx.x;
	int pixel = idx*4;
    for(x = (pixel - (4 * blurAmount)); x <=  (pixel + (4 * blurAmount)); x+=4){
		if ((x > 0) && x < (h * w * 4) && ((x-4)/(4*w) == pixel/(4*w))){
			for(y = (x - (16 * w * blurAmount)); y <=  (x + (16 *w * blurAmount)); y+=(4*w)){
				if(y > 0 && y < ((h * w * 4))){
					red += imageDataHost[y];
					green += imageDataHost[1+y];
					blue += imageDataHost[2+y]; 
					a++;
				}
			}
		}
	}

	imageOutPutDevice[pixel] = red / a;
	imageOutPutDevice[1+pixel] = green / a;
	imageOutPutDevice[2+pixel] = blue / a;
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
    cudaError_t cudaError; // to check for error during cuda functions.
    int blurAmount = 1; // To Determine the blur effects of picture
	unsigned int error; // to check for error during encoding and decoding the image data.
	unsigned char* img;
	unsigned int imageWidth;
	unsigned int imageHeight;
	const char* userInputImageName = "originalImage.png";  // image given by the user.
	const char* programOutputImage = "blurImageOutput.png";  // blur image created by the program.

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

	// Running the GPU funtion
	ImageBlur<<<imageHeight, imageWidth>>>(imageDataHost, imageDataDevice, imageWidth, imageHeight,blurAmount);

	cudaError = cudaMemcpy(host_imgOutput, imageDataHost, ARRAY_BYTES, cudaMemcpyDeviceToHost);
    CheckForError(cudaError);

	printf("The blur Image has been created!!\n");
	
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