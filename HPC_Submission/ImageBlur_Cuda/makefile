all: ImageBlur   

ImageBlur: ImageBlurCuda.cu lodepng.cpp lodepng.h
	nvcc -Wno-deprecated-gpu-targets -o ImageBlur ImageBlurCuda.cu lodepng.cpp 

clean:
	rm -f ImageBlur *~ a.out