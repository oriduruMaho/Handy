#include <handy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int random;

void d100(void){
    srand(time(NULL));
    random = rand() % 100;
    printf("%d",random);
}

void d10(void){
    srand(time(NULL));
    random = rand() % 10;
    printf("%d",random);
}

int main() {

    
    return 0;
}
