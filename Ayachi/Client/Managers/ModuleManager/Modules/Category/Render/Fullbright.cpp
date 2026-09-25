#include "Fullbright.h"

Fullbright::Fullbright()
	: Module("Fullbright", "Keep the world fully lit", Category::RENDER) {
}

void Fullbright::onGetGamma(float& gamma) {
	gamma = 12.f;
}
