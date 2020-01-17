#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "neillsdl2.h"

#define HEIGHT (3)
#define WIDTH (3)
#define EMPTY (' ')
#define RIGHT (1)
#define LEFT (-1)
#define UP (-1)
#define DOWN (1)
#define MAX_NUM_BOARDS (181440) /* (9!)/2 = 181 440 */
#define NO_PARENT (-1)
#define RECTSIZE (120)

typedef fntrow Font[FNTCHARS][FNTHEIGHT];

typedef struct {
  char board[HEIGHT][WIDTH];
  int parent;
} Board;

typedef struct{
  int write;
  int read;
  Board array[MAX_NUM_BOARDS];
} Board_Queue;

typedef struct{
  int y;
  int x;
} Coordinate;

typedef enum{
  NoPossibleMoves,
  SolutionFound,
  KeepGoing
} Status;

bool initialise_board(char board[HEIGHT][WIDTH], char *input);
bool is_valid_char_input(char input);
Coordinate find_space(char board[HEIGHT][WIDTH]);
bool is_valid_move(Coordinate place);
void get_moves(char board[HEIGHT][WIDTH], Coordinate moves[4]);
void initialise_queue(Board_Queue* queue_pointer);
bool push_board(Board_Queue* queue_pointer, Board board);
bool pop_board(Board_Queue* queue_pointer,
               Board *board_pointer, int *board_index);
bool contains_duplicate(const Board_Queue *queue_pointer,
                        Board board);
void solve_puzzle(char board[HEIGHT][WIDTH]);
void print_list(const Board_Queue *queue_pointer, int index,
                SDL_Simplewin *sw, Font font);
Status make_children(Board_Queue *queue);
void swap_tile(char board[HEIGHT][WIDTH],
               Coordinate space, Coordinate swap_to);
void print_board(char board[HEIGHT][WIDTH], SDL_Simplewin *sw,
                 Font font);
bool same_board(const char left_board[HEIGHT][WIDTH],
                const char right_board[HEIGHT][WIDTH]);
Coordinate coordinate(int y, int x);


void test(void);

int main(int argc, char **argv)
{

  char board[HEIGHT][WIDTH];
  if(argc != 2)
  {
    fprintf(stderr, "Usage: %s [Start Configuration]\n", argv[0]);
    fprintf(stderr, "Usage: %s --test\n", argv[0]);
    return -1;
  }

  if(strcmp(argv[1], "--test") == 0)
  {
    test();
    return 0;
  }

  if(!initialise_board(board, argv[1]))
  {
    return -2;
  }

  solve_puzzle(board);

  return 0;
}

bool initialise_board(char board[HEIGHT][WIDTH], char *input)
{
  int y;
  int x;
  char ch;
  char to_use[10] = " 12345678";
  int i;

  if(strlen(input) != 9)
  {
    fprintf(stderr, "Error: Input must be nine characters long \n");
    return false;
  }

  /*account for repeated numbers i.e., 222 4555 */

  for(y=0; y < HEIGHT; y++)
  {
    for(x=0; x < WIDTH; x++)
    {
      ch = input[(WIDTH * y) + x];
      /*the forumla 3y+x to iterate through the init_board
      N.B WIDTH is 3. Board is a 1D array not 2D */
      if(!is_valid_char_input(ch))
      {
        return false;
      }
      board[y][x] = ch;
      for(i = 0; i < 9; i++)
      {
        if(to_use[i] == ch)
        {
          to_use[i] = 0;
        }
      }
    }
  }
  for(i = 0; i < 9; i++)
  {
    if(to_use[i] != 0)
    {
      fprintf(stderr, "Invalid input\n");
      return false;
    }
  }

  return true;
}

bool is_valid_char_input(char input)
{
  return input == ' '
      || ('1' <= input && input <= '8');
}

/*this function is to test and checks if
two parameters given are the same*/

