#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS		(int)480
#define COLUMNS		(int)640

void clear( unsigned char image[][COLUMNS] );
void header( int row, int col, unsigned char head[32] );
void drawline(int rho, int theta);

float hough_func;
unsigned char himage[ROWS][COLUMNS];
int x,y, temp;

void drawline(int rho, int theta)
	{


		for(x=0; x<ROWS; x++)
		{
			for(y=0; y<COLUMNS; y++)
			{
				hough_func = x*cos(theta* M_PI/180) - y*sin(theta*M_PI/180);
				temp = (int) hough_func;
				if(temp == rho-800)
				{
					himage[x][y] = 255;
				}
			}
		}
	}




int main( int argc, char **argv )
{

	int				i, j, k, max[3], x, y, temp, theta,rho;
	int				sgm_threshold, hough_threshold, voting[1600][180], threshold;
	int 			a,b;
	float			pixel;
	double			p, hough_func;
	int 			Gxmax, Gymax;
	FILE			*fp;
	unsigned char	image[ROWS][COLUMNS], ximage[ROWS][COLUMNS], yimage[ROWS][COLUMNS], head[32];
	unsigned char   simage[ROWS][COLUMNS], bimage[ROWS][COLUMNS];
	char			filename[50], ifilename[50], ch;

	strcpy( filename, "image" );
	header ( ROWS, COLUMNS, head );




	/*FUNCTION*/



		clear( ximage );
		clear( yimage );

		/* Read in the image */
		strcpy( ifilename, filename );
		if (( fp = fopen( strcat(ifilename, ".raw"), "rb" )) == NULL )
		{
		  fprintf( stderr, "error: couldn't open %s\n", ifilename );
		  exit( 1 );
		}
		for ( i = 0; i < ROWS ; i++ )
		  if ( fread( image[i], sizeof(char), COLUMNS, fp ) != COLUMNS )
		  {
			fprintf( stderr, "error: couldn't read enough stuff\n" );
			exit( 1 );
		  }
		fclose( fp );

		max[0] = 0; //maximum of Gx
		max[1] = 0; //maximum of Gy
		max[2] = 0; //maximum of SGM

		/* Compute Gx, Gy, SGM, find out the maximum and normalize*/
		int xmask[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1}};
		int ymask[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};


		/* Get the max value for x*/
		for(x=1;x<ROWS-1;x++)
		{
			for(y=1; y<COLUMNS -1 ; y++)
			{
				Gxmax=0;
				for(a=-1; a<=1; a++)
				{
					for(b=-1; b<=1; b++)
					{
						Gxmax += xmask[a+1][b+1] * image[x+a][y+b];
					}
				}
				if(abs(Gxmax) > max[0])
				{
					max[0] = abs(Gxmax);
				}
			}
		}

		/* Solving for dE/dx*/

		for(x=1; x<ROWS-1; x++)
		{
			for(y=1; y<COLUMNS-1 ; y++)
			{
				pixel = 0;
				for(a=-1;a<=1;a++)
				{
					for(b=-1;b<=1 ; b++)
					{
						pixel += xmask[a+1][b+1] * image[x+a][y+b];
					}
				}
				pixel = (pixel*1.0/max[0]) * 255;
				ximage[x][y] =abs(pixel);
			}
		}

		/*Max for Gymax */
		for(x=1;x<ROWS-1;x++)
				{
					for(y=1; y<COLUMNS -1 ; y++)
					{
						Gymax=0;
						for(a=-1; a<=1; a++)
						{
							for(b=-1; b<=1; b++)
							{
								Gymax += ymask[a+1][b+1] * image[x+a][y+b];
							}
						}
						if(abs(Gymax) > max[1])
						{
							max[1] = abs(Gymax);
						}
					}
				}
		/* Solving for dE/dy*/

				for(x=1; x<ROWS-1; x++)
				{
					for(y=1; y<COLUMNS-1 ; y++)
					{
						pixel = 0;
						for(a=-1;a<=1;a++)
						{
							for(b=-1;b<=1 ; b++)
							{
								pixel += ymask[a+1][b+1] * image[x+a][y+b];
							}
						}
						pixel = (pixel*1.0/max[1]) * 255;
						yimage[x][y] =abs(pixel);
					}
				}


		/*Solving for SGM*/

		for(a=0; a<ROWS; a++)
		{
			for(b=0; b<COLUMNS; b++)
			{
				if((pow(ximage[a][b],2) + pow(yimage[a][b],2)) > max[2])
				{
					max[2] = pow(ximage[a][b],2) + pow(yimage[a][b],2);
				}
			}
		}
		for(a=0; a<ROWS; a++)
				{
					for(b=0; b<COLUMNS; b++)
					{
						simage[a][b] = ((pow(ximage[a][b],2) + pow(yimage[a][b], 2))/max[2]) * 255;
					}
				}


		/* Write SGM to a new image */
		strcpy( ifilename, filename );
		if (!( fp = fopen( strcat( ifilename, "-s.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
		for ( i = 0 ; i < ROWS ; i++ ) fwrite( simage[i], 1, COLUMNS, fp );
		fclose( fp );


		/* Compute the binary image */
		threshold =30;

		for ( a = 0 ; a < ROWS ; a++ )
				{
					for ( b = 0 ; b < COLUMNS ; b++ )
						{
							if( simage[a][b] <= threshold)
							{
								bimage[a][b]=0;
							}
							else
							{
								bimage[a][b]=255;
							}
						}
				}



		/*HOUGH TRANSFORM PROCESS   */

		for(x=0; x<ROWS; x++)
		 {
			for(y=0; y<COLUMNS ; y++)
			 {
				if(simage[x][y] > 36)
				{
					for(theta = 0; theta <180; theta++)
						{
							p = x*cos(theta* M_PI/180) - y*sin(theta*M_PI/180);
							rho = round(p);
							voting[800+rho][theta]+=1;
						}
				}

			 }
		 }

		for(rho = 0; rho <1600; rho++)
		{
			for(theta = 0; theta <180; theta++)
			{
				if(voting[rho][theta] > 280 && voting[rho][theta] < 640)
				{
					printf("p= %d  theta=%d  vote=%d\n", rho, theta, voting[rho][theta]);
					drawline(rho, theta);
				}
			}
		}







		/* Write the binary image to a new image */
		strcpy( ifilename, filename );
		if (!( fp = fopen( strcat( ifilename, "-b.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
		for ( i = 0 ; i < ROWS ; i++ ) fwrite( bimage[i], 1, COLUMNS, fp );
		fclose( fp );


		/* Write the Hough Transformed image to a new image */
			strcpy( ifilename, filename );
			if (!( fp = fopen( strcat( ifilename, "-hough.ras" ), "wb" ) ))
			{
				 fprintf( stderr, "error: could not open %s\n", ifilename );
				 exit( 1 );
			}
			fwrite( head, 4, 8, fp );
			for ( i = 0 ; i < ROWS ; i++ ) fwrite( himage[i], 1, COLUMNS, fp );
			fclose( fp );



	printf( "Press any key to exit: " );
	gets( &ch );
	return 0;
}




void clear( unsigned char image[][COLUMNS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLUMNS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */

	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch ++;
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++;
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;

	ch = (char*)&num;
	head[19] = *ch;
	ch ++;
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
	head[16] = *ch;


	/* Big-endian for unix */
	/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;
*/
}
