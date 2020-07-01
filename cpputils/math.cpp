#include "math.h"
using namespace spes;

namespace spes::math
{
	size2di rect_adjust(size2di border, size2di rect)
	{
		if (rect.w > border.w)
		{
			f32 factor = (border.w + .0f) / rect.w;

			if (rect.h > border.h)
				factor = fminf(factor, (border.h + .0f) / rect.h);

			rect = rect * factor;
		}
		return rect;
	}
}