/*Alan Lo
 *EECS 101
 *1/26/2018
 * */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define ROWS		100
#define COLUMNS		100

int main( int argc, char **argv )
{

	int			i,j, n;
	FILE			*fp;
	float			u, var;
	unsigned char	image[ROWS][COLUMNS];
  	char			*ifile, *ofile, ch;
	char			filename[4][50];

	strcpy(filename[0], "image1.raw");
	strcpy(filename[1], "image2.raw");
	strcpy(filename[2], "image3.raw");
	strcpy(filename[3], "image4.raw");



	for(n=0; n<4; n++)
	{
		ifile = filename[n];



    if (( fp = fopen( ifile, "rb" )) == NULL )
	{
	  fprintf( stderr, "error: couldn't open %s\n", ifile );
	  exit( 1 );
	}

	for ( i = 0; i < ROWS ; i++ )
	  if ( fread( image[i], 1, COLUMNS, fp ) != COLUMNS )
	  {
	    fprintf( stderr, "error: couldn't read enough stuff\n" );
	    exit( 1 );
	  }

	fclose( fp );



	//Calculate Mean for each image here
	for(i=0; i< ROWS; i++)
	{
		for(j=0; j<COLUMNS; j++)
		{
			u += image[i][j];
		}
	}
	u /= pow(ROWS,2);

	for(i=0; i< ROWS; i++)
		{
			for(j=0; j<COLUMNS; j++)
			{
				var += pow(image[i][j] - u,2);
			}
		}
	var = var/(pow(ROWS,2)-1);

	//Calculate Variance for each image here

	//Print mean and variance for each image
	printf("%s: Mean: %f Variance: %f\n", ifile, u, var);

}

	printf("Press any key to exit: ");
	gets( &ch );

	return 0;
}



