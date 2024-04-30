/*******************************************
 * Solutions for the CS:APP Performance Lab
 ********************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following student struct 
 */
student_t student = {
  "Daniel Kopta",     /* Full name */
  "no_one@nowhere.edu",  /* Email address */
};

/***************
 * COMPLEX KERNEL
 ***************/

/******************************************************
 * Your different versions of the complex kernel go here
 ******************************************************/

/* 
 * naive_complex - The naive baseline version of complex 
 */
char naive_complex_descr[] = "naive_complex: Naive baseline implementation";
void naive_complex(int dim, pixel *src, pixel *dest)
{
  int i, j;

  for(i = 0; i < dim; i++)
    for(j = 0; j < dim; j++)
    {

      dest[RIDX(dim - j - 1, dim - i - 1, dim)].red = ((int)src[RIDX(i, j, dim)].red +
						      (int)src[RIDX(i, j, dim)].green +
						      (int)src[RIDX(i, j, dim)].blue) / 3;
      
      dest[RIDX(dim - j - 1, dim - i - 1, dim)].green = ((int)src[RIDX(i, j, dim)].red +
							(int)src[RIDX(i, j, dim)].green +
							(int)src[RIDX(i, j, dim)].blue) / 3;
      
      dest[RIDX(dim - j - 1, dim - i - 1, dim)].blue = ((int)src[RIDX(i, j, dim)].red +
						       (int)src[RIDX(i, j, dim)].green +
						       (int)src[RIDX(i, j, dim)].blue) / 3;

    }
}





/* 
 * better_complex - First improvement to complex
 *                  See comments in code
 *                  Achieves 1.2 - 1.3 speedup
 */
char better_complex_descr[] = "better_complex";
void better_complex(int dim, pixel *src, pixel *dest)
{
  int i, j;
  for(i = 0; i < dim; i++)
    for(j = 0; j < dim; j++)
    {

      // Compute this once, then assign it to each pixel.
      // Compiler can't do this for us due to aliasing between src and dest.
      short avg =  ((int)src[RIDX(i, j, dim)].red +
		    (int)src[RIDX(i, j, dim)].green +
		    (int)src[RIDX(i, j, dim)].blue) / 3;

      dest[RIDX(dim - j - 1, dim - i - 1, dim)].red = avg;
      
      dest[RIDX(dim - j - 1, dim - i - 1, dim)].green = avg;
      
      dest[RIDX(dim - j - 1, dim - i - 1, dim)].blue = avg;
      
    }
}


/* 
 * better_complex8 - Better complex plus 8x8 tiling
 *                   Achieves 1.7 speedup
 */
char better_complex_descr8[] = "better_complex8x8";
void better_complex8(int dim, pixel *src, pixel *dest)
{
  int i, j, ii, jj;

  const int block = 8;

  // perform the transformation in 8x8 blocks
  // this gives us good access patterns on both reads and writes
  for(i = 0; i < dim; i+=block)
    for(j = 0; j < dim; j+=block)
      for(ii = i; ii < i + block; ii++)
	for(jj = j; jj < j + block; jj++)
    {

      short avg =  ((int)src[RIDX(ii, jj, dim)].red +
		    (int)src[RIDX(ii, jj, dim)].green +
		    (int)src[RIDX(ii, jj, dim)].blue) / 3;

      dest[RIDX(dim - jj - 1, dim - ii - 1, dim)].red = avg;
      
      dest[RIDX(dim - jj - 1, dim - ii - 1, dim)].green = avg;
      
      dest[RIDX(dim - jj - 1, dim - ii - 1, dim)].blue = avg;
      
    }
}


/* 
 * best_complex - better_complex8 plus rotating the access patterns, and unrolling by 2
 *                Achieves 2.1 - 2.2 speedup
 */
