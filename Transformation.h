#include "Matrix.h"

void lowercase(char *);
void Rotate(Matrix3x3 &, float);
void Scale(Matrix3x3 &, float, float);
void Translate(Matrix3x3 &, float, float);
void Flip(Matrix3x3 &, int, int);
void Shear(Matrix3x3 &, float, float);
void Perspective(Matrix3x3 &, float, float);
void process_input(Matrix3x3 &);
