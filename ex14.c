#include <stdio.h>
#include <string.h>
#include <ctype.h>

int can_print_it(char character);
void print_letters(char letters[], int length);

void print_arguments(int number_of_arguments, char *argument_values[])
{
  int argument_index = 1;
  for(;argument_index < number_of_arguments; argument_index++)
  {
    print_letters(argument_values[argument_index], strlen(argument_values[argument_index]));
  }
}

void print_letters(char letters[], int length)
{
  int letter_index;
  char letter;
  for(letter_index = 0; letter_index < length; letter_index++)
  {
    letter = letters[letter_index];

    if(can_print_it(letter)){
      printf("'%c' == '%d'", letter, letter);
    }
  }
}

int can_print_it(char character)
{
  return isalpha(character) || isblank(character);
}

int main(int number_of_arguments, char *argument_values[])
{
  print_arguments(number_of_arguments, argument_values);
  printf("\n");
  return 0;
}
