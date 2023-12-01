#include <iostream>
#include <stdio.h>

using namespace std;

int main(){
//declaramos un puntero x = 0xf1a35ffd58
int* address = (int *) 0xdcd93ff918;

 printf("address1 = %p\n", address);
*address = 0xdead;
printf("address = %p\n", *address);
int x = *address;
printf("x = %d\n", x);
}