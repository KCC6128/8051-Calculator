#include "delay.h"

void delay(unsigned int ms) { //delay
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 50; j++);
}