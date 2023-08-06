#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state()
{
  game_state_t *p_game = (game_state_t *)malloc(sizeof(game_state_t));
  if (p_game)
  {
    p_game->num_rows = 18;
    int num_cols = 20;

    p_game->board = (char **)malloc(sizeof(char *) * ((size_t)p_game->num_rows));
    if (!p_game->board)
    {
      free(p_game);
      return NULL;
    }

    char *board[] = {
        "####################",
        "#                  #",
        "# d>D    *         #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "####################"};

    for (int i = 0; i < p_game->num_rows; i++)
    {
      p_game->board[i] = (char *)malloc(sizeof(char) * (size_t)(num_cols + 1));
      if (!p_game->board[i])
      {
        for (int j = 0; j < i; j++)
          free(p_game->board[j]);
        free(p_game->board);
        free(p_game);
        return NULL;
      }
      strncpy(p_game->board[i], board[i], (size_t)(num_cols + 1));
    }

    p_game->num_snakes = 1;
    p_game->snakes = (snake_t *)malloc(sizeof(snake_t) * p_game->num_snakes);
    if (!p_game->snakes)
    {
      for (int i = 0; i < p_game->num_rows; i++)
        free(p_game->board[i]);
      free(p_game->board);
      free(p_game);
      return NULL;
    }

    p_game->snakes->tail_row = 2;
    p_game->snakes->tail_col = 2;
    p_game->snakes->head_row = 2;
    p_game->snakes->head_col = 4;

    p_game->snakes->live = true;
    return p_game;
  }
  return NULL;
}
/* Task 2 */
void free_state(game_state_t *state)
{
  // TODO: Implement this function.
  if (state)
  {
    for (int i = 0; i < state->num_rows; i++)
    {
      if (state->board[i])
        free(state->board[i]);
    }
    if (state->board)
      free(state->board);
    if (state->snakes)
      free(state->snakes);
    free(state);
  }
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp)
{
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++)
    fprintf(fp, "%s\n", state->board[i]);
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename)
{
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col)
{
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch)
{
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c)
{
  // TODO: Implement this function.
  if (c == 'w' || c == 'a' || c == 's' || c == 'd')
    return true;
  else
    return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c)
{
  // TODO: Implement this function.
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x')
    return true;
  else
    return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c)
{
  // TODO: Implement this function.
  if (is_head(c) || is_tail(c) || c == '<' || c == '>' || c == '^' || c == 'v')
    return true;
  else
    return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c)
{
  // TODO: Implement this function.
  switch (c)
  {
  case '^':
    return 'w';
    break;
  case '<':
    return 'a';
    break;
  case 'v':
    return 's';
    break;
  case '>':
    return 'd';
    break;
  default:
    return c;
    break;
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c)
{
  switch (c)
  {
  case 'W':
    return '^';
    break;
  case 'A':
    return '<';
    break;
  case 'S':
    return 'v';
    break;
  case 'D':
    return '>';
    break;
  default:
    return c;
    break;
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c)
{
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S')
    return cur_row + 1;
  else if (c == '^' || c == 'w' || c == 'W')
    return cur_row - 1;
  else
    return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c)
{
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D')
    return cur_col + 1;
  else if (c == '<' || c == 'a' || c == 'A')
    return cur_col - 1;
  else
    return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum)
{
  // print_board(state,stdout);

  // TODO: Implement this function.
  unsigned int row = ((state->snakes) + snum)->head_row;
  unsigned int col = ((state->snakes) + snum)->head_col;
  char c = get_board_at(state, row, col);

  row = get_next_row(row, c);
  col = get_next_col(col, c);

  c = get_board_at(state, row, col);

  // game_state_t* expected_state = create_default_state();
  // print_board(state,stdout);
  // print_board(expected_state,stdout);
  return c;
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  unsigned int row = ((state->snakes) + snum)->head_row;
  unsigned int col = ((state->snakes) + snum)->head_col;
  char c = get_board_at(state, row, col);
  set_board_at(state, row, col, head_to_body(c));

  row = get_next_row(row, c);
  col = get_next_col(col, c);
  ((state->snakes) + snum)->head_row = row;
  ((state->snakes) + snum)->head_col = col;
  set_board_at(state, row, col, c);

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum)
{
  // print_board(state,stdout);

  // TODO: Implement this function.
  unsigned int row = ((state->snakes) + snum)->tail_row;
  unsigned int col = ((state->snakes) + snum)->tail_col;
  char c = get_board_at(state, row, col);
  set_board_at(state, row, col, ' ');

  row = get_next_row(row, c);
  col = get_next_col(col, c);
  ((state->snakes) + snum)->tail_row = row;
  ((state->snakes) + snum)->tail_col = col;
  set_board_at(state, row, col, body_to_tail(get_board_at(state, row, col)));

  // print_board(state,stdout);
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state))
{
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_snakes; i++)
  {
    if ((state->snakes + i)->live)
    {
      char c = next_square(state, i);
      if (is_snake(c) || c == '#')
      {
        (state->snakes + i)->live = false;
        set_board_at(state, (state->snakes + i)->head_row, (state->snakes + i)->head_col, 'x');
      }
      else if (c == '*')
      {
        update_head(state, i);
        add_food(state);
      }
      else
      {
        update_head(state, i);
        update_tail(state, i);
      }
    }
  }
  return;
}