char best_complex_descr[] = "best_complex";
void best_complex(int dim, pixel *src, pixel *dest)
{

  int i, j, ii, jj;

  const int block = 8;

  // Rotate the access patterns so we read up a col, and write across a row
  // This helps quite a bit, improving over better_complex8 from 1.7 to roughly 2.0
  /*

    src       dst
    .......   .......
    .^    .   .     .
    .|    .   .     .
    .|    .   .---> .
    .......   .......
    read      write

   */

  for(i = 0; i < dim; i+=block)
    for(j = 0; j < dim; j+=block)
      for(jj = j; jj < j + block; jj++)
        // unroll by factor of 2. Seems to help a tiny bit
	for(ii = i; ii < i + block; ii+=2) 
	{ 
	  short avg =  ((int)src[RIDX(dim - ii - 1, jj, dim)].red +
			(int)src[RIDX(dim - ii - 1, jj, dim)].green +
			(int)src[RIDX(dim - ii - 1, jj, dim)].blue) / 3;
	  dest[RIDX(dim - jj - 1, ii, dim)].red = avg;	  
	  dest[RIDX(dim - jj - 1, ii, dim)].green = avg;
	  dest[RIDX(dim - jj - 1, ii, dim)].blue = avg;
	  
	  
	  avg =  ((int)src[RIDX(dim - (ii + 1) - 1, jj, dim)].red +
		  (int)src[RIDX(dim - (ii + 1) - 1, jj, dim)].green +
		  (int)src[RIDX(dim - (ii + 1) - 1, jj, dim)].blue) / 3;
	  dest[RIDX(dim - jj - 1, (ii + 1), dim)].red = avg;	  
	  dest[RIDX(dim - jj - 1, (ii + 1), dim)].green = avg;
	  dest[RIDX(dim - jj - 1, (ii + 1), dim)].blue = avg;	  
	}
}


/* 
 * complex - Your current working version of complex
 * IMPORTANT: This is the version you will be graded on
 */
char complex_descr[] = "complex: Current working version";
void complex(int dim, pixel *src, pixel *dest)
{
  best_complex(dim, src, dest);
}

/*********************************************************************
 * register_complex_functions - Register all of your different versions
 *     of the complex kernel with the driver by calling the
 *     add_complex_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_complex_functions() {
  add_complex_function(&complex, complex_descr);
  add_complex_function(&naive_complex, naive_complex_descr);
  add_complex_function(&better_complex, better_complex_descr);
  add_complex_function(&better_complex8, better_complex_descr8);
  add_complex_function(&best_complex, best_complex_descr);
}


/***************
 * MOTION KERNEL
 **************/

/***************************************************************
 * Various helper functions for the motion kernel
 * You may modify these or add new ones any way you like.
 **************************************************************/


/* 
 * weighted_combo - Returns new pixel value at (i,j) 
 */
static pixel weighted_combo(int dim, int i, int j, pixel *src) 
{
  int ii, jj;
  pixel current_pixel;

  int red, green, blue;
  red = green = blue = 0;

  int num_neighbors = 0;
  for(ii=0; ii < 3; ii++)
    for(jj=0; jj < 3; jj++) 
      if ((i + ii < dim) && (j + jj < dim)) 
      {
	num_neighbors++;
	red += (int) src[RIDX(i+ii,j+jj,dim)].red;
	green += (int) src[RIDX(i+ii,j+jj,dim)].green;
	blue += (int) src[RIDX(i+ii,j+jj,dim)].blue;
      }
  
  current_pixel.red = (unsigned short) (red / num_neighbors);
  current_pixel.green = (unsigned short) (green / num_neighbors);
  current_pixel.blue = (unsigned short) (blue / num_neighbors);
  
  return current_pixel;
}



/* 
 * mid_weighted_combo - Returns new pixel value at (i,j) 
 * assumes i and j are <= dim - 2
 */
static pixel mid_weighted_combo(int dim, int i, int j, pixel *src) 
{
  int ii;
  pixel current_pixel;

  int red, green, blue;
  red = green = blue = 0;

 
  for(ii=0; ii < 3; ii++)
    {

      // unroll the inner loop (x3)
      red += (int)src[RIDX(i+ii,j,dim)].red;
      green += (int)src[RIDX(i+ii,j,dim)].green;
      blue += (int)src[RIDX(i+ii,j,dim)].blue;

      red += (int)src[RIDX(i+ii,j+1,dim)].red;
      green += (int)src[RIDX(i+ii,j+1,dim)].green;
      blue += (int)src[RIDX(i+ii,j+1,dim)].blue;

      red += (int)src[RIDX(i+ii,j+2,dim)].red;
      green += (int)src[RIDX(i+ii,j+2,dim)].green;
      blue += (int)src[RIDX(i+ii,j+2,dim)].blue;

    }
  
  current_pixel.red = (unsigned short) (red / 9);
  current_pixel.green = (unsigned short) (green / 9);
  current_pixel.blue = (unsigned short) (blue / 9);
  
  return current_pixel;
}


