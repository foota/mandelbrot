// Mandelbrot set (multi-thread) by N. Futatsugi, 2011.02.02, 2020.06.27

#include <iostream>
#include <fstream>
#include <ctime>

#include <opencv2/opencv.hpp>

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/tick_count.h>

inline float get_time()
{
	return static_cast<float>(std::clock()) / CLOCKS_PER_SEC;
}

using namespace std;
using namespace tbb;
using namespace cv;

class CalcMandelbrot {
private:
	float t, l, w, h, th;
	int sw, sh, max_iter;
	Mat& h_color;

public:
	CalcMandelbrot(float t_, float l_, float w_, float h_, int sw_, int sh_, int max_iter_, float th_, Mat& h_color_)
		: t(t_), l(l_), w(w_), h(h_), sw(sw_), sh(sh_), max_iter(max_iter_), th(th_), h_color(h_color_) { }

	const Vec3b coloring(int n) const {
		const float ratio = 4.5f;
		const int H = 176;
		const int S = 128;
		const int V = 255;
		int c = static_cast<int>(n * ratio) / (V + 1);
		int x = static_cast<int>(n * ratio) % (V + 1);
		return Vec3b(H, S, c % 2 == 0 ? V - x : x);
	}
	
	void operator()(const blocked_range<int>& r) const {
		for (int i = r.begin(); i != r.end(); i++) {
			int ix = i % sw;
			int iy = i / sw;
			float ci = t + (static_cast<float>(iy) / sh) * h;
			float cr = l + (static_cast<float>(ix) / sw) * w;
			float zi = 0.0f;
			float zr = 0.0f;
			float zrzi, zr2, zi2;

			for (int j = 0; j < max_iter; j++) {
				zrzi = zr * zi;
				zr2 = zr * zr;
				zi2 = zi * zi;
				zr = zr2 - zi2 + cr;
				zi = zrzi + zrzi + ci;
				if (zi2 + zr2 >= th) {
					h_color.at<Vec3b>(iy, ix) = coloring(j);
					break;
				}
			}
		}
	}
};

void write_mandelbrot(const string outfile, float t, float l, float w, float h, int sw, int sh, int max_iter=256, float th=4.0f)
{
	Mat h_color(sh, sw, CV_8UC3);
	
	tick_count start_time = tick_count::now();
	parallel_for(blocked_range<int>(0, sw * sh), CalcMandelbrot(t, l, w, h, sw, sh, max_iter, th, h_color), auto_partitioner());
	tick_count end_time = tick_count::now();
	cout << "Time (ms): " << (end_time - start_time).seconds() * 1000.0 << endl;

	cvtColor(h_color, h_color, COLOR_HSV2RGB);
	imwrite(outfile, h_color);
}

int main(int argc, char* argv[])
{
	string outfile("mandelbrot_multi.jpg");
	if (argc >= 2) outfile = argv[1];

	//write_mandelbrot(outfile, -1.0f, -2.0f, 2.666f, 2.0f, 640, 480);
	write_mandelbrot(outfile, 0.680f, -0.220f, 0.008f, 0.0045f, 2560*5, 1440*5, 10000, 10.0f);  // WQHD (16:9) xN

	return 0;
}
