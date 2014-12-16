/*
Description:    Header File containing structs for pixel RGBA information, other variables used as well as function declarations.
Name:           Nitin Madhok
Date:           December 4, 2013
Email:          nmadhok@clemson.edu
*/


#include <cstdio>
#include <cstring>
#include "Transformation.h"

struct pixel {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
};


Vector3d point1_image1(0,0,1);
Vector3d point2_image1(0,0,1);
Vector3d point3_image1(0,0,1);
Vector3d point4_image1(0,0,1);
Vector3d point1_image2(0,0,1);
Vector3d point2_image2(0,0,1);
Vector3d point3_image2(0,0,1);
Vector3d point4_image2(0,0,1);

Vector h(8);
Vector b(8);
Matrix A(8,8);
Matrix invA(8,8);
Matrix3x3 M;
Matrix3x3 invM;

// Variables to store the image width, image height and number of channels
int xres1, yres1, channels1;
int xres2, yres2, channels2;
int xresWarped, yresWarped;
int min_x, min_y, max_x, max_y;

int numberOfArguments = 0;

int counter1 = 4, counter2 = 4;

// Variables to store the output file name and input filenames
string outfilename;
string infilename1;
string infilename2;
const char *filename1;
const char *filename2;

// To store the image data read using OpenImageIO 
unsigned char *pixels1;
unsigned char *pixels2;

// Create double pointers of type pixel. Will be used as pixmaps to store image rgba information 
pixel **pixmap1;
pixel **pixmap2;
pixel **pixmapWarped;

// Create pointers of type pixel. Will be used to store rgba data in a contiguous block
pixel *data1;
pixel *data2;
pixel *dataWarped;


int maximum(int, int, int, int, int, int, int, int);
int minimum(int, int, int, int, int, int, int, int);
void display1();
void display2();
void display3();
void drawSquare(int, int, int);
void writeImage();
void keyboard(unsigned char, int, int);
void handleMouseClick1(int, int, int, int);
void handleMouseClick2(int, int, int, int);
void CreateGLWindow();
void computeHomographyMatrix();
