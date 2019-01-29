#include <stddef.h>
#include <stdio.h>

struct mystruct {
  char *first;
  char *second;
};

struct ints {
  int a;
  int b;
};

struct perf {
	int a;
	long tim;
	unsigned b : 1;
	unsigned c : 1;
	short d;
	char e;
	int big[1000];
	long james;
	double cole;
	long jordan;
};

int main(void) {
  char *first = "I'm the first string!";
  char *second = "And I'm the second string!";
  const char *FMT = "%s\n%s\n";
  struct mystruct m;
  m.first = first;
  m.second = second;

  struct perf p;

  // There shouldn't be any padding for this structure here,
  // so it should be the width of two (char*) pointers == 16 bytes
  //
  // Assuming the fields were REVERSED, then the field located
  // at the structure's address should be technically the 2nd field
  // and the last field should be what used to be the FIRST field.
  printf(FMT, *(&m), *(&m + sizeof(char *)));

  struct ints i;
  i.a = 100;
  i.b = 999;
  printf("%d\n%d\n", *((int *)&i), *((int *)&i + 1));
  return 0;
}