/*void append_board(queue list_of_boards[MAX_NUM_BOARDS],
                  char board[HEIGHT][WIDTH],
                  int parent, int board_cnt)
{
  memcpy(list_of_boards[0].board, board, sizeof(char)*(HEIGHT*WIDTH));
  list_of_boards[0].parent = parent;
  list_of_boards[0].board_cnt = board_cnt;

}*/
Coordinate find_space(char board[HEIGHT][WIDTH])
{
  Coordinate space;
  for(space.y = 0; space.y < HEIGHT; space.y++)
  {
    for(space.x = 0; space.x < WIDTH; space.x++)
    {
      if(board[space.y][space.x] == ' ')
      {
        return space;
      }
    }
  }
  fprintf(stderr, "No empty spaces found in array\n");
  exit(-1);
}

bool is_valid_move(Coordinate place)
{
  return 0 <= place.y && place.y < HEIGHT
      && 0 <= place.x && place.x < WIDTH;
}

void get_moves(char board[HEIGHT][WIDTH], Coordinate moves[4])
{
  /*Make moves by first programming in L, R U and D
  then checking if moved coordinates are within board
  parameters*/
  Coordinate space = find_space(board);
  moves[0] = coordinate(space.y + UP, space.x);
  moves[1] = coordinate(space.y, space.x + RIGHT);
  moves[2] = coordinate(space.y + DOWN, space.x);
  moves[3] = coordinate(space.y, space.x + LEFT);
}

Coordinate coordinate(int y, int x)
{
  Coordinate coordinate;
  coordinate.y = y;
  coordinate.x = x;
  return coordinate;
}

void initialise_queue(Board_Queue* queue_pointer)
{
  queue_pointer -> write = 0;
  /*another way of writing this is *queue_pointer.write
    which means you're pointing to a queue to write
    something in */
  queue_pointer -> read = 0;
}

bool push_board(Board_Queue* queue_pointer, Board board)
{
  if(queue_pointer -> write == MAX_NUM_BOARDS)
  {
    return false;
  }
  if(contains_duplicate(queue_pointer, board))
  {
    return false;
  }

  queue_pointer -> array[queue_pointer -> write] = board;
  /*queue_pointer has an array which holds boards that can
    be written into which is then set to the board given to us*/
  queue_pointer -> write++; /*to go to next cell */

  return true;
  /*need to also check not a duplicate before completing push
  confirm 'write' board not already been in queue*/
}

bool pop_board(Board_Queue* queue_pointer,
               Board *board_pointer, int *board_index)
{
  if(queue_pointer -> read == queue_pointer -> write)
  {
    return false;
  }

  *board_pointer = queue_pointer -> array[queue_pointer -> read];
  *board_index = queue_pointer -> read;
  queue_pointer -> read++;

  return true;


}

bool contains_duplicate(const Board_Queue *queue_pointer, Board board)
{
  int i;

  for(i = 0; i < queue_pointer -> write; i++)
  {
    if(same_board(queue_pointer -> array[i].board, board.board))
    {
      return true;
    }
  }
  return false;
}
/*THE algorithm */
void solve_puzzle(char board[HEIGHT][WIDTH])
{
  bool unsolved_board;
  static Board_Queue queue;

  SDL_Simplewin sw;
  Font font;


  Board start_board;
  memcpy(start_board.board, board, HEIGHT*WIDTH);
  start_board.parent = NO_PARENT;

  initialise_queue(&queue);
  push_board(&queue, start_board);

  Neill_SDL_Init(&sw);
  Neill_SDL_Events(&sw);
  Neill_SDL_ReadFont(font, "mode7.fnt");

  unsolved_board = true;
  while(unsolved_board)
  {
    switch (make_children(&queue))
    {
      case NoPossibleMoves:
        fprintf(stderr, "No Possible Moves\n");
        return;

      case SolutionFound:
        unsolved_board = false;
        break;

      default:
        ; /* nothing to do, so KeepGoing*/
    }
  }
  print_list(&queue, (queue.write - 1), &sw, font);
  do{
    Neill_SDL_Events(&sw);
  }
  while(!sw.finished);
  SDL_Quit();
}

void print_list(const Board_Queue *queue_pointer, int index,
                SDL_Simplewin *sw, Font font)
{
  Board current;

  if(index == NO_PARENT)
  {
    return;
  }
  current = queue_pointer -> array[index];
  print_list(queue_pointer, current.parent, sw, font);
  print_board(current.board, sw, font);
}

Status make_children(Board_Queue *queue)
{
  Coordinate moves[4];
  int i;
  Board child_board;
  Coordinate space;
  Board board;

  char solution_board[HEIGHT][WIDTH];
  initialise_board(solution_board, "12345678 ");

  if(!pop_board(queue, &board, &child_board.parent))
  {
    return NoPossibleMoves;
  }
  space = find_space(board.board);
  get_moves(board.board, moves);

  for(i = 0; i < 4; i++)
  {
    if(is_valid_move(moves[i]))
    {
      memcpy(child_board.board, board.board, (HEIGHT*WIDTH));
      swap_tile(child_board.board, space, moves[i]);
      push_board(queue, child_board);
      if(same_board(solution_board, child_board.board))
      {
        return SolutionFound;
      }
    }
  }
  return KeepGoing;
}

void swap_tile(char board[HEIGHT][WIDTH],
               Coordinate space, Coordinate swap_to)
{
  char temp = board[space.y][space.x];
  board[space.y][space.x] = board[swap_to.y][swap_to.x];
  board[swap_to.y][swap_to.x] = temp;

}

void print_board(char board[HEIGHT][WIDTH], SDL_Simplewin *sw, Font font)
{
  int y;
  int x;
  SDL_Rect rectangle;
  rectangle.w = RECTSIZE;
  rectangle.h = RECTSIZE;

  Neill_SDL_SetDrawColour(sw, 0, 0 ,0);
  SDL_RenderClear(sw -> renderer);

  for(y=0; y < HEIGHT; y++)
  {
    rectangle.y = 5 + y*(rectangle.h +10);
    for(x=0; x < WIDTH; x++)
    {
      if(board[y][x] != ' ')
      {
        rectangle.x = 5 + x*(rectangle.w + 10);
        Neill_SDL_SetDrawColour(sw, 128, 192, 255);
        SDL_RenderFillRect(sw -> renderer, &rectangle);
        Neill_SDL_SetDrawColour(sw, 225, 255, 225);
        Neill_SDL_DrawChar(sw, font, board[y][x],
                           rectangle.x + 30, rectangle.y + 30);
      }
            /*printf("%c ", board[y][x]);*/
    }
    /*printf("\n");*/
  }
  Neill_SDL_UpdateScreen(sw);
  SDL_Delay(325);
  Neill_SDL_Events(sw);
}

void board_literal(char board[HEIGHT][WIDTH],
  char a, char b, char c,
  char d, char e, char f,
  char g, char h, char i)
{
  board[0][0] = a;
  board[0][1] = b;
  board[0][2] = c;
  board[1][0] = d;
  board[1][1] = e;
  board[1][2] = f;
  board[2][0] = g;
  board[2][1] = h;
  board[2][2] = i;
}

bool same_board(const char left_board[HEIGHT][WIDTH],
                const char right_board[HEIGHT][WIDTH])
{
  int y;
  int x;

  for(y=0; y < HEIGHT; y++)
  {
    for(x=0; x < WIDTH; x++)
    {
      if(left_board[y][x] != right_board[y][x])
      {
      return false;
      }
    }
  }
  return true;
}

void test(void)
{
  char *test_input;
  char test_board[HEIGHT][WIDTH];
  char answer_board[HEIGHT][WIDTH];
  Coordinate test_coordinates[4];
  static Board_Queue test_queue;
  Board testboard_board;
  int test_index;
  /* test if is_valid_char_input works*/
  assert(is_valid_char_input(' ') == 1);
  assert(is_valid_char_input('1') == 1);
  assert(is_valid_char_input('2') == 1);
  assert(is_valid_char_input('3') == 1);
  assert(is_valid_char_input('4') == 1);
  assert(is_valid_char_input('5') == 1);
  assert(is_valid_char_input('6') == 1);
  assert(is_valid_char_input('7') == 1);
  assert(is_valid_char_input('8') == 1);

  assert(is_valid_char_input('0') == 0);
  assert(is_valid_char_input('9') == 0);
  assert(is_valid_char_input('a') == 0);
  assert(is_valid_char_input('b') == 0);
  assert(is_valid_char_input('!') == 0);
  assert(is_valid_char_input('#') == 0);

  /*function to compare two boards */
  test_input = "1234 5678";
  board_literal(answer_board, '1', '2', '3', '4', ' ',
                              '5', '6', '7', '8');
  assert(initialise_board(test_board, test_input));
  assert(same_board(test_board, answer_board));

  /*test coordinates cannot go out of bounds */

  assert(is_valid_move(coordinate(0,0)) == 1);
  assert(is_valid_move(coordinate(0,1)) == 1);
  assert(is_valid_move(coordinate(0,2)) == 1);
  assert(is_valid_move(coordinate(2,2)) == 1);
  assert(is_valid_move(coordinate(2,1)) == 1);
  assert(is_valid_move(coordinate(1,2)) == 1);
  assert(is_valid_move(coordinate(1,0)) == 1);

  assert(is_valid_move(coordinate(3,2)) == 0);
  assert(is_valid_move(coordinate(7,2)) == 0);
  assert(is_valid_move(coordinate(4,1)) == 0);
  assert(is_valid_move(coordinate(2,3)) == 0);
  assert(is_valid_move(coordinate(1,0)) != 0);

  /* test to find space*/
  initialise_board(test_board, "12 345678");
  assert(find_space(test_board).x == 2
      && find_space(test_board).y == 0);

  /*test get moves */
  initialise_board(test_board, "1234 5678");
  get_moves(test_board, test_coordinates);
  assert(test_coordinates[0].x == 1);
  assert(test_coordinates[0].y == 0);
  assert(test_coordinates[1].x == 2);
  assert(test_coordinates[1].y == 1);
  assert(test_coordinates[2].x == 1);
  assert(test_coordinates[2].y == 2);
  assert(test_coordinates[3].x == 0);
  assert(test_coordinates[3].y == 1);
  /*function works in clockwise order */

  /*test moves queues work as intended*/
  initialise_queue(&test_queue);
  assert(test_queue.read == 0);
  assert(test_queue.write == 0);

  /*test push and pop*/
  memcpy(testboard_board.board, test_board, HEIGHT*WIDTH);
  testboard_board.parent = -1;
  assert(push_board(&test_queue, testboard_board));
  assert(test_queue.write == 1);
  assert(test_queue.read == 0);
  assert(same_board(
          testboard_board.board,
          test_queue.array[0].board
  ));
  assert(testboard_board.parent == test_queue.array[0].parent);

  assert(pop_board(&test_queue, &testboard_board, &test_index));
  assert(test_queue.write == 1);
  assert(test_queue.read == 1);
  assert(same_board(
          testboard_board.board,
          test_board
  ));
  assert(testboard_board.parent == -1);
  assert(test_index == 0);

  assert(contains_duplicate(&test_queue, testboard_board));

  /*following test tests both make_children
   as well as swap_tile functions */
  assert(make_children(&test_queue)== NoPossibleMoves);
  test_queue.read = 0;
  /* manually moving back to test again*/
  assert(make_children(&test_queue) == KeepGoing);
  assert(test_queue.write == 5);

  /*almost solved board so re-set queue */
  initialise_queue(&test_queue);
  initialise_board(testboard_board.board, "1234567 8");
  assert(push_board(&test_queue, testboard_board));
  assert(make_children(&test_queue) == SolutionFound);

  printf("All tests succesfully passed\n");
}
