/*
Description:    Program to create a panorama from two input images, display it using OpenGL, optionally write it using OpenImageIO. 
Name:           Nitin Madhok
Date:           December 4, 2013
Email:          nmadhok@clemson.edu
*/

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cstdio>
#include <cstring>
#include <cmath>
#include "panorama.h"

#include <OpenImageIO/imageio.h>
OIIO_NAMESPACE_USING

using namespace std;


// Function to return the maximum of eight integers
int maximum(int a, int b, int c, int d, int e, int f, int g, int h) {
  int num[8] = {a, b, c, d, e, f, g, h};
  int max=num[0];

  for(int i=1; i<8; i++) {
    if(num[i]>max)
      max=num[i];
  }
  return max;
}

// Function to return the minimum of eight integers
int minimum(int a, int b, int c, int d, int e, int f, int g, int h) {
  int num[8] = {a, b, c, d, e, f, g, h};
  int min=num[0];

  for(int i=1; i<8; i++) {
    if(num[i]<min)
      min=num[i];
  }
  return min;
}


// Function to display input image1 using OpenGL.
void display1(){
  glClear(GL_COLOR_BUFFER_BIT);

  // Define the Drawing Coordinate system on the viewport
  // Lower left is (0, 0), Upper right is (width, height)
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, xres1, 0, yres1);

  // Specifies the raster position for pixel operations
  glRasterPos2i(0,0);
  
  // Writes a block of pixels to the framebuffer
  glDrawPixels(xres1,yres1,GL_RGBA,GL_UNSIGNED_BYTE, data1);

  // To Force execution of GL commands in finite time
  glFlush();
}

// Function to display input image2 using OpenGL.
void display2(){
  glClear(GL_COLOR_BUFFER_BIT);

  // Define the Drawing Coordinate system on the viewport
  // Lower left is (0, 0), Upper right is (width, height)
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, xres2, 0, yres2);

  // Specifies the raster position for pixel operations
  glRasterPos2i(0,0);
  
  // Writes a block of pixels to the framebuffer
  glDrawPixels(xres2,yres2,GL_RGBA,GL_UNSIGNED_BYTE, data2);

  // To Force execution of GL commands in finite time
  glFlush();
}

// Function to display panorama using OpenGL. 
void display3(){
  glClear(GL_COLOR_BUFFER_BIT);

  // Define the Drawing Coordinate system on the viewport
  // Lower left is (0, 0), Upper right is (width, height)
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, xresWarped, 0, yresWarped);

  // Specifies the raster position for pixel operations
  glRasterPos2i(0,0);
  
  // Writes a block of pixels to the framebuffer
  glDrawPixels(xresWarped,yresWarped,GL_RGBA,GL_UNSIGNED_BYTE, dataWarped);

  // To Force execution of GL commands in finite time
  glFlush();
}

// Function to draw maximum four points on each input image upon mouse click
void drawSquare(int x, int y, int win) {
  // Set Point size 
  glPointSize(7);

  // Enable GL_POINT_SMOOTH so that the point is not a square but a circle
  glEnable(GL_POINT_SMOOTH);

  // If points clicked on Input Image1 then draw points on it
  if(win==1) {
    y = yres1-y;
    switch(counter1) {
      case 3: 
        point1_image1[0] = x;
        point1_image1[1] = y;
        glColor3f(1.0f, 0.0f, 0.0f);
        break;
      case 2: 
        point2_image1[0] = x;
        point2_image1[1] = y;
        glColor3f(0.0f, 0.0f, 1.0f);
        break;
      case 1: 
        point3_image1[0] = x;
        point3_image1[1] = y;
        glColor3f(0.0f, 1.0f, 0.0f);
        break;
      case 0: 
        point4_image1[0] = x;
        point4_image1[1] = y;
        glColor3f(1.0f, 1.0f, 0.0f);
        break;
    }
  }
  // If points clicked on Input Image2 then draw points on it
  else if(win==2) {
    y = yres2-y;
    switch(counter2) {
      case 3:  
        point1_image2[0] = x;
        point1_image2[1] = y;
        b[0]=x;
        b[1]=y;
        glColor3f(1.0f, 0.0f, 0.0f);
        break;
      case 2: 
        point2_image2[0] = x;
        point2_image2[1] = y;
        b[2]=x;
        b[3]=y;
        glColor3f(0.0f, 0.0f, 1.0f);
        break;
      case 1: 
        point3_image2[0] = x;
        point3_image2[1] = y;
        b[4]=x;
        b[5]=y;
        glColor3f(0.0f, 1.0f, 0.0f);
        break;
      case 0: 
        point4_image2[0] = x;
        point4_image2[1] = y;
        b[6]=x;
        b[7]=y;
        glColor3f(1.0f, 1.0f, 0.0f);
        break;
    }
  }
  glBegin(GL_POINTS);
  glVertex2f(x , y);
  glEnd();
  glutSwapBuffers();

  // If all 4 points on each input image have been clicked then compute the homography and create the panorama
  if(counter1==0 && counter2==0) {
    computeHomographyMatrix();
    if(numberOfArguments==4) {
      writeImage();
      cout<<"File: "<<outfilename<<" written successfully!"<<endl;
    }
  }
}


