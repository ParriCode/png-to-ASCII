/*
 ============================================================================
 Name        : png_to_ASCII.c
 Author      : theparri@protonmail.com
 Version     : 1.0
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <png.h>
#include <errno.h>
#include <stdarg.h>
#include "csv.h"
#include "dataconfig.h"

int resize(int **matrix,int height, int width,int **newmatrix,int newheight, int newwidth);
void abort_(const char * s, ...);
struct frame{
	int height;				//size of image on axis Y and axis X
	int width;   	  		//size of image on axis X
	int **matrix;			//Pointer that contains the value of each pixel
	char *picture;			//Array that contains the image transformed
};
int main(int argc,char *argv[]){
	char *path;
	char *savepath;
	char *configpath;
	bool bool_resize = false;
	int newwidth;
	int newheight;
	if (argc > 1 ){
		if(strcmp(argv[1],"-h") == 0  || strcmp(argv[1],"--help") == 0){
			printf("\n png_to_ASCII [png image] [ascii text] [config file]");
			printf("\n png_to_ASCII --config [config file]");
			return 0;
		}

		if(strcmp(argv[1],"--config") == 0){
			if(argc != 3){printf("\nToo few arguments , revise --help to more information\n");exit(1);}
			char *datapath;
			datapath = (char *)malloc((strlen("./data/")+strlen(argv[2]))*sizeof(char));
			strcpy(datapath,"./data/");
			strcat(datapath,argv[2]);
			dataconfig(datapath);
		}
		else if(argc >= 4){
			path 		= argv[1];
			savepath 	= argv[2];
			configpath = (char *)malloc((strlen("./data/")+strlen(argv[2]))*sizeof(char));
			strcpy(configpath,"./data/");
			strcat(configpath,argv[3]);
			if (argc>5){
				bool_resize = true;
				newwidth = atoi(argv[4]);
				if (argc == 6){
					newheight = atoi(argv[5]);
				}
				else{
					newheight = newwidth;
				}
			}
		}
		else{printf("\nToo few arguments , revise --help to more information\n");exit(1);}
		printf("y = %d",newheight);
		printf("x = %d",newwidth);
		printf("argc = %d",argc);

	}
	else{printf("\nToo few arguments , revise --help to more information\n");exit(1);}

	char pixelchar[2];
	int n_characters,picture_exist,pixel;
	int x,y,i;
	struct frame img;
	x= y = i = img.height = img.width = 0;

	n_characters = picture_exist= pixel=  0;
	size_csv_file(configpath,&n_characters);  			//Get n_characters of characters file
	/*=============================================================================*/
	int *colors_num;
	colors_num = (int   *)malloc(n_characters*sizeof(int   ));
	/*=============================================================================*/
	char **characters;
	characters = (char **)malloc(n_characters*sizeof(char *));
	for (i = 0;i<n_characters;i++){
		characters[i] = (char *)malloc(2*sizeof(char));
	}
	/*=============================================================================*/
	/*Read data from datafiles*/
	readcsv(configpath,colors_num,characters);
	printf("\nOpen %s image",path);
	printf("\nopen configuration: %s",configpath);
	for(i = 0;i<n_characters;i++){
		printf("\n%s,%d",characters[i],colors_num[i]);
	}
/* ===================================================
   READ PNG FILE
   ===================================================*/
    png_bytep * row_pointers;
    png_structp png_ptr;
	png_infop info_ptr;
	char header[8];
	int color_type;
	FILE *fp = fopen(path, "rb");
	if (!fp)
			abort_("[read_png_file] File %s could not be opened for reading", path);
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
			abort_("[read_png_file] File %s is not recognized as a PNG file", path);
	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
			abort_("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
			abort_("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
			abort_("[read_png_file] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	img.width = png_get_image_width(png_ptr, info_ptr);    // get image width file
	img.height = png_get_image_height(png_ptr, info_ptr);  // get image height file
    color_type = png_get_color_type(png_ptr, info_ptr);
    printf("\ntype = %d\n",color_type);
	png_read_update_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr)))
			abort_("[read_png_file] Error during read_image");
	/*=============================================================================*/
	//Set img.matrix 2D array memory size
	/*=============================================================================*/
	img.matrix = (int**)malloc(img.width*sizeof(int *));
	for(int i = 0;i < img.width;i++){
		img.matrix[i] = (int*)malloc(img.height*sizeof(int));
	}
	/*=============================================================================*/
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * img.height);
	for (y=0; y<img.height; y++)
			row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
	/*=============================================================================*/
	png_read_image(png_ptr, row_pointers);
	fclose(fp);
	/*=============================================================================*/
	// SET MAP BIT IMAGE
	/*=============================================================================*/

	int np;
	if (color_type == 4){
		/*Gray scale image*/
		/*=================*/
		if (color_type == 3){np = 1;} // depending of color_type np must be changed
		if (color_type == 4){np = 2;}
		for (y=0; y<img.height; y++) {
			png_byte* row = row_pointers[y];
			for (x=0; x<img.width; x++) {
				png_byte* ptr = &(row[x*np]);
				img.matrix[x][y] = ptr[0];
				//printf("gray pixel = %d \n",(x));
			}
		}
	}
	else{
		/*RGB image*/
		/*=================*/
		if (color_type == 2){np = 3;}
		if (color_type == 6){np = 4;}
		for (y=0; y<img.height; y++) {
			png_byte* row = row_pointers[y];
			for (x=0; x<img.width; x++) {
				png_byte* ptr = &(row[x*np]);
			    /*transform RGB to gray scale*/
				img.matrix[x][y] = (float)ptr[0]*0.299+(float)ptr[1]*0.587+(float)ptr[2]*0.114;
				//printf("gray pixel = %d \n",img.matrix[x][y]);
			}
		}
	}
	img.picture = (char *)malloc((img.width+10)*(img.height+10)*sizeof(char[4]));        //dispose memory for ascii string

	printf("sizex = %d\n",img.width);
	printf("sizey = %d\n",img.height);
	/* =========================================================
	   RESIZE IMAGE
	   ==========================================================|*/
	if(bool_resize == true){    												//if bool resize is true the image will be resized
		int **newmatrix;
		newmatrix = (int**)malloc(newwidth*sizeof(int *));          			 //dispose the memory for the new image size
		for(int i = 0;i < newwidth;i++){
			newmatrix[i] = (int*)malloc(newheight*sizeof(int));
		}
		resize(img.matrix,img.width,img.height,newmatrix,newwidth,newheight);  //image resizing algorithm
		img.width = newwidth;													 //convert image size
		img.height = newheight;                                                      //free matrix memory
		img.matrix = (int**)malloc(img.width*sizeof(int *)); 					 //Return to set img.matrix 2D array memory size
		for(int i = 0;i < img.width;i++){
			img.matrix[i] = (int*)malloc(img.height*sizeof(int));
		}
		memcpy(img.matrix,newmatrix,img.width*img.height*sizeof(int));
		free(newmatrix);
		newmatrix = NULL;

	}
