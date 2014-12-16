/*
 * Program template for HW 7 to warp an image using projective warps
 * 
 * Command line parameters are as follows:
 *
 * warper infile.png [outfile.png] 
 *
 * Author: Joshua A. Levine, 10/21/12
 */

#include "Transformation.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cfloat>

using namespace std;

/*
   Convert the string s to lower case
*/
void lowercase(char *s){
   int i;

   if(s != NULL) {
      for(i = 0; s[i] != '\0'; i++) {
         if(s[i] >= 'A' && s[i] <= 'Z')
            s[i] += ('a' - 'A');
      }
   }
}



/* 
   Multiply M by a rotation matrix of angle theta
*/

void Rotate(Matrix3x3 &M, float theta){
   int row, col;
   Matrix3x3 R(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
   double rad, c, s;

   rad = PI * theta / 180.0;
   c = cos(rad);
   s = sin(rad);

   R[0][0] = c;
   R[0][1] = -s;
   R[1][0] = s;
   R[1][1] = c;

   Matrix3x3 Prod = R * M;

   for(row = 0; row < 3; row++) {
      for(col = 0; col < 3; col++) {
         M[row][col] = Prod[row][col];
      }
   }
}


void Scale(Matrix3x3 &M, float sx, float sy){
  int row, col;
  Matrix3x3 R(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

  R[0][0] = sx;
  R[1][1] = sy;

  Matrix3x3 Prod = R * M;

  for(row = 0; row < 3; row++) {
    for(col = 0; col < 3; col++) {
      M[row][col] = Prod[row][col];
    }
  }
}

void Translate(Matrix3x3 &M, float dx, float dy){
  int row, col;
  Matrix3x3 R(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

  R[0][2] = dx;
  R[1][2] = dy;

  Matrix3x3 Prod = R * M;

  for(row = 0; row < 3; row++) {
    for(col = 0; col < 3; col++) {
      M[row][col] = Prod[row][col];
    }
  }
}

void Flip(Matrix3x3 &M, int xf, int yf){
  int row, col;
  Matrix3x3 R(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

  if(yf==1) {
    R[1][1] = -1;
    R[1][2] = yf;
  }
  if(xf==1) {
    R[0][0] = -1;
    R[0][2] = xf;
  }

  Matrix3x3 Prod = R * M;

  for(row = 0; row < 3; row++) {
    for(col = 0; col < 3; col++) {
      M[row][col] = Prod[row][col];
    }
  }
}

void Shear(Matrix3x3 &M, float hx, float hy){
  int row, col;
  Matrix3x3 R(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

  R[0][1] = hx;
  R[1][0] = hy;

  Matrix3x3 Prod = R * M;

  for(row = 0; row < 3; row++) {
    for(col = 0; col < 3; col++) {
      M[row][col] = Prod[row][col];
    }
  }
}

void Perspective(Matrix3x3 &M, float px, float py){
  int row, col;
  Matrix3x3 R(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

  R[2][0] = px;
  R[2][1] = py;

  Matrix3x3 Prod = R * M;

  for(row = 0; row < 3; row++) {
    for(col = 0; col < 3; col++) {
      M[row][col] = Prod[row][col];
    }
  }
}


/*
   Routine to build a projective transform from input text, display, or
   write transformed image to a file
*/
void process_input(Matrix3x3 &M){
   char command[1024];
   bool done;
   float theta;
   float sx, sy, dx, dy, hx, hy, px, py;
   int xf, yf;

   /* build identity matrix */
   M.identity();

   printf("\nEnter specifications for the transformation matrix:\n");
   cout<<"r \u03B8     : counter clockwise rotation about image origin, \u03B8 in degrees\n"
       <<"s sx sy : scale\n"
       <<"t dx dy : translate\n"
       <<"f xf yf : flip (if xf = 1 flip x coordinates, if yf = 1 flip y coordinates)\n"
       <<"h hx hy : shear\n"
       <<"p px py : perspective\n"
       <<"d       : done\n"
       <<"q       : exit or quit from program\n"<<endl;

   for(done = false; !done;) {

      /* prompt and accept input, converting text to lower case */
      printf("> ");
      scanf("%s", command);
      lowercase(command);

      /* parse the input command, and read parameters as needed */
      if(strcmp(command, "d") == 0) {
         done = true;
      } else if(strlen(command) != 1) {
         printf("invalid command, enter r, s, t, f, h, p, d\n");
      } else {
         switch(command[0]) {

            case 'r':		/* Rotation, accept angle in degrees */
               if(scanf("%f", &theta) == 1)
                  Rotate(M, theta);
               else
                  fprintf(stderr, "invalid rotation angle\n");
               break;
            case 's':		/* Scale, accept scale factors */
               if(scanf("%f %f", &sx, &sy) == 2)
                  Scale(M, sx, sy);
               else
                  fprintf(stderr, "invalid scale factor\n");
               break;
            case 't':		/* Translation, accept translations */
               if(scanf("%f %f", &dx, &dy) == 2)
                  Translate(M, dx, dy);
               else
                  fprintf(stderr, "invalid translate factor\n");
               break;
            case 'f':		/* Mirror, accept 0/1, 0/1 for x mirror, ymirror */
               if(scanf("%d %d", &xf, &yf) == 2) {
                 if((xf==1 || xf==0) && (yf==1 || yf==0)) {
                   Flip(M, xf, yf);
                 }
                 else 
                   fprintf(stderr, "invalid flip factor: flip factor can only be either 0 or 1\n");
               }
               else
                  fprintf(stderr, "invalid flip factor\n");
               break;
            case 'h':		/* Shear, accept shear factors */
               if(scanf("%f %f", &hx, &hy) == 2)
                  Shear(M, hx, hy);
               else
                  fprintf(stderr, "invalid shear factor\n");
               break;
            case 'p':		/* Perspective, accept perspective factors */
               if(scanf("%f %f", &px, &py) == 2)
                  Perspective(M, px, py);
               else
                  fprintf(stderr, "invalid perspective factor\n");
               break;
            case 'd':		/* Done, that's all for now */
               done = true;
               break;
            case 'q':
               exit(1);
            default:
               printf("invalid command, enter r, s, t, f, h, p, d\n");
         }
      }
   }
}