// Function to write an image using OpenImageIO that reads and stores the pixel bein    g displayed on screen using OpenGL
void writeImage() {
 
  // Store the Output File Type in outfiletype, example .ppm or .jpg
  string outfiletype = outfilename.substr(outfilename.find("."));

  // Create ImageOutput instance using the outfilename & exit if error in creating
  ImageOutput *out = ImageOutput::create(outfilename);
  if (!out) {
    cerr << "Could not create an ImageOutput for " 
         << outfilename << "\nError: "
         << geterror()<<endl;
    exit(-1);
  }

  // Set outputchannels to 3 if outputfiletype is either ppm/pnm/pgm/pbm/hdr/rgbe else let it be equal to the number of channels of the input image (either 3 or 4)
  int outputchannels = (outfiletype==".ppm" || outfiletype==".pnm" || outfiletype==".pgm" || outfiletype==".pbm" || outfiletype==".hdr" || outfiletype==".rgbe" ? 3 : channels1 );

  // Allocate memory based on the number of channels
  unsigned char *oiio_pixels = new unsigned char[xresWarped*yresWarped*outputchannels];
  // Check if memory has been allocated successfully
  if (oiio_pixels==0) {
    // Memory not allocated successfully! Display message and Exit
    cout<<"Couldn't allocate memory. Exiting!"<<endl;
    exit(-1);
    delete out;
  }
 
  // If number of channels is 4 then read in RGBA format using GL_RGBA
  if(outputchannels==4) {
    for(int i=0, k=0; i<yresWarped && k<(xresWarped*yresWarped*outputchannels); i++) {
      for(int j=0; j<xresWarped; j++, k+=4) {
        oiio_pixels[k] = pixmapWarped[i][j].red;
        oiio_pixels[k+1] = pixmapWarped[i][j].green;
        oiio_pixels[k+2] = pixmapWarped[i][j].blue;
        oiio_pixels[k+3] = pixmapWarped[i][j].alpha;
      }
    }
  }
 
  // If number of channels is 3 then read in RGB format using GL_RGB
  else if(outputchannels==3) {
    for(int i=0, k=0; i<yresWarped && k<(xresWarped*yresWarped*outputchannels); i++) {
      for(int j=0; j<xresWarped; j++, k+=3) {
        oiio_pixels[k] = pixmapWarped[i][j].red;
        oiio_pixels[k+1] = pixmapWarped[i][j].green;
        oiio_pixels[k+2] = pixmapWarped[i][j].blue;
      }
    }
  }

  // Create ImageSpec for the output image with name outfile
  ImageSpec spec(xresWarped,yresWarped,outputchannels,TypeDesc::UINT8);
  if (! out->open (outfilename, spec)) {
    cerr << "Could not open " 
         << outfilename << "\nError: "
         << out->geterror()<< endl;
    delete out;
    delete [] oiio_pixels;
    exit(-1);
  }

  // This particular call to write flips the image for us
  int scanlinesize = xresWarped * outputchannels * sizeof(oiio_pixels[0]);
  if(! out->write_image (TypeDesc::UINT8, (unsigned char*)oiio_pixels+(yresWarped-1)*scanlinesize, AutoStride, -scanlinesize, AutoStride)) {
     cerr << "Could not write pixels to " 
          << outfilename << "\nError: "
          << out->geterror()<< endl;
     delete out;
     delete [] oiio_pixels;
     exit(-1);
   }

  // Close the output file
  if(! out->close ()) {
    std::cerr << "Error closing " 
              << outfilename << "\nError: " 
              << out->geterror() << endl;
    delete out;
    delete [] oiio_pixels;
    exit(-1);
  }
  delete out;
  delete [] oiio_pixels;
}


