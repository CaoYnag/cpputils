#pragma once
#include "math.h"
#include "image.h"
#include "canvas.h"
#include <vector>
#include <functional>
using namespace std;
using namespace spes;
using namespace spes::image;
using namespace spes::math;

namespace spes::canvas
{
	typedef function<f32(f32)> func;
	class FuncCanvas : public Canvas
	{
	public:
		FuncCanvas(size2di&, range2f&);
		FuncCanvas(size2di&, size2di&, range2f&);
		virtual ~FuncCanvas();

		void background(color_t);
		void background(image_t&);
		void axis(bool draw, color_t c = Colors::BLACK);
		void color(color_t);
		void x_range(range2f&);
		void y_range(range2f&);
		void func(func);
	};
}