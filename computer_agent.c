#include "computer_agent.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <math.h>
/*
 * In this file, implement your AI agent.
 *
 * Use the minimax algorithm to determine the next move.
 *
 * Look at player_agent.h and player_agent.c for an example of how to
 * implement each function.
 */
// agent_t * computer_agent_create()
// {
//     return NULL;
// }



static char * mystrdup (const char * s)
{
   char * n = (char *) malloc (strlen(s)+1);
   strcpy (n, s);
   return n;
}

int optimal_move(const board_t * b, player_t player, int flip_player_counter, bool first_play)
{  
    int result = 0;   
    int move = -1;
    double max_result = -INFINITY;
    
    // board_print(b, f);  

    player_t opponent;
    opponent = (player == PLAYER_BLUE) ? PLAYER_YELLOW : PLAYER_BLUE;  

    // base case     
    player_t player_temp = PLAYER_EMPTY;
    if(board_has_winner(b, &player_temp) == 1 && player_temp == player && flip_player_counter % 2 == 0)
    {
       return 100;
    } 
    else if(board_has_winner(b, &player_temp) == 1 && player_temp == player && flip_player_counter % 2 == 1)
    {
       return -100;
    } 
    else if(board_has_winner(b, &player_temp) == -1)
    {
       return 0;
    }   

   // recurse 
   for(int col = 0 ; col < board_get_width(b); col++)
   {
      if(board_can_play_move(b, player, col) == 1)
      {
         board_t * board_temp;
         board_duplicate(&board_temp, b);
         flip_player_counter++;

         if(!first_play)
         {           
            // not sure if below commented code was on right track, was trying to calculate
            // optimal move by opponent before calculating optimal move by current player

            // int move_temp;
            // move_temp = optimal_move(board_temp, opponent, flip_player_counter, true);                      
            // board_play(board_temp, move_temp, opponent);
            
            board_play(board_temp, col, opponent);
            result += optimal_move(board_temp, opponent, flip_player_counter, false);                              
         }
         else
         {
            board_play(board_temp, col, player);
            result += optimal_move(board_temp, player, flip_player_counter, false);                   
            if(result > max_result)
            {
                max_result = result;
                move = col;
            }
         }
         free(board_temp);
      }
   }

   if(first_play)
   {
      return move;
   }
   return result;
}

typedef struct
{
   char * name;
} computer_agent_data_t;

static const char * computer_agent_describe (agent_t * gthis)
{
   computer_agent_data_t * data = (computer_agent_data_t *) gthis->data;
   return data->name;
}


static int computer_agent_play (agent_t * gthis, const board_t * b,
        player_t color, unsigned int * move)
{   
   while (true)
   {
      *move = optimal_move(b, color, 0, true);

      if (!board_can_play_move(b, color, *move))
      {
         continue;
      }

      return 0;
   }
}

static bool computer_agent_destroy (agent_t * this)
{
   computer_agent_data_t * data = (computer_agent_data_t *) this->data;
   free (data->name);
   free (this->data);
   return true;
}

agent_t * computer_agent_create()
{
    // Allocate vtable structure
    agent_t * n = malloc (sizeof(agent_t));

    n->destroy = computer_agent_destroy;
    n->play = computer_agent_play;
    n->describe = computer_agent_describe;

    computer_agent_data_t * data = malloc (sizeof(computer_agent_data_t));
    n->data = data;

    data->name = mystrdup("SMART_COMPUTER");

    return n;
}