// Keyboard Callback Routine: 
// 'q' or 'Q' or ESC to quit
// This routine is called every time a key is pressed on the keyboard
void keyboard(unsigned char key, int x, int y){
  switch(key){
    
    // If 'q' or 'Q' or ESC key is pressed, then Exit
    case 'q':
    case 'Q':
    case 27:
      exit(0);

    // If any other key is pressed then do nothing and ignore the key press
    default: 
      return;
  }
}


// Mouse Callback Routine: 
// This routine is called every time a mouse click is encountered
void handleMouseClick1(int button, int state, int x, int y){
  switch(button){
    // If Left Click
    case GLUT_LEFT_BUTTON:
      // If click released and there are points left to be clicked on input image1
      if(state == GLUT_DOWN && counter1>0){
        counter1--;
        drawSquare(x,y,1);
      }

    // If any other mouse click is encountered then do nothing and ignore the click
    default:
      return;
  }
}

// Mouse Callback Routine: 
// This routine is called every time a mouse click is encountered
void handleMouseClick2(int button, int state, int x, int y){
  switch(button){
    // If Left Click
    case GLUT_LEFT_BUTTON:
      // If click released and there are points left to be clicked on input image1
      if(state == GLUT_DOWN && counter2>0){
        counter2--;
        drawSquare(x,y,2);
      }

    // If any other mouse click is encountered then do nothing and ignore the click
    default:
      return;
  }
}



// Function to create the display window using OpenGL
void CreateGLWindow(int w, int h) {
  // Create the Graphics Window, giving width & height & title text
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);


  // Window 2
  glutInitWindowSize(xres2, yres2);
  glutCreateWindow(filename2);
  glutPositionWindow(w+10,0);

  // Set up the callback routines to be called when glutMainLoop() detects an event
  glutSetCursor(GLUT_CURSOR_INFO);	// Set Cursor to Pointing Hand
  glutDisplayFunc(display2);		// Display callback
  glutKeyboardFunc(keyboard);		// Keyboard callback
  glutMouseFunc(handleMouseClick2);	// Mouse Click callback


  // Window 1
  glutInitWindowSize(xres1, yres1);
  glutCreateWindow(filename1);
  glutPositionWindow(0,0);

  // Set up the callback routines to be called when glutMainLoop() detects an event
  glutSetCursor(GLUT_CURSOR_INFO);	// Set Cursor to Pointing Hand
  glutDisplayFunc(display1);		// Display callback
  glutKeyboardFunc(keyboard);		// Keyboard callback
  glutMouseFunc(handleMouseClick1);	// Mouse Click callback
}


