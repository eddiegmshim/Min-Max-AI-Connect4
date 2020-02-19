
// C program for generating a 
// random number in a given range. 
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
  
  
// Driver code 
int main() 
{ 
    const unsigned int width = 7;
  
    srand(time(0)); 
  
    for(int i = 0 ; i < 100; i++)
    {
        int rand_col = (rand() % (width));
        printf("test:rand_col %d\n", rand_col);
    }
  
    return 0; 
} 

