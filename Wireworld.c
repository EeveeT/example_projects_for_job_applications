
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>

#define WIDTH (40)
#define HEIGHT (40)

enum State {empty, head, tail, copper};
typedef enum State State;
/*can now just call enum State, State */

void iteration(State world[HEIGHT][WIDTH], State new_world[HEIGHT][WIDTH]);
State the_rule(State world[HEIGHT][WIDTH], int y, int x);
int neighbouring_heads(State world[HEIGHT][WIDTH], int current_y, int current_x);
void set_up_world(FILE *file, State world[HEIGHT][WIDTH]);
void using_iterations(State world[HEIGHT][WIDTH], State new_world[HEIGHT][WIDTH]);
void test(void);


int main(int argc, char**argv)
{
  FILE *file;
  State world[HEIGHT][WIDTH];
  State new_world[HEIGHT][WIDTH];

  if(argc < 2)
  {
    fprintf(stderr, "Incorrect usage\n");
    fprintf(stderr, "Testing: %s --test\n", argv[0]);
    fprintf(stderr, "Running: %s [filename]\n", argv[0]);
    return -1;
  }

  if(strcmp(argv[1], "--test") == 0)
  {
    test();
    return 0;
  }
/* argv[1] contains name of file*/
  file = fopen(argv[1], "r");
  if(file == NULL)
  {
    fprintf(stderr, "Error: File incorrect \n");
    return -1;
  }

  set_up_world(file, world);
  fclose(file);
  using_iterations(world, new_world);

  return 0;
}

void iteration (State world[HEIGHT][WIDTH], State new_world[HEIGHT][WIDTH])
{
  /*As we have enumerated State, that is the type
    like int or char hence it being green in Atom'*/
    /*world[H][W] is the "whole world", all the cells in our world */
    int y;
    int x;

    for(y=0; y < HEIGHT; y++)
    {
      for(x=0; x < WIDTH; x++)
      {
        State new_state = the_rule(world, y, x);
        new_world[y][x] = new_state;
      }
    }
}

State the_rule(State world[HEIGHT][WIDTH], int y, int x)
{
  switch (world[y][x])
  {
    case empty:
      return empty;
      break;

    case head:
      return tail;
      break;

    case tail:
      return copper;
      break;

    case copper:
    {
      int n = neighbouring_heads(world, y, x);
      if(n==1 || n==2)
      {
        return head;
      }
      else
      {
        return copper;
      }
      break;
    }
    default:
      fprintf(stderr, "Computer says no\n");
      exit(-1);
  }
}
/*DO NOT CALL BELOW FUNCTION IF CURRENT CELL IS A HEAD
  (which shouldn't occur as "current" cell ought to be
   copper if you're looking for head_count ) */
int neighbouring_heads(State world[HEIGHT][WIDTH], int current_y, int current_x)
{
  /*neighbouring_heads is, of all the cells in the world, "this" is the current one we're in */

  /* need to find the difference in
    relation to current cell*/
  int head_count = 0;

  int y_offset;
  int x_offset;
  for(y_offset = -1; y_offset <= 1; y_offset++)
  {
    for(x_offset = -1; x_offset <= 1; x_offset++)
    {
      int y = current_y + y_offset;
      int x = current_x + x_offset;
      if(y < 0)
      /*if y are gone over the top of the edge/is less than minimum*/
      {
        y = HEIGHT - 1;
      }

      if(y > HEIGHT-1)
      /*if it has gone past the bottom/ it's maximum
        HEIGHT*/
      {
        y = 0;
      }

      if(x < 0)
      {
        x = WIDTH -  1;
      }
      if(x > WIDTH - 1)
      {
        x = 0;
      }
      if(world[y][x] == head)
      {
        head_count++;
      }
    }
  }
  return head_count;
}

