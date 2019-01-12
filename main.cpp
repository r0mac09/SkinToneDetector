#include <cstdio>

#include "BMPImage.h"


int main(void) {
	BMPImage test("test.bmp");

	printf("W = %d\nH = %d\nBpp = %d\n", test.width, test.height, test.bpp);

	return 0;

}