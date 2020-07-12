# Mandelbrot set
Draw the Mandelbrot set using single thread CPU, multi-thread CPU, and GPU.

![Mandelbrot set](mandelbrot.jpg "Mandelbrot set")

## Requirements
* OpenCV 4.3.0
* Threading Building Blocks (TBB) v2020.2 [for multi-thread]
* CUDA Toolkit 11.0 [for GPU]

### Windows
* Visual Studio 2019

### Linux
* GCC 8.3
* pkg-config 0.29

## Build

### Single-thread
* Windows (MS VS2019)
```
> cl /EHsc /W3 /nologo /O2 /Zi /MT mandelbrot_single.cpp opencv_world430.lib
```

* Linux (GCC 8.3)
```
$ g++ mandelbrot_single.cpp `pkg-config --libs opencv` `pkg-config --cflags opencv` -std=c++11 -O3 -o mandelbrot_single
```

### Multi-thread using TBB
* Windows (MS VS2019)
```
> cl /EHsc /W3 /nologo /O2 /Zi /MT mandelbrot_multi.cpp tbb.lib opencv_world430.lib
```

* Linux (GCC 8.3)
```
$ g++ mandelbrot_multi.cpp `pkg-config --libs opencv` `pkg-config --cflags opencv` -std=c++11 -ltbb -O3 -o mandelbrot_multi
```

### GPU using CUDA
* Windows (MS VS2019)
```
> nvcc -Xcompiler /EHsc,/W3,/nologo,/O2,/Zi,/MT mandelbrot_gpu.cu opencv_world430.lib -o mandelbrot_gpu
```

* Linux (GCC 8.3)
```
$ nvcc `pkg-config --libs opencv` `pkg-config --cflags opencv` mandelbrot_gpu.cu -o mandelbrot_gpu
```

## Benchmarks
* Microsoft Windows 10 Pro (x64)
* Intel i9-10900K @ 3.70GHz
* NVIDIA GeForce RTX 2080 Ti

### Single-thread CPU
```
> mandelbrot_single
Time (ms): 11660
```
### Multi-thread CPU
```
> mandelbrot_multi
Time (ms): 674.269
```
### GPU
```
> mandelbrot_gpu
Time (ms): 8.31939
```
