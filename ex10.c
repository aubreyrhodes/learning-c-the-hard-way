#include <stdio.h>

int main(int argc, char *argv[])
{

  char *states[] = { "Georgia", "South Carolina", "Alabama", "Florida" };
  int number_of_states = 4;
  int i = 0;

  argv[1] = states[1];
  for(i = 1; i < argc; i++)
  {
    printf("argv[%d]: %s\n", i, argv[i]);
  }

  for(i = 0; i < number_of_states; i++)
  {
    printf("states[%d]: %s\n", i, states[i]);
  }

  return 0;
}
