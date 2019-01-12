

struct RGB8Pixel {
	unsigned char red_;
	unsigned char green_;
	unsigned char blue_;

	RGB8Pixel(const unsigned char red, const unsigned char green, const unsigned char blue) {
		red_   = red;
		green_ = green;
		blue_  = blue;
	}

	void set(const unsigned char red, const unsigned char green, const unsigned char blue) {
		red_   = red;
		green_ = green;
		blue_  = blue;
	}

	unsigned char red() const { return red_; }

	unsigned char green() const { return green_; }

	unsigned char blue() const { return blue_; }
}

struct RGBA8Pixel : RGB8Pixel {
	unsigned char alpha_;

	RGBA8Pixel(unsigned char red, unsigned char green, unsigned char blue, alpha) : RGB8Pixel(red, green, blue) {
		alpha_ = alpha;
	}
}