/* 
 * penultimate_row_combo - Returns new pixel value at (dim-2, j) 
 * in other words, weighted_combo for the second to last row
 * assumes j < dim - 2
 */
static pixel penultimate_row_combo(int dim, int j, pixel *src) 
{
  int ii;
  pixel current_pixel;

  int red, green, blue;
  red = green = blue = 0;
  
  // Loop over second to last, and last rows
  for(ii = dim-2; ii < dim; ii++)
    {
      // unroll the inner loop (x3)
      red += (int) src[RIDX(ii, j, dim)].red;
      green += (int) src[RIDX(ii, j, dim)].green;
      blue += (int) src[RIDX(ii, j, dim)].blue;

      red += (int) src[RIDX(ii, j+1, dim)].red;
      green += (int) src[RIDX(ii, j+1, dim)].green;
      blue += (int) src[RIDX(ii, j+1, dim)].blue;

      red += (int) src[RIDX(ii, j+2, dim)].red;
      green += (int) src[RIDX(ii, j+2, dim)].green;
      blue += (int) src[RIDX(ii, j+2, dim)].blue;
    }
  
  current_pixel.red = (unsigned short) (red / 6);
  current_pixel.green = (unsigned short) (green / 6);
  current_pixel.blue = (unsigned short) (blue / 6);
  
  return current_pixel;
}

/* 
 * last_row_combo - Returns new pixel value at (dim-1, j)
 * assumes j < dim - 2
 */
static pixel last_row_combo(int dim, int j, pixel *src) 
{
  pixel current_pixel;

  int red, green, blue;
  red = green = blue = 0;
  
  // unrolled loop over 3 neighboring pixels
  red += (int) src[RIDX(dim-1, j, dim)].red;
  green += (int) src[RIDX(dim-1, j, dim)].green;
  blue += (int) src[RIDX(dim-1, j, dim)].blue;
  
  red += (int) src[RIDX(dim-1, j+1, dim)].red;
  green += (int) src[RIDX(dim-1, j+1, dim)].green;
  blue += (int) src[RIDX(dim-1, j+1, dim)].blue;
  
  red += (int) src[RIDX(dim-1, j+2, dim)].red;
  green += (int) src[RIDX(dim-1, j+2, dim)].green;
  blue += (int) src[RIDX(dim-1, j+2, dim)].blue;

  
  current_pixel.red = (unsigned short) (red / 3);
  current_pixel.green = (unsigned short) (green / 3);
  current_pixel.blue = (unsigned short) (blue / 3);
  
  return current_pixel;
}



/* 
 * penultimate_col_combo - Returns new pixel value at (i, dim-2) 
 * in other words, weighted_combo for the second to last column
 * assumes i < dim - 2
 */
static pixel penultimate_col_combo(int dim, int i, pixel *src) 
{
  int ii;
  pixel current_pixel;

  int red, green, blue;
  red = green = blue = 0;
  
  for(ii = 0; ii < 3; ii++)
    {
      // unroll the inner loop (x2)
      red += (int) src[RIDX(i+ii, dim-2, dim)].red;
      green += (int) src[RIDX(i+ii, dim-2, dim)].green;
      blue += (int) src[RIDX(i+ii, dim-2, dim)].blue;

      red += (int) src[RIDX(i+ii, dim-1, dim)].red;
      green += (int) src[RIDX(i+ii, dim-1, dim)].green;
      blue += (int) src[RIDX(i+ii, dim-1, dim)].blue;
    }
  
  current_pixel.red = (unsigned short) (red / 6);
  current_pixel.green = (unsigned short) (green / 6);
  current_pixel.blue = (unsigned short) (blue / 6);
  
  return current_pixel;
}


