/*                                                                      */
/*      Erastosthenes Sieve Prime Number Program in C                   */
/*      from Byte, Sept. 1981, pg. 186                                  */
/*                                                                      */

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

        printf("__FILE__ = %s\n", __FILE__); 
        printf("__LINE__ = %d\n", __LINE__); 
        printf("__DATE__ = %s\n", __DATE__); 
        printf("__TIME__ = %s\n", __TIME__); 
#ifdef __ELLCC__
        printf("__ELLCC__ = %d\n", __ELLCC__); 
        printf("__ELLCC_MINOR__ = %d\n", __ELLCC_MINOR__); 
        printf("__ELLCC_PATCHLEVEL__ = %d\n", __ELLCC_PATCHLEVEL__); 
        printf("__ELLCC_VERSION_STRING__ = %s\n", __ELLCC_VERSION_STRING__); 
        printf("__ELLCC_VERSIONG__ = %s\n", __ELLCC_VERSION__); 
#endif
        }
               
