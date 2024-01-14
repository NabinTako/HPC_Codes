all: ImageBlur   

ImageBlur: ImageBlur.cu lodepng.cpp lodepng.h
	nvcc -Wno-deprecated-gpu-targets -o ImageBlur ImageBlur.cu lodepng.cpp 

clean:
	rm -f ImageBlur *~ a.out