/* 
 * last_col_combo - Returns new pixel value at (i, dim-1) 
 * assumes i < dim - 2
 */
static pixel last_col_combo(int dim, int i, pixel *src) 
{
  int ii;
  pixel current_pixel;

  int red, green, blue;
  red = green = blue = 0;
  
  for(ii = 0; ii < 3; ii++)
  {
    red += (int) src[RIDX(i+ii, dim-1, dim)].red;
    green += (int) src[RIDX(i+ii, dim-1, dim)].green;
    blue += (int) src[RIDX(i+ii, dim-1, dim)].blue;
  }
  
  current_pixel.red = (unsigned short) (red / 3);
  current_pixel.green = (unsigned short) (green / 3);
  current_pixel.blue = (unsigned short) (blue / 3);
  
  return current_pixel;
}



/******************************************************
 * Your different versions of the motion kernel go here
 ******************************************************/


/*
 * naive_motion - The naive baseline version of motion 
 */
char naive_motion_descr[] = "naive_motion: Naive baseline implementation";
void naive_motion(int dim, pixel *src, pixel *dst) 
{
  int i, j;
    
  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[RIDX(i, j, dim)] = weighted_combo(dim, i, j, src);
}




/*
 * better_motion - remove if statements from inner loop by specializing 
 *                 for certain regions of the image. Removes boundary checks.
 *                 Achieves 2.9 - 3.0 speedup
 */
char better_motion_descr[] = "better_motion: Better baseline implementation";
void better_motion(int dim, pixel *src, pixel *dst) 
{
  int i, j;

  
  // top-left / middle block
  // handle everything but the last two rows and cols
  for (i = 0; i < dim-2; i++)
    for (j = 0; j < dim-2; j++)
      dst[RIDX(i, j, dim)] = mid_weighted_combo(dim, i, j, src);


  // second to last row
  for (j = 0; j < dim-2; j++)
    dst[RIDX(dim-2, j, dim)] = penultimate_row_combo(dim, j, src);
    
  // last row
  for (j = 0; j < dim-2; j++)
    dst[RIDX(dim-1, j, dim)] = last_row_combo(dim, j, src);

  // second to last column
  for (i = 0; i < dim-2; i++)
    dst[RIDX(i, dim-2, dim)] = penultimate_col_combo(dim, i, src);

  // last column
  for (i = 0; i < dim-2; i++)
    dst[RIDX(i, dim-1, dim)] = last_col_combo(dim, i, src);
  
 
  // last four corner pixels

  // dim-2, dim-2 -------------------------------------------------------------------
  // red
  dst[RIDX(dim-2, dim-2, dim)].red = ( (int)src[RIDX(dim-2, dim-2, dim)].red +
				       (int)src[RIDX(dim-2, dim-1, dim)].red +
				       (int)src[RIDX(dim-1, dim-2, dim)].red +
				       (int)src[RIDX(dim-1, dim-1, dim)].red ) / 4;
  // green
  dst[RIDX(dim-2, dim-2, dim)].green = ( (int)src[RIDX(dim-2, dim-2, dim)].green +
					 (int)src[RIDX(dim-2, dim-1, dim)].green +
					 (int)src[RIDX(dim-1, dim-2, dim)].green +
					 (int)src[RIDX(dim-1, dim-1, dim)].green ) / 4;
  // blue
  dst[RIDX(dim-2, dim-2, dim)].blue = ( (int)src[RIDX(dim-2, dim-2, dim)].blue +
					(int)src[RIDX(dim-2, dim-1, dim)].blue +
					(int)src[RIDX(dim-1, dim-2, dim)].blue +
					(int)src[RIDX(dim-1, dim-1, dim)].blue ) / 4;

  // dim-2, dim-1 -------------------------------------------------------------------
  // red
  dst[RIDX(dim-2, dim-1, dim)].red = ( (int)src[RIDX(dim-2, dim-1, dim)].red +
				       (int)src[RIDX(dim-1, dim-1, dim)].red ) / 2;
  // green 
  dst[RIDX(dim-2, dim-1, dim)].green = ( (int)src[RIDX(dim-2, dim-1, dim)].green +
				       (int)src[RIDX(dim-1, dim-1, dim)].green ) / 2;
  // blue
  dst[RIDX(dim-2, dim-1, dim)].blue = ( (int)src[RIDX(dim-2, dim-1, dim)].blue +
				       (int)src[RIDX(dim-1, dim-1, dim)].blue ) / 2;
  
  // dim-1, dim-2 -------------------------------------------------------------------
  // red
  dst[RIDX(dim-1, dim-2, dim)].red = ( (int)src[RIDX(dim-1, dim-2, dim)].red +
				       (int)src[RIDX(dim-1, dim-1, dim)].red ) / 2;
  // green
  dst[RIDX(dim-1, dim-2, dim)].green = ( (int)src[RIDX(dim-1, dim-2, dim)].green +
				       (int)src[RIDX(dim-1, dim-1, dim)].green ) / 2;
  // blue
  dst[RIDX(dim-1, dim-2, dim)].blue = ( (int)src[RIDX(dim-1, dim-2, dim)].blue +
				       (int)src[RIDX(dim-1, dim-1, dim)].blue ) / 2;

  //dim-1, dim-1 -------------------------------------------------------------------
  dst[RIDX(dim-1, dim-1, dim)] = src[RIDX(dim-1, dim-1, dim)];

}



