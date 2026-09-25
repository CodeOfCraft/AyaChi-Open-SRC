#pragma once

namespace mce {
	struct Color {
		union {
			struct {
				float r, g, b, a;
			};
			float arr[4];
		};

		Color(float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f) {
			this->r = red;
			this->g = green;
			this->b = blue;
			this->a = alpha;
		}
	};
}