// Function to compute the homography matrix, warp the second imput image using inverse warp and stitch it with the first input image. Magnifications artifacts are fixed by using Bilinear Interpolation. 
void computeHomographyMatrix() {

  // Compute 8x8 Matrix
  A[0][0] = point1_image1[0];
  A[0][1] = point1_image1[1];
  A[0][2] = 1;
  A[0][3] = 0;
  A[0][4] = 0;
  A[0][5] = 0;
  A[0][6] = -(point1_image1[0] * point1_image2[0]);
  A[0][7] = -(point1_image1[1] * point1_image2[0]);
  A[1][0] = 0;
  A[1][1] = 0;
  A[1][2] = 0;
  A[1][3] = point1_image1[0];
  A[1][4] = point1_image1[1];
  A[1][5] = 1;
  A[1][6] = -(point1_image1[0] * point1_image2[1]);
  A[1][7] = -(point1_image1[1] * point1_image2[1]);

  A[2][0] = point2_image1[0];
  A[2][1] = point2_image1[1];
  A[2][2] = 1;
  A[2][3] = 0;
  A[2][4] = 0;
  A[2][5] = 0;
  A[2][6] = -(point2_image1[0] * point2_image2[0]);
  A[2][7] = -(point2_image1[1] * point2_image2[0]);
  A[3][0] = 0;
  A[3][1] = 0;
  A[3][2] = 0;
  A[3][3] = point2_image1[0];
  A[3][4] = point2_image1[1];
  A[3][5] = 1;
  A[3][6] = -(point2_image1[0] * point2_image2[1]);
  A[3][7] = -(point2_image1[1] * point2_image2[1]);

  A[4][0] = point3_image1[0];
  A[4][1] = point3_image1[1];
  A[4][2] = 1;
  A[4][3] = 0;
  A[4][4] = 0;
  A[4][5] = 0;
  A[4][6] = -(point3_image1[0] * point3_image2[0]);
  A[4][7] = -(point3_image1[1] * point3_image2[0]);
  A[5][0] = 0;
  A[5][1] = 0;
  A[5][2] = 0;
  A[5][3] = point3_image1[0];
  A[5][4] = point3_image1[1];
  A[5][5] = 1;
  A[5][6] = -(point3_image1[0] * point3_image2[1]);
  A[5][7] = -(point3_image1[1] * point3_image2[1]);

  A[6][0] = point4_image1[0];
  A[6][1] = point4_image1[1];
  A[6][2] = 1;
  A[6][3] = 0;
  A[6][4] = 0;
  A[6][5] = 0;
  A[6][6] = -(point4_image1[0] * point4_image2[0]);
  A[6][7] = -(point4_image1[1] * point4_image2[0]);
  A[7][0] = 0;
  A[7][1] = 0;
  A[7][2] = 0;
  A[7][3] = point4_image1[0];
  A[7][4] = point4_image1[1];
  A[7][5] = 1;
  A[7][6] = -(point4_image1[0] * point4_image2[1]);
  A[7][7] = -(point4_image1[1] * point4_image2[1]);

  // Computer Inverse of 8x8 matrix
  invA = A.inv();

  // Multiply the inverse of 8x8 matrix with the 1x8 vector having points from input image2 to get the 1x8 homography vector
  h=invA*b;
  
  // Create the homography matrix from the homography vector
  M[0][0] = h[0]; // a
  M[0][1] = h[1]; // b
  M[0][2] = h[2]; // c
  M[1][0] = h[3]; // d
  M[1][1] = h[4]; // e
  M[1][2] = h[5]; // f
  M[2][0] = h[6]; // g
  M[2][1] = h[7]; // h
  M[2][2] = 1;    // i

  // Compute the inverse of homography matrix
  invM=M.inv();

  // Corner Points of input image1
  Vector3d bottom_left1(0,0,1);
  Vector3d bottom_right1(xres1,0,1); 
  Vector3d top_left1(0,yres1,1);
  Vector3d top_right1(xres1,yres1,1); 

  // Corner Points of input image2
  Vector3d bottom_left2(0,0,1);
  Vector3d bottom_right2(xres2,0,1); 
  Vector3d top_left2(0,yres2,1);
  Vector3d top_right2(xres2,yres2,1); 

  // Compute the mapping of corner points of input image2 upon warp
  Vector3d new_bottom_left = invM * bottom_left2;
  Vector3d new_bottom_right = invM * bottom_right2;
  Vector3d new_top_left = invM * top_left2;
  Vector3d new_top_right = invM * top_right2;

  // Normalize the points
  new_bottom_left[0] = new_bottom_left[0] / new_bottom_left[2];
  new_bottom_left[1] = new_bottom_left[1] / new_bottom_left[2];
  new_bottom_left[2] = new_bottom_left[2] / new_bottom_left[2];
  new_bottom_right[0] = new_bottom_right[0] / new_bottom_right[2];
  new_bottom_right[1] = new_bottom_right[1] / new_bottom_right[2];
  new_bottom_right[2] = new_bottom_right[2] / new_bottom_right[2];
  new_top_left[0] = new_top_left[0] / new_top_left[2];
  new_top_left[1] = new_top_left[1] / new_top_left[2];
  new_top_left[2] = new_top_left[2] / new_top_left[2];
  new_top_right[0] = new_top_right[0] / new_top_right[2];
  new_top_right[1] = new_top_right[1] / new_top_right[2];
  new_top_right[2] = new_top_right[2] / new_top_right[2];

  // Find the minimum and maximum x,y coordinates from taking all points from input image1 and input image2
  min_x = minimum(new_bottom_left[0], new_bottom_right[0], new_top_left[0], new_top_right[0], bottom_left1[0], bottom_right1[0], top_left1[0], top_right1[0]);
  min_y = minimum(new_bottom_left[1], new_bottom_right[1], new_top_left[1], new_top_right[1], bottom_left1[1], bottom_right1[1], top_left1[1], top_right1[1]);
  max_x = maximum(new_bottom_left[0], new_bottom_right[0], new_top_left[0], new_top_right[0], bottom_left1[0], bottom_right1[0], top_left1[0], top_right1[0]);
  max_y = maximum(new_bottom_left[1], new_bottom_right[1], new_top_left[1], new_top_right[1], bottom_left1[1], bottom_right1[1], top_left1[1], top_right1[1]);

  // Compute the origin
  Vector3d origin(min_x, min_y, 0);

  // Compute the size of the panorama
  xresWarped = max_x - min_x;
  yresWarped = max_y - min_y;

  cout<<"\n\nPanorama Specification:"<<endl;
  cout<<"Image Width: "<<xresWarped<<endl;
  cout<<"Image Height: "<<yresWarped<<endl;

  cout<<"\nDisplaying Panorama"<<endl;

  if(xresWarped == 0) {
    xresWarped = 1;
  }
  if(yresWarped == 0) {
    yresWarped = 1;
  }

  // Allocate memory of size equal to the height of the image to store pointers
  //pixmapWarped = new pixel*[spec.height];
  pixmapWarped = new pixel*[yresWarped];
  // Check if memory has been allocated successfully
  if (pixmapWarped==0) {
    // Memory not allocated successfully! Display message and Exit
    cout<<"Couldn't allocate memory. Exiting!"<<endl;
    exit(-1);
  }

  // Allocate memory to store all the RGBA data from the image of size=width*height! (Contiguous Allocation)
  dataWarped = new pixel[yresWarped*xresWarped];
  // Check if memory has been allocated successfully
  if (dataWarped==0) {
    // Memory not allocated successfully! Display message and Exit
    cout<<"Couldn't allocate memory. Exiting!"<<endl;
    exit(-1);
    delete[] pixmapWarped;
  }

  // Set the pointer present at 0th position of pixmap to the data
  pixmapWarped[0] = dataWarped;

  // Set the rest of the pointers to the different rows of the image. Notice it can be accessed like a 2D array
  for (int y=1; y<yresWarped; y++) {
    pixmapWarped[y] = pixmapWarped[y-1] + xresWarped;
  }

  for (int y = 0; y < yresWarped; y++) {
    for (int x = 0; x < xresWarped; x++) {
      //map the pixel coordinates
      Vector3d pixel_out(x, y, 1);
      pixel_out = pixel_out + origin;
      Vector3d pixel_in = M * pixel_out;
      //normalize the pixmap
      float m = pixel_in[0] / pixel_in[2];
      float n = pixel_in[1] / pixel_in[2];

      // Use Bilinear Interpolation to fix magnification artifacts
      int k = floor(m);
      int j = floor(n);
      float a = n - j;
      float b = m - k;
      int p = floor(pixel_out[0]);
      int q = floor(pixel_out[1]); 

      // Take pixel from input image2
      if((j>=0 && j<yres2-1) && (k>=0 && k<xres2-1)) {
        pixmapWarped[y][x].red = (((1-b) * ((pixmap2[j][k].red * (1-a)) + (pixmap2[j+1][k].red * a))) + (b * ((pixmap2[j][k+1].red * (1-a)) + (pixmap2[j+1][k+1].red * a))));
        pixmapWarped[y][x].green = (((1-b) * ((pixmap2[j][k].green * (1-a)) + (pixmap2[j+1][k].green * a))) + (b * ((pixmap2[j][k+1].green * (1-a)) + (pixmap2[j+1][k+1].green * a))));
        pixmapWarped[y][x].blue = (((1-b) * ((pixmap2[j][k].blue * (1-a)) + (pixmap2[j+1][k].blue * a))) + (b * ((pixmap2[j][k+1].blue * (1 -a)) + (pixmap2[j+1][k+1].blue * a))));
        pixmapWarped[y][x].alpha = (((1-b) * ((pixmap2[j][k].alpha * (1-a)) + (pixmap2[j+1][k].alpha * a))) + (b * ((pixmap2[j][k+1].alpha * (1-a)) + (pixmap2[j+1][k+1].alpha * a))));
      }
      // Take pixel from input image2
      else if(j==yres2-1 && k==xres2-1) {
        pixmapWarped[y][x] = pixmap2[j][k];
      }
      // Take pixel from input image1
      else if (q>=0 && q<yres1 && p>=0 && p<xres1) {
	pixmapWarped[y][x] = pixmap1[q][p];
      } 
      else {
	// Black pixel 
      }
    }
  }
 
  // Window 3
  glutInitWindowSize(xresWarped, yresWarped);
  glutCreateWindow("Final Panorama");

  // Set up the callback routines to be called when glutMainLoop() detects an event
  glutDisplayFunc(display3);		// Display callback
  glutKeyboardFunc(keyboard);		// Keyboard callback
 
}


