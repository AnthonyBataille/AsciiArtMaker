#include <stdlib.h>
#include <stdio.h>

typedef struct pixel{ // Pixel in BGR order
	unsigned char B;
	unsigned char G;
	unsigned char R;
} pixel;


unsigned char grayscale_chars[36] = { // A set of character order by gray level
	' ',
	'`',
	'.',
	'\'',
	'-',
	'-',
	'^',
	'/',
	'!',
	';',
	';',
	'=',
	'|',
	'i',
	'j',
	'1',
	't',
	'I',
	'J',
	'L',
	'n',
	'V',
	'Y',
	'N',
	'P',
	'D',
	'G',
	'K',
	'B',
	'W',
	'H',
	'#',
	'#',
	'@',
	'@',
	'@'
};

int main(){
	char file_path[256];
	printf("File path: ");
	if(scanf("%256s", file_path) != 1){
		printf("Invalid input.\n");
		return 1;
	}
	FILE* f = NULL;
	if((f = fopen(file_path, "rb")) == NULL){
		printf("Unable to open file.\n");
		return 1;
	}

	// Header
	
	// ID
	char id[2];
	fread(id, 2, 1, f);
	if(id[0] != 'B' || id[1] != 'M'){
		printf("Incorrect file format\n");
		return 1;
	}
	// File size
	
	unsigned int file_size = 0;
	
	fread(&file_size, 4, 1, f);
	//printf("File size: %d\n", file_size);
	
	// Skip reserved bytes
	fseek(f, 4, SEEK_CUR);
	
	// Pixels array offset
	unsigned int pix_offset = 0;
	fread(&pix_offset, 4, 1, f);
	
	// DIB Header
	
	fseek(f, 4, SEEK_CUR);
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int pixel_size = 0;
	fread(&width, 4, 1, f);
	fread(&height, 4, 1, f);
	fseek(f, 2, SEEK_CUR);
	fread(&pixel_size, 2, 1, f);
	unsigned int color_depth = pixel_size / 8;
	
	// Pixels array
	
	fseek(f, pix_offset, SEEK_SET);
	unsigned int pix_array_size = width * height;
	pixel* pix_array = (pixel*)malloc(pix_array_size * sizeof(pixel));
	
	int pix_num = 0;
	for(unsigned int i = 0; i < pix_array_size; ++i){ // Load pixels in array assuming BGR order
		fread(&pix_array[width * (height - 1 - i / width) + i % width], sizeof(pixel), 1, f);
		if(color_depth - sizeof(pixel) > 0) fseek(f, color_depth - sizeof(pixel), SEEK_CUR);
		if(!((1 + pix_num++) % width) && (width * color_depth) % 4) fseek(f, 4 - (width * color_depth) % 4, SEEK_CUR);
	}
	
	// Print ASCII chars
	
	for(unsigned int i = 0; i < pix_array_size; ++i){
		int grayscale_value = (int)(((float)(pix_array[i].B) + (float)(pix_array[i].G) + (float)(pix_array[i].R)) / 768.0 * 36.0); // Compute the grayscale level of the current pixel
		printf("%c", grayscale_chars[grayscale_value]); // Convert it to a ASCII character
		if(!((i + 1) % width)) printf("\n"); // New line of pixels
	}
	
	free(pix_array);
	fclose(f);
	return 0;
}