/* ==========================================================
	TRANSFORM BITMAP TO ASCII TEXT
	==========================================================|*/
	int n = 0; // for avoid double written character
	for(y = 0; y < img.height; y++){
		strcat(img.picture,"\n");
		for(x = 0;x <img.width;x++){
			n = 0;
			for(i = n_characters-1; i > -1;i--){
				if(img.matrix[x][y]>= colors_num[i-1] && img.matrix[x][y]<= colors_num[i] && n == 0){
					strcat(img.picture,characters[i]);
					n = 1;
				}
		    }
		}
	}

/* ==========================================================
	SAVE ASCII IMAGE
	==========================================================|*/
	FILE * saveimg;
	saveimg = fopen(savepath,"w");
	fprintf(saveimg,"%s",img.picture);
	fclose(saveimg);
	printf("Image converter correctly\n");
	printf("make sure the text editor you use correctly opens the text\n");
	return 0;
}
int resize(int **matrix,int height, int width,int **newmatrix,int newheight, int newwidth){
/* ==========================================================
	RESIZE FUNCTION
	==========================================================|*/
	int i = 0;
	int j = 0;
	int q,q1,q2;
	int v1,v2,v3,v4;
	float x_ceil,y_ceil,xfloor,yfloor;
	float x, y;
	float w_scale_factor = width  / (float)newwidth;
	float h_scale_factor = height / (float)newheight;
	for(i = 0;i< newheight;i++){
		for(j = 0;j<newwidth;j++){
			x = i*h_scale_factor;
			y = j*w_scale_factor;
			xfloor = floor(x);
			if (height-1 > ceil(x)){x_ceil = ceil(x);}
			if (height-1 < ceil(x)){x_ceil = height-1;}

			yfloor = floor(y);
			if (width-1 > ceil(y)){y_ceil = ceil(y);}
			if (width-1 < ceil(y)){y_ceil = width-1;}

			if(x_ceil == xfloor && y_ceil == yfloor){
				q = matrix[(int)x][(int)y];
			}
			else if(x_ceil == xfloor){
				q1 = matrix[(int)x][(int)yfloor];
				q2 = matrix[(int)x][(int)y_ceil];
				q = (q1 *(y_ceil -y)) +(q2 * (y-yfloor));
			}
			else if(y_ceil == yfloor){
				q1 = matrix[(int)xfloor][(int)y];
				q2 = matrix[(int)x_ceil][(int)y];
				q = (q1 *(x_ceil -x)) +(q2 * (x-xfloor));
			}
			else{
				v1 = matrix[(int)xfloor][(int)yfloor];
				v2 = matrix[(int)x_ceil][(int)yfloor];
				v3 = matrix[(int)xfloor][(int)y_ceil];
				v4 = matrix[(int)x_ceil][(int)y_ceil];
				q1 = v1*(x_ceil -x) + v2 *(x-xfloor);
				q2 = v3*(x_ceil -x) + v4 *(x-xfloor);
				q = q1*(y_ceil -y) + q2 *(y-yfloor);
			}
			newmatrix[i][j] = (int)q;

		}
	}
	/*original resizing algorithm code from -> https://meghal-darji.medium.com/implementing-bilinear-interpolation-for-image-resizing-357cbb2c2722     */
	return 0;
}
void abort_(const char * s, ...){
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}