/* Task 5 */
game_state_t *load_board(FILE *fp)
{
  // TODO: Implement this function.
  char *buf = (char *)malloc(1024 * sizeof(char));
  if (!buf)
    return NULL;

  size_t cnt = 0, len = 1024;
  char ch;

  if (!fp)
    return NULL;

  game_state_t *state = (game_state_t *)malloc(sizeof(game_state_t));
  if (!state)
    return NULL;

  state->board = NULL;
  state->num_rows = 0;
  state->num_snakes = 0;
  state->snakes = NULL;

  while ((ch = (char)fgetc(fp)) != EOF)
  {
    if (ch == '\n')
    {
      buf[cnt] = '\0';
      cnt++;

      if (state->num_rows == 0)
      {
        state->num_rows++;
        state->board = (char **)malloc(sizeof(char *));

        if (!state->board)
          return NULL;
        (state->board)[0] = (char *)malloc(sizeof(char) * (cnt));
        if (!(state->board)[0])
          return NULL;
        strncpy((state->board)[0], buf, cnt);
      }
      else
      {
        state->num_rows++;
        state->board = (char **)realloc(state->board, sizeof(char *) * state->num_rows);

        if (!state->board)
          return NULL;
        (state->board)[state->num_rows - 1] = (char *)malloc(sizeof(char) * (cnt));
        if (!(state->board)[state->num_rows - 1])
          return NULL;
        strncpy((state->board)[state->num_rows - 1], buf, cnt);
      }
      cnt = 0;
    }
    else
    {
      buf[cnt] = ch;
      cnt++;
      if (cnt == len)
      {
        buf = (char *)realloc(buf, (len + 1024) * sizeof(char));
        len += 1024;
        if (!buf)
          return NULL;
      }
    }
  }

  //print_board(state,stdout);
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  unsigned int row = (state->snakes + snum)->tail_row;
  unsigned int col = (state->snakes + snum)->tail_col;

  char ch = get_board_at(state, row, col);

  while (!is_head(ch))
  {
    /* code */
    row = get_next_row(row,ch);
    col = get_next_col(col,ch);
    ch = get_board_at(state, row, col);
  }
  if (ch == 'x')
    (state->snakes + snum)->live = false;
  else
    (state->snakes + snum)->live = true;
  (state->snakes + snum)->head_row = row;
  (state->snakes + snum)->head_col = col;

  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state)
{
  // return NULL;
  // TODO: Implement this function.
  if (!state)
    return NULL;
  state->num_snakes = 0;
  state->snakes = NULL;

  for (unsigned i = 0; i < state->num_rows; i++)
  {
    for (unsigned j = 0; state->board[i][j]; j++)
    {
      char ch = get_board_at(state, i, j);

      if (is_tail(ch))
      {
        state->num_snakes++;

        if (state->num_snakes == 1)
        {
          state->snakes = (snake_t *)malloc(sizeof(snake_t) * state->num_snakes);
          if (!state->snakes)
            return NULL;
          state->snakes->tail_row = i;
          state->snakes->tail_col = j;
          find_head(state, 0);
        }
        else
        {
          state->snakes = (snake_t *)realloc(state->snakes,sizeof(snake_t) * state->num_snakes);
          if (!state->snakes)
            return NULL;
          (state->snakes + state->num_snakes - 1)->tail_row = i;
          (state->snakes + state->num_snakes - 1)->tail_col = j;
          find_head(state, state->num_snakes - 1);
        }
      }
    }
  }

  return state;
}
