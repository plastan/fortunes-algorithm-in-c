#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "main.h"




float test_m(Point *p1, Point *p2){
       return  -(float)(p1->x - p2->y)/(p1->y-p2->y);
}

float test_q(Point *p1,Point * p2){ 

    return (0.5 *(float) (p1->x * p1->x - p2->x * p2->x  + p1->y * p1->y - p2->y * p2->y)) / (float)(p1->y - p2->y);
}

void main(){


	// testing m
	Point *	m_a = newPoint(11,200);
	Point *	m_b = newPoint(200,30);
	float result = test_m(m_a,m_b);	
	
	printf("test m : %f",result) ;
	return;
	// testing q

}

