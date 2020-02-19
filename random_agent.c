#include "random_agent.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

static char * mystrdup (const char * s)
{
   char * n = (char *) malloc (strlen(s)+1);
   strcpy (n, s);
   return n;
}

typedef struct
{
   char * name;
} random_agent_data_t;

static const char * random_agent_describe (agent_t * gthis)
{
   random_agent_data_t * data = (random_agent_data_t *) gthis->data;
   return data->name;
}


static int random_agent_play (agent_t * gthis, const board_t * b,
        player_t color, unsigned int * move)
{
   srand(time(NULL));
   const unsigned int width = board_get_width(b);
   
   while (true)
   {
   	  int rand_col = (rand() % (width)); //gets a random column from 0 to width - 1
      *move = rand_col;

      if (!board_can_play_move(b, color, *move))
      {
         continue;
      }

      return 0;
   }
}

static bool random_agent_destroy (agent_t * this)
{
   random_agent_data_t * data = (random_agent_data_t *) this->data;
   free (data->name);
   free (this->data);
   return true;
}

agent_t * random_agent_create()
{
    // Allocate vtable structure
    agent_t * n = malloc (sizeof(agent_t));

    n->destroy = random_agent_destroy;
    n->play = random_agent_play;
    n->describe = random_agent_describe;

    random_agent_data_t * data = malloc (sizeof(random_agent_data_t));
    n->data = data;

    data->name = mystrdup("RANDOM_COMPUTER");

    return n;
}