// Main function
int main(int argc, char *argv[]) {

  // Check for the number of arguments. Display message and exit if incorrect number of arguments
  if((argc < 3) || (argc > 4)){
    cerr<<"Usage: "<<argv[0]<<" inputimage1 inputimage2 "<<endl;
    cerr<<"            -OR-"<<endl;
    cerr<<"Usage: "<<argv[0]<<" inputimage1 "<<"inputimage2 outputimage"<<endl;
    return 1;
  } 

  numberOfArguments = argc;

  // If there are three command line arguments passed then store the fourth argument as output file name
  if(argc==4) {
    outfilename = argv[3];
  }

  // Set pointerd to the second and third command line argument (input file)
  filename1 = argv[1];
  filename2 = argv[2];

  // Input file name is equal to the second and third command line argument
  infilename1 = argv[1];
  infilename2 = argv[2];

  // Create an ImageInput for the input image and exit if error occurs
  ImageInput *in1 = ImageInput::create(filename1);
  if(!in1) {
    cerr<<"Could not create an ImageInput for "
        <<filename1<<"\nError: "
        <<geterror()<<endl;
    exit(-1);
  }

  // Create an ImageSpec and open the image. If error occurs then exit
  ImageSpec spec1;
  if (! in1->open (filename1, spec1)) {
    cerr<<"Could not open "
        <<filename1<<"\nError: "
        <<in1->geterror()<<endl;
    delete in1;
    exit(-1);
  }

  // Create an ImageInput for the input image and exit if error occurs
  ImageInput *in2 = ImageInput::create(filename2);
  if(!in2) {
    cerr<<"Could not create an ImageInput for "
        <<filename2<<"\nError: "
        <<geterror()<<endl;
    exit(-1);
    delete in1;
  }

  // Create an ImageSpec and open the image. If error occurs then exit
  ImageSpec spec2;
  if (! in2->open (filename2, spec2)) {
    cerr<<"Could not open "
        <<filename2<<"\nError: "
        <<in2->geterror()<<endl;
    delete in2;
    delete in1;
    exit(-1);
  }

  // Store the width, height, number of channels, names of channels and channel format in variables
  cout<<"\nFirst Input Image Specification: "<<endl;
  xres1 = spec1.width;
  cout<<"Image Width: "<<xres1<<endl;
  yres1 = spec1.height;
  cout<<"Image Height: "<<yres1<<endl;
  channels1 = spec1.nchannels;
  cout<<"No. of Channels: "<<channels1<<endl;
  cout<<"Channel Names: ";
  for (int i = 0;  i < spec1.nchannels;  ++i)
    cout<<spec1.channelnames[i] << " ";
  cout<<"\n";
  cout<<"Color Depth: "<<sizeof(spec1.format)*spec1.nchannels<<"bpp"<<endl;
 
  // Store the width, height, number of channels, names of channels and channel format in variables
  cout<<"\n\nSecond Input Image Specification: "<<endl;
  xres2 = spec2.width;
  cout<<"Image Width: "<<xres2<<endl;
  yres2 = spec2.height;
  cout<<"Image Height: "<<yres2<<endl;
  channels2 = spec1.nchannels;
  cout<<"No. of Channels: "<<channels2<<endl;
  cout<<"Channel Names: ";
  for (int i = 0;  i < spec2.nchannels;  ++i)
    cout<<spec2.channelnames[i] << " ";
  cout<<"\n";
  cout<<"Color Depth: "<<sizeof(spec2.format)*spec2.nchannels<<"bpp"<<endl;
 
 
  // Allocate memory to store image data read using OpenImageIO
  pixels1 = new unsigned char [xres1*yres1*channels1];
  pixels2 = new unsigned char [xres2*yres2*channels2];
  
  // Obtain the size in bits of a single scanline
  int scanlinesize1 = spec1.width * spec1.nchannels * sizeof(pixels1[0]);
  int scanlinesize2 = spec2.width * spec2.nchannels * sizeof(pixels2[0]);

  // Read the image and exit if error occurs. This also flips the image automatically for us
  if(! in1->read_image (TypeDesc::UINT8, (char *)pixels1+(yres1-1)*scanlinesize1, AutoStride, -scanlinesize1, AutoStride)) {
    cerr<<"Could not read pixels from "
        <<filename1<<"\nError: "
        <<in1->geterror()<<endl;
    exit(-1);
    delete pixels2;
    delete pixels1;
    delete in2;
    delete in1;
  }

  // Read the image and exit if error occurs. This also flips the image automatically for us
  if(! in2->read_image (TypeDesc::UINT8, (char *)pixels2+(yres2-1)*scanlinesize2, AutoStride, -scanlinesize2, AutoStride)) {
    cerr<<"Could not read pixels from "
        <<filename2<<"\nError: "
        <<in2->geterror()<<endl;
    exit(-1);
    delete pixels2;
    delete pixels1;
    delete in2;
    delete in1;
  }

  // Allocate memory of size equal to the height of the image to store pointers
  pixmap1 = new pixel*[spec1.height];
  // Check if memory has been allocated successfully
  if (pixmap1==0) {
    // Memory not allocated successfully! Display message and Exit
    cout<<"Couldn't allocate memory. Exiting!"<<endl;
    exit(-1);
    delete pixels2;
    delete pixels1;
    delete in2;
    delete in1;
  }

  // Allocate memory to store all the RGBA data from the image of size=width*height! (Contiguous Allocation)
  data1 = new pixel[spec1.height*spec1.width];
  // Check if memory has been allocated successfully
  if (data1==0) {
    // Memory not allocated successfully! Display message and Exit
    cout<<"Couldn't allocate memory. Exiting!"<<endl;
    exit(-1);
    delete[] pixmap1;
    delete pixels2;
    delete pixels1;
    delete in2;
    delete in1;
  }

  // Set the pointer present at 0th position of pixmap to the data
  pixmap1[0] = data1;

  // Set the rest of the pointers to the different rows of the image. Notice it can be accessed like a 2D array
  for (int y=1; y<yres1; y++) {
    pixmap1[y] = pixmap1[y-1] + xres1;
  }

  //If the input image has 3 channels
  if(spec1.nchannels==3) {
      // Read in all the RGB data and store it in data specifying value of alpha as 255
      for (int j=0, i=0; j<(xres1*yres1*channels1) && i<(xres1*yres1); j+=3, i++) {
        data1[i].red = pixels1[j];
        data1[i].green = pixels1[j+1];
        data1[i].blue = pixels1[j+2];
        data1[i].alpha = 255;
      }
  }
  //If the input image has 4 channels (Alpha)
  else if(spec1.nchannels==4) {
      // Read in all the RGBA data and store it in data
      for (int j=0, i=0; j<(xres1*yres1*channels1) && i<(xres1*yres1); j+=4, i++) {
        data1[i].red = pixels1[j];
        data1[i].green = pixels1[j+1];
        data1[i].blue = pixels1[j+2];
        data1[i].alpha = pixels1[j+3];
      }
  }

  // Allocate memory of size equal to the height of the image to store pointers
  pixmap2 = new pixel*[spec2.height];
  // Check if memory has been allocated successfully
  if (pixmap2==0) {
    // Memory not allocated successfully! Display message and Exit
    cout<<"Couldn't allocate memory. Exiting!"<<endl;
    exit(-1);
    delete[] data1;
    delete[] pixmap1;
    delete pixels2;
    delete pixels1;
    delete in2;
    delete in1;
  }

  // Allocate memory to store all the RGBA data from the image of size=width*height! (Contiguous Allocation)
  data2 = new pixel[spec2.height*spec2.width];
  // Check if memory has been allocated successfully
  if (data2==0) {
    // Memory not allocated successfully! Display message and Exit
    cout<<"Couldn't allocate memory. Exiting!"<<endl;
    exit(-1);
    delete[] pixmap2;
    delete[] data1;
    delete[] pixmap1;
    delete pixels2;
    delete pixels1;
    delete in2;
    delete in1;
  }

  // Set the pointer present at 0th position of pixmap to the data
  pixmap2[0] = data2;

  // Set the rest of the pointers to the different rows of the image. Notice it can be accessed like a 2D array
  for (int y=1; y<yres2; y++) {
    pixmap2[y] = pixmap2[y-1] + xres2;
  }

  //If the input image has 3 channels
  if(spec2.nchannels==3) {
      // Read in all the RGB data and store it in data specifying value of alpha as 255
      for (int j=0, i=0; j<(xres2*yres2*channels2) && i<(xres2*yres2); j+=3, i++) {
        data2[i].red = pixels2[j];
        data2[i].green = pixels2[j+1];
        data2[i].blue = pixels2[j+2];
        data2[i].alpha = 255;
      }
  }
  //If the input image has 4 channels (Alpha)
  else if(spec2.nchannels==4) {
      // Read in all the RGBA data and store it in data
      for (int j=0, i=0; j<(xres2*yres2*channels2) && i<(xres2*yres2); j+=4, i++) {
        data2[i].red = pixels2[j];
        data2[i].green = pixels2[j+1];
        data2[i].blue = pixels2[j+2];
        data2[i].alpha = pixels2[j+3];
      }
  }


  // Close the input file and exit if error occurs
  if(! in1->close ()) {
    cerr<<"Error closing "
        <<filename1<<"\nError: "
        <<in1->geterror()<<endl;
    exit(-1);
    delete[] data2;
    delete[] pixmap2;
    delete[] data1;
    delete[] pixmap1;
    delete pixels2;
    delete pixels1;
    delete in2;
    delete in1;
  }

  // Close the input file and exit if error occurs
  if(! in2->close ()) {
    cerr<<"Error closing "
        <<filename2<<"\nError: "
        <<in2->geterror()<<endl;
    exit(-1);
    delete[] data2;
    delete[] pixmap2;
    delete[] data1;
    delete[] pixmap1;
    delete pixels2;
    delete pixels1;
    delete in2;
    delete in1;
  }

  // Start up the GLUT utilities
  glutInit(&argc, argv);
  CreateGLWindow(xres1, yres1);

  // Routine that loops forever looking for events. It calls the registered callback routine to handle each event that is detected
  glutMainLoop();

  // Delete the memory allocated manualy at the end of the program
  delete[] dataWarped;
  delete[] pixmapWarped;
  delete[] data2;
  delete[] pixmap2;
  delete[] data1;
  delete[] pixmap1;
  delete pixels2;
  delete pixels1;
  delete in2;
  delete in1;
}
