//SOURCE: https://github.com/sol-prog/cpp-bmp-images


#include <cstdio>
#include <stdexcept>

#include "BMPImage.h"

BMPImage::BMPImage(const char* fileName_) {
	snprintf(this->fileName, 256, "%s", fileName);
	imageFile = fopen(fileName, "rb");
	if(!imageFile) {
		throw std::runtime_error("Error! Could not open the file ...");
	}
}

void BMPImage::read() {
	if(fread((char*)&file_header, 1, sizeof(file_header), imageFile) != 1) {
		throw std::runtime_error("Error! Could not read file header ...");
	}

	if(file_header.file_type != 0x4D42) {
        throw std::runtime_error("Error! Unrecognized file format ...");
	}

	if(fread((char*)&bmp_info_header, 1, sizeof(bmp_info_header), imageFile) != 1) {
		throw std::runtime_error("Error! Could not bmp info header ...");
	}

	if(bmp_info_header.bit_count == 32) {
		if(bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
			if(fread((char*)&bmp_color_header, 1, sizeof(bmp_color_header), imageFile) != 1) {
				throw std::runtime_error("Error! Could not bmp info header ...");
			}
			check_color_header(bmp_color_header);
		}
		else {
            std::cerr << "Error! The file \"" << fileName << "\" does not seem to contain bit mask information\n";
            throw std::runtime_error("Error! Unrecognized file format.");
		}
	}

	fseek(imageFile, file_header.offset_data, SEEK_SET);


	if(bmp_info_header.bit_count == 32) {
        bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
    } else {
        bmp_info_header.size = sizeof(BMPInfoHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    }
    file_header.file_size = file_header.offset_data;

    if (bmp_info_header.height < 0) {
        throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
    }

    data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

    // Here we check if we need to take into account row padding
    if (bmp_info_header.width % 4 == 0) {
        inp.read((char*)data.data(), data.size());
        file_header.file_size += data.size();
    }
    else {
        row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
        uint32_t new_stride = make_stride_aligned(4);
        std::vector<uint8_t> padding_row(new_stride - row_stride);

        for (int y = 0; y < bmp_info_header.height; ++y) {
            inp.read((char*)(data.data() + row_stride * y), row_stride);
            inp.read((char*)padding_row.data(), padding_row.size());
        }
        file_header.file_size += data.size() + bmp_info_header.height * padding_row.size();
    }

}

void BMPImage::check_color_header(BMPColorHeader &bmp_color_header) {
    BMPColorHeader expected_color_header;
    if(
    	expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask
    ) {
        throw std::runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
    }
    if(expected_color_header.color_space_type != bmp_color_header.color_space_type) {
        throw std::runtime_error("Unexpected color space type! The program expects sRGB values");
    }
}