void set_up_world(FILE *file, State world[HEIGHT][WIDTH])
{
  /*set every cell in the world*/
  int y;
  int x;
  for(y=0; y < HEIGHT; y++)
  {
    for(x=0; x < WIDTH; x++)
    {
      try_again:
      switch(fgetc(file))
      {
        case EOF:
          fprintf(stderr, "Unexpected end of file \n");
          exit (-1);

        case ' ':
          world[y][x] = empty;
          break;

        case 'H':
          world[y][x] = head;
          break;

        case 't':
          world[y][x] = tail;
          break;

        case 'c':
         world[y][x] = copper;
         break;

        case '\n':
          goto try_again;

       default:
        fprintf(stderr, "Oups, something went wrong, \n"
                        "you used arandom character \n"
                        "which I didn't like \n");
        fclose(file);
        exit (-1);
      }
    }
  }
}


void using_iterations(State world[HEIGHT][WIDTH], State new_world[HEIGHT][WIDTH])
{
  void *temp;
  int i, x, y;

  for(i = 0; i < 1000; i++)
  {
    iteration(world, new_world);

    temp = world;
    world = new_world;
    new_world = temp;

    for(y = 0; y < HEIGHT; y++)
    {
      for(x = 0; x < WIDTH; x++)
      {
        printf("%d",(int)world[y][x]);
      }
      printf("\n");
    }
    printf("\n");
  }
}

void test(void)
{
  int test_count, test_wrap_count, test_zero_count,
      test_wrap_right, test_wrap_bottom;

  State test_empty_empty, test_head_tail, test_tail_copper,
        test_copper_head, test_copper_copper;

  State test_world[HEIGHT][WIDTH] = {
    { empty,  empty,   head,  empty},
    {  tail,  empty, copper,  empty},
    {  head, copper, empty},
    {empty}
  };

    test_world[HEIGHT-1][WIDTH-1] = head;
    test_world[9][WIDTH-1] = head;
    test_world[10][WIDTH-2] = head;
    test_world[9][0] = copper;
    test_world[11][0] = head;
    test_world[HEIGHT-1][30] = head;
    test_world[HEIGHT-2][28] = tail;
    test_world[0][30] = head;
    test_world[HEIGHT-1][29] = copper;
    test_world[10][WIDTH-1] = copper;

  test_count = neighbouring_heads(test_world, 1, 1);
  assert(test_count == 2);
  printf("Passed test 1: Neighbouring heads\n");

  test_wrap_count = neighbouring_heads(test_world, 0, 0);
  assert(test_wrap_count == 1);
  printf("Passed test 2: Wrap count\n");

  test_zero_count = neighbouring_heads(test_world, 15, 27);
  assert(test_zero_count == 0);
  printf("Passed test 3: Zero heads\n");

  test_wrap_right = neighbouring_heads(test_world, 10, WIDTH-1);
  assert(test_wrap_right == 3);
  printf("Passed test 4: Three heads and a wrap\n");

  test_wrap_bottom = neighbouring_heads(test_world, HEIGHT-1, 29);
  assert(test_wrap_bottom == 2);
  printf("Passed test 5: Two heads near the bottom\n");

  test_empty_empty = the_rule(test_world, 1, 1);
  assert(test_empty_empty == empty);
  printf("Passed test 6: Test empty to empty\n");

  test_head_tail = the_rule(test_world, 0, 2);
  assert(test_head_tail == tail);
  printf("Passed test 7: Test head to tail\n");

  test_tail_copper = the_rule(test_world, 1, 0);
  assert(test_tail_copper == copper);
  printf("Passed test 8: Test tail to copper\n");

  test_copper_head = the_rule(test_world, HEIGHT-1, 29);
  assert(test_copper_head == head);
  printf("Passed test 9: Test copper to head\n");

  test_copper_copper = the_rule(test_world, 10, WIDTH-1);
  assert(test_copper_copper == copper);
  printf("Passed test 10: Test copper to copper\n");

}
