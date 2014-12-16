========
Panorama
========

Brief Description
=================

This program reads two images, takes 4 pairs of correspondences from the user between the two image, computes and displays a panorama and also writes the panorama image using OpenImageIO and OpenGL. It works with image format that the OpenImageIO library supports such as TIFF, JPEG/JFIF, PNG, HDR/RGBE, DPX, Cineon, FITS, BMP, ICO, DDS, SGI, PNM/PPM/PGM/PBM. This program has been designed in C++ and can be compiled easily using G++ compiler. Makefile has been included in this package to make it easier to compile and build the executable. To compile, simply type 'make' on the command line to create an executable file called 'panorama'.

This tool is based on projective warps including all of the affine and perspective warps. This tool will provide for input of correspondences between the two input images, will compute the homography matrix using the 4 pairs of correspondences, and finally apply the transform to the second input image via inverse mapping and then stitch it with the first input image.

It does all of the following:

* Read two input images from two image files at a time.
* Accept 4 pairs of correspondences from the user mapping points from one input image to another.
* Compute homography matrix using the 4 pairs of correspondences.
* Transform just the corners of second input image with the forward transform.
* Make space for a new image exactly big enough to contain the four transformed/warped corners of second input image.
* Calculate the inverse transform by inverting the homography matrix.
* Transform the second input image with the inverse homography matrix, truncating non-integer pixel locations to integers.
* Make space for a new image exactly big enough to contain both the first input image as well as transformed second input image.
* Stitch the transformed image with the first input image.
* Optionally (if a third filename was specified) save the panorama in an image file.
* Display the panorama.


The command line must be of this form:
panorama inimage1 inimage2 [outimage]

If the output filename is missing, the program simply skips saving the output image. If no filenames are present, the program prints an error message and stops.

Compiling Instructions
======================

To compile, simply type 'make' on the command line and it would generate an executable called 'panorama'.

Usage
=====

./panorama input1.img input2.img [output.img]

Keyboard Features
=================

'q' or 'Q' or ESC to quit

Mouse Features
==============

Left click to select/draw a point on the window. Maximum 4 points per window can be selected. 
Order of point color: red, blue, green, yellow

Author
======

Name:            Nitin Madhok
Email:           nmadhok@clemson.edu