/********************************************
Helper functions for best_motion
*********************************************/

typedef struct pixel_sum{
  int r, g, b;
}pixel_sum;


/*
 * Returns the average of a 3 pixel sub-column with the top at i, j
 * IMPORTANT: If the code gets too large, gcc will stop inlining, and performance will suffer.
 * Some students will run in to this problem if they keep around too much old code.
 * This can be very hard to catch. Changes to seemingly unrelated parts of code can slow everything down.
 */
 __attribute__((always_inline)) static pixel_sum col_avg(int dim, int i, int j, pixel* src)
{
  pixel_sum retval;
  retval.r   = ((int)src[RIDX(i,j,dim)].red   + (int)src[RIDX(i+1,j,dim)].red   + (int)src[RIDX(i+2,j,dim)].red);
  retval.g = ((int)src[RIDX(i,j,dim)].green + (int)src[RIDX(i+1,j,dim)].green + (int)src[RIDX(i+2,j,dim)].green);
  retval.b  = ((int)src[RIDX(i,j,dim)].blue  + (int)src[RIDX(i+1,j,dim)].blue  + (int)src[RIDX(i+2,j,dim)].blue);
  return retval;
}



/*
 * best_motion - Compute and save partial averages for each 3 pixel sub-column
 *               in the 3x3 window, to avoid summing 9 pixels every time.
 *               Only sum 3 pixels, and reuse the other two partial averages.
 *               Achieves 4.4 speedup
 */
