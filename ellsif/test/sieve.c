/*                                                                      */
/*      Erastosthenes Sieve Prime Number Program in C                   */
/*      from Byte, Sept. 1981, pg. 186                                  */
/*                                                                      */
/*
 *	To compile with any Introl-C compiler
 *	(replace XX with processor number):
 *		ccXX -g sieve.c
 *	To link:
 *		ildXX -gnXX -o sieveXX sieve.oXX -lc
 *	To run:
 *		idb -mXX -w -i sieveXX
 */

#include <stdio.h>

#define TRUE    1
#define FALSE   0
#define SIZE    8190
#define SIZEPL  8191

char    flags[SIZEPL];

int main(int ac, char **av)
        {
	int     i,prime,k,count,iter;

	printf("10000 iterations\n");
        for(iter=1; iter <= 10000; iter++)
                {
                count=0;
                for(i=0; i <= SIZE; i++)
                        flags[i]=TRUE;

                for(i=0; i <= SIZE; i++)
                        {
                        if(flags[i])
                                {
                                prime=i+i+3;
                                k=i+prime;
                                while(k <= SIZE)
                                        {
                                        flags[k]=FALSE;
                                        k+=prime;
                                        }
                                count=count+1;
                                }
                        }
                }
	printf("%d primes\n",count);
        }
               