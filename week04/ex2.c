#include <stdio.h>
#include <math.h>
#include <stdlib.h>

struct Point{
	double x;
	double y;
};

double distance(struct Point point1, struct Point point2){
	double dist_x = point1.x - point2.x;
	double dist_y = point1.y - point2.y;
	return sqrt(pow(dist_x, 2) + pow(dist_y, 2));
}

double area(struct Point a, struct Point b, struct Point c){
	double x1y2 = a.x*b.y;
	double x2y1 = b.x*a.y;
	double x2y3 = b.x*c.y;
	double x3y2 = c.x*b.y;
	double x3y1 = c.x*a.y;
	double x1y3 = a.x*c.y;
	return abs(x1y2-x2y1+x2y3-x3y2+x3y1-x1y3)/2;
}

int main(){
	struct Point a = {2.5, 6};
	struct Point b = {1, 2.2};
	struct Point c = {10, 6};
	printf("%f\n", distance(a, b));
	printf("%f\n", area(a, b, c));
	return 0;
}