char best_motion_descr[] = "best_motion";
void best_motion(int dim, pixel *src, pixel *dst) 
{
  int i, j;

  // Keep track of 3 partial averages
  pixel_sum a1, a2, a3;
  
  
  // top-left / middle block
  // handle everything but the last two rows and cols
  for (i = 0; i < dim-2; i++)
  {
    // calculate first two columns of new row
    a1 = col_avg(dim, i, 0, src);
    a2 = col_avg(dim, i, 1, src);
    for (j = 0; j < dim-2; j++)
    {
      // add in third column
      a3 = col_avg(dim, i, j + 2, src);
      
      dst[RIDX(i, j, dim)].red = (a1.r + a2.r + a3.r) * (1.0f / 9.0f);
      dst[RIDX(i, j, dim)].green = (a1.g + a2.g + a3.g) * (1.0f / 9.0f);
      dst[RIDX(i, j, dim)].blue = (a1.b + a2.b + a3.b) * (1.0f / 9.0f);

      // move sliding window
      a1 = a2;
      a2 = a3;      
    }
  }

  // second to last row
  for (j = 0; j < dim-2; j++)
    dst[RIDX(dim-2, j, dim)] = penultimate_row_combo(dim, j, src);
    
  // last row
  for (j = 0; j < dim-2; j++)
    dst[RIDX(dim-1, j, dim)] = last_row_combo(dim, j, src);

  // second to last column
  for (i = 0; i < dim-2; i++)
    dst[RIDX(i, dim-2, dim)] = penultimate_col_combo(dim, i, src);

  // last column
  for (i = 0; i < dim-2; i++)
    dst[RIDX(i, dim-1, dim)] = last_col_combo(dim, i, src);
  
 
  // last four corner pixels

  // dim-2, dim-2 -------------------------------------------------------------------
  // red
  dst[RIDX(dim-2, dim-2, dim)].red = ( (int)src[RIDX(dim-2, dim-2, dim)].red +
				       (int)src[RIDX(dim-2, dim-1, dim)].red +
				       (int)src[RIDX(dim-1, dim-2, dim)].red +
				       (int)src[RIDX(dim-1, dim-1, dim)].red ) * 0.25f;
  // green
  dst[RIDX(dim-2, dim-2, dim)].green = ( (int)src[RIDX(dim-2, dim-2, dim)].green +
					 (int)src[RIDX(dim-2, dim-1, dim)].green +
					 (int)src[RIDX(dim-1, dim-2, dim)].green +
					 (int)src[RIDX(dim-1, dim-1, dim)].green ) * 0.25f;
  // blue
  dst[RIDX(dim-2, dim-2, dim)].blue = ( (int)src[RIDX(dim-2, dim-2, dim)].blue +
					(int)src[RIDX(dim-2, dim-1, dim)].blue +
					(int)src[RIDX(dim-1, dim-2, dim)].blue +
					(int)src[RIDX(dim-1, dim-1, dim)].blue ) * 0.25f;

  // dim-2, dim-1 -------------------------------------------------------------------
  // red
  dst[RIDX(dim-2, dim-1, dim)].red = ( (int)src[RIDX(dim-2, dim-1, dim)].red +
				       (int)src[RIDX(dim-1, dim-1, dim)].red ) * 0.5f;
  // green 
  dst[RIDX(dim-2, dim-1, dim)].green = ( (int)src[RIDX(dim-2, dim-1, dim)].green +
				       (int)src[RIDX(dim-1, dim-1, dim)].green ) * 0.5f;
  // blue
  dst[RIDX(dim-2, dim-1, dim)].blue = ( (int)src[RIDX(dim-2, dim-1, dim)].blue +
				       (int)src[RIDX(dim-1, dim-1, dim)].blue ) * 0.5f;
  
  // dim-1, dim-2 -------------------------------------------------------------------
  // red
  dst[RIDX(dim-1, dim-2, dim)].red = ( (int)src[RIDX(dim-1, dim-2, dim)].red +
				       (int)src[RIDX(dim-1, dim-1, dim)].red ) * 0.5f;
  // green
  dst[RIDX(dim-1, dim-2, dim)].green = ( (int)src[RIDX(dim-1, dim-2, dim)].green +
				       (int)src[RIDX(dim-1, dim-1, dim)].green ) * 0.5f;
  // blue
  dst[RIDX(dim-1, dim-2, dim)].blue = ( (int)src[RIDX(dim-1, dim-2, dim)].blue +
				       (int)src[RIDX(dim-1, dim-1, dim)].blue ) * 0.5f;

  //dim-1, dim-1 -------------------------------------------------------------------
  dst[RIDX(dim-1, dim-1, dim)] = src[RIDX(dim-1, dim-1, dim)];

}





/*
 * motion - Your current working version of motion. 
 * IMPORTANT: This is the version you will be graded on
 */
char motion_descr[] = "motion: Current working version";
void motion(int dim, pixel *src, pixel *dst) 
{
  best_motion(dim, src, dst);
}

/********************************************************************* 
 * register_motion_functions - Register all of your different versions
 *     of the motion kernel with the driver by calling the
 *     add_motion_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_motion_functions() {
  add_motion_function(&motion, motion_descr);
  add_motion_function(&naive_motion, naive_motion_descr);
  add_motion_function(&better_motion, better_motion_descr);
  add_motion_function(&best_motion, best_motion_descr);
}
