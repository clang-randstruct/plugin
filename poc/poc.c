#include <stdio.h>

struct mystruct {
	int *p;
	int *c;
};

int main(void)
{
	struct mystruct m = { 0, 0 };
	puts("Proof of Concept!");
	return 0;
}
