// Mandelbrot set (single-thread) by N. Futatsugi, 2011.01.22, 2020.06.27

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>

#include <opencv2/opencv.hpp>

inline float get_time()
{
	return static_cast<float>(std::clock()) / CLOCKS_PER_SEC;
}

using namespace std;
using namespace cv;

typedef struct { unsigned char x, y, z, w; } uchar4;

uchar4 coloring(int n)
{
	const float ratio = 4.5f;
	const int H = 176;
	const int S = 128;
	const int V = 255;
	int c = static_cast<int>(n * ratio) / (V + 1);
	int x = static_cast<int>(n * ratio) % (V + 1);
	uchar4 ret({static_cast<unsigned char>(H), static_cast<unsigned char>(S), static_cast<unsigned char>(c % 2 == 0 ? V - x : x), 0});
	return ret;
}

inline void mandelbrot(int ix, int iy, float t, float l, float w, float h, int sw, int sh, int max_iter, float th, uchar4* h_color)
{
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
			h_color[iy*sw+ix] = coloring(i);
			return;
		}
	}
	memset((void*)&h_color[iy*sw+ix], 0, sizeof(uchar4));
}

void write_mandelbrot(const string outfile, float t, float l, float w, float h, int sw, int sh, int max_iter=256, float th=4.0f)
{
	uchar4* h_color = new uchar4[sw * sh];

	double start_time = get_time();
	for (int iy = 0; iy < sh; iy++) {
		for (int ix = 0; ix < sw; ix++) {
			mandelbrot(ix, iy, t, l, w, h, sw, sh, max_iter, th, h_color);
		}
	}
	double end_time = get_time();
	cout << "Time (ms): " << (end_time - start_time) * 1000.0 << endl;

	Mat h_color_8UC4(sh, sw, CV_8UC4, reinterpret_cast<unsigned char*>(&h_color[0]));
	Mat h_color_8UC3(sh, sw, CV_8UC3);
	cvtColor(h_color_8UC4, h_color_8UC3, COLOR_RGBA2RGB);
	cvtColor(h_color_8UC3, h_color_8UC3, COLOR_HSV2RGB);
	imwrite(outfile, h_color_8UC3);

	delete h_color;
}

int main(int argc, char* argv[])
{
	string outfile("mandelbrot_single.jpg");
	if (argc >= 2) outfile = argv[1];

	//write_mandelbrot(outfile, -1.0f, -2.0f, 2.666f, 2.0f, 640, 480);
	write_mandelbrot(outfile, 0.680f, -0.220f, 0.008f, 0.0045f, 2560*5, 1440*5, 10000, 10.0f);  // WQHD (16:9) xN

	return 0;
}
