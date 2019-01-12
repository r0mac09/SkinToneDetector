//SOURCE: https://github.com/sol-prog/cpp-bmp-images

#include <cstdio>
#include <ctime>
#include <string>
#include <stdexcept>
#include <iostream>

#include "BMPImage.h"
#include "Pixels.h"

BMPImage::BMPImage(const char* fileName_) {
    hasAlpha = false; //We assume that it odesn't have transparency unless otherwise
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

    if(bmp_info_header.bit_count != 24 && bmp_info_header.bit_count != 32) {
        throw std::runtime_error("Error! Can't handle anythin rlse than 8 bit images ...");
    }

	if(bmp_info_header.bit_count == 32) {
        hasAlpha = true;
		if(bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
			if(fread((char*)&bmp_color_header, 1, sizeof(bmp_color_header), imageFile) != 1) {
				throw std::runtime_error("Error! Could not bmp color header ...");
			}
			check_color_header(bmp_color_header);
		}
		else {
            std::cerr << "Error! The file \"" << fileName << "\" does not seem to contain bit mask information\n";
            throw std::runtime_error("Error! Unrecognized file format.");
		}
	}

	fseek(imageFile, file_header.offset_data, SEEK_SET);


	if(hasAlpha) {
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

    // data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);
    if(hasAlpha) {
        data = new RGBA8Pixel*[bmp_info_header.height];
        for(int i = 0; i < bmp_info_header.height; i++) data[i] = new RGBA8Pixel[bmp_info_header.width];
    } else {
        data = new RGB8Pixel*[bmp_info_header.height];
        for(int i = 0; i < bmp_info_header.height; i++) data[i] = new RGB8Pixel[bmp_info_header.width];
    }
    

    // Here we check if we need to take into account row padding
    uint32_t widthInBytes = bmp_info_header.width * bmp_info_header.bit_count / 8;
    
    if(hasAlpha) {
        uint8_t buffer[widthInBytes];
        for(int i = 0; i < bmp_info_header.height; i++) {
            if(fread(buffer, 1, widthInBytes, imageFile) != widthInBytes) {
                throw std::runtime_error("Error! Could't read pixel data ...");
            }
            for(int j = 0, k = 0; j < widthInBytes; j+=4, k++) {
                data[i][k]->set(buffer[j], buffer[j+1], bugger[j+2], buffer[j+3]); 
            }
        }
        file_header.file_size += widthInBytes*bmp_info_header.height;
    } else if (widthInBytes % 4 == 0) {
        uint8_t buffer[widthInBytes];

        for(int i = 0; i < bmp_info_header.height; i++) {
            if(fread(buffer, 1, widthInBytes, imageFile) != widthInBytes) {
                throw std::runtime_error("Error! Could't read pixel data ...");
            }
            for(int j = 0, k = 0; j < widthInBytes; j+=4, k++) {
                data[i][k]->set(buffer[j], buffer[j+1], buffer[j+2]); 
            }
        }
        file_header.file_size += widthInBytes*bmp_info_header.height;
    } else {
        uint32_t newWidth = widthInBytes + (widthInBytes % 4 == 0);
        uint8_t buffer[newWidth];

        for(int i = 0; i < bmp_info_header.height; i++) {
            if(fread(buffer, 1, newWidth, imageFile) != newWidth) {
                throw std::runtime_error("Error! Could't read pixel data ...");
            }
            for(int j = 0, k = 0; j < widthInBytes; j+=4, k++) {
                data[i][k]->set(buffer[j], buffer[j+1], buffer[j+2]); 
            }
        }
        file_header.file_size += newWidth*bmp_info_header.height;
    }

}

//TO BE DETERMINED IF REQUIERED

// BMPImage::BMPImage(int32_t width, int32_t height, bool has_alpha = true) {
//     if (width <= 0 || height <= 0) {
//         throw std::runtime_error("The image width and height must be positive numbers.");
//     }

//     bmp_info_header.width = width;
//     bmp_info_header.height = height;
//     if (has_alpha) {
//         bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
//         file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);

//         bmp_info_header.bit_count = 32;
//         bmp_info_header.compression = 3;
//         row_stride = width * 4;
//         data.resize(row_stride * height);
//         file_header.file_size = file_header.offset_data + data.size();
//     }
//     else {
//         bmp_info_header.size = sizeof(BMPInfoHeader);
//         file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

//         bmp_info_header.bit_count = 24;
//         bmp_info_header.compression = 0;
//         row_stride = width * 3;
//         data.resize(row_stride * height);

//         uint32_t new_stride = make_stride_aligned(4);
//         file_header.file_size = file_header.offset_data + data.size() + bmp_info_header.height * (new_stride - row_stride);
//     }
// }

void BMPImage::write() {
    FILE* writeImage = fopen(generateFileName(), "wb");
    if(!writeImage) {
        throw std::runtime_error("Error! Could not write the image ...");
    }

    //To be further implemented

}

std::string BMPImage:generateFileName() {
    char time[80];
    time_t rawtime;
    struct tm * timeinfo;
    std::string generatedName;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time, sizeof(time), "%d-%m-%Y_%H:%M:%S", timeinfo);
    generatedName = str("generated_") + str(time);

    return generatedName;
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

void BMPImage::print() {
    printf("");
}