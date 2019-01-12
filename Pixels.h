#pragma once

#pragma pack(push, 1)
struct RGB8Pixel {
	uint8_t red_;
	uint8_t green_;
	uint8_t blue_;

	RGB8Pixel(const uint8_t red, const uint8_t green, const uint8_t blue) {
		red_   = red;
		green_ = green;
		blue_  = blue;
	}

	void set(const uint8_t red, const uint8_t green, const uint8_t blue) {
		red_   = red;
		green_ = green;
		blue_  = blue;
	}

	uint8_t red() const { return red_; }

	uint8_t green() const { return green_; }

	uint8_t blue() const { return blue_; }
};

struct RGBA8Pixel : RGB8Pixel {
	uint8_t alpha_;

	RGBA8Pixel(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) : RGB8Pixel(red, green, blue) {
		alpha_ = alpha;
	}

	void set(const uint8_t alpha, const uint8_t red, const uint8_t green, const uint8_t blue) {
		alpha_ = alpha;
		red_   = red;
		green_ = green;
		blue_  = blue;
	}
};
#pragma pack(pop)	