// madelbrot  by N. Futatsugi, 2020.06.18

#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int BLOCK_SIZE_X = 16;
const int BLOCK_SIZE_Y = 16;

__device__
uchar4 coloring(int n)
{
	//const float ratio = 3.2f;
	const float ratio = 4.5f;
	const int H = 176;
	const int S = 128;
	const int V = 255;
	int c = static_cast<int>(n * ratio) / (V + 1);
	int x = static_cast<int>(n * ratio) % (V + 1);
	return make_uchar4(H, S, c % 2 == 0 ? V - x : x, 0);
}

__global__
void mandelbrot(float t, float l, float w, float h, int sw, int sh, int max_iter, float th, uchar4* d_color)
{
	int ix = blockIdx.x * blockDim.x + threadIdx.x;
	int iy = blockIdx.y * blockDim.y + threadIdx.y;
	if (ix >= sw || iy >= sh) return;

	float ci = t + (static_cast<float>(iy) / sh) * h;
	float cr = l + (static_cast<float>(ix) / sw) * w;
	float zi = 0.0f;
	float zr = 0.0f;
	float zrzi, zr2, zi2;

	for (int i = 0; i < max_iter; i++) {
		zrzi = zr * zi;
		zr2 = zr * zr;
		zi2 = zi * zi;
		zr = zr2 - zi2 + cr;
		zi = zrzi + zrzi + ci;
		if (zi2 + zr2 >= th) {
			d_color[iy*sw+ix] = coloring(i);
			return;
		}
	}
	d_color[iy*sw+ix] = make_uchar4(0, 0, 0, 0);
}

void write_mandelbrot(const string outfile, float t, float l, float w, float h, int sw, int sh, int max_iter=256, float th=4.0f)
{
	dim3 num_blocks((sw - 1) / BLOCK_SIZE_X + 1, (sh - 1) / BLOCK_SIZE_Y + 1, 1);
	dim3 num_threads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1);

	uchar4* h_color;
	uchar4* d_color;
	cudaMallocHost(reinterpret_cast<void**>(&h_color), sizeof(uchar4) * sw * sh);
	cudaMalloc(reinterpret_cast<void**>(&d_color), sizeof(uchar4) * sw * sh);

	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start);
	
	mandelbrot<<<num_blocks, num_threads>>>(t, l, w, h, sw, sh, max_iter, th, d_color);
	cudaDeviceSynchronize();

	cudaEventRecord(stop);
	cudaEventSynchronize(stop);

	cudaMemcpy(h_color, d_color, sizeof(uchar4) * sw * sh, cudaMemcpyDeviceToHost);

	float milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start, stop);
	cout << "Time (ms): " << milliseconds << endl;

	Mat h_color_8UC4(sh, sw, CV_8UC4, reinterpret_cast<unsigned char*>(&h_color[0]));
	Mat h_color_8UC3(sh, sw, CV_8UC3);
	cvtColor(h_color_8UC4, h_color_8UC3, COLOR_RGBA2RGB);
	cvtColor(h_color_8UC3, h_color_8UC3, COLOR_HSV2RGB);
	imwrite(outfile, h_color_8UC3);

	cudaFree(d_color);
	cudaFreeHost(h_color);
}

int main(int argc, char* argv[])
{
	string outfile("mandelbrot_gpu.jpg");
	if (argc >= 2) outfile = argv[1];

	cudaSetDevice(0);

	//write_mandelbrot(outfile, -1.0f, -2.0f, 2.666f, 2.0f, 640, 480);
	write_mandelbrot(outfile, 0.680f, -0.220f, 0.008f, 0.0045f, 2560*5, 1440*5, 10000, 10.0f);  // WQHD (16:9) xN

	return 0;
}
