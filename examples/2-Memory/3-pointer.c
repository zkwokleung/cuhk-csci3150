#include <stdlib.h>
#include <stdio.h>


int main()
{
	int a;
	int b;

	int *p;

	p = & a;

	a = 5;
 
	printf("%d, %p, %p\n", *p, p, &p);

	b = 6;

	p= &b;

	printf("%d, %p, %p\n", *p, p, &p);

	

	return 0;
}

