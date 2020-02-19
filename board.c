#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "board.h"

struct board_t
{
        unsigned int width;
        unsigned int height;
        unsigned int run;
        player_t* config;
};


bool board_create (board_t ** b, unsigned int height,
        unsigned int width, unsigned int run,
        const player_t * initial)
{
        board_t * board = (board_t *) malloc(sizeof(*board));
        board -> width = width;
        board -> height = height;
        board -> run = run;
        board -> config = (player_t*) malloc(width * height * sizeof(player_t));

        int counter = 0;
        for(int w = 0; w < width; w++)
        {
            for(int h = 0; h < height; h++)
            {
                if(initial == NULL)
                {
                        board -> config[counter] = PLAYER_EMPTY;
                        counter++;
                }
                else 
                {
                        board -> config[counter] = initial[counter];
                        counter++;
                }
            }
        }
        *b = board;
        return true;
}

bool board_destroy (board_t * b)
{
        free(b->config);
        free(b);

        return true;
}

unsigned int board_get_height (const board_t * b)
{
        return b -> height;
}

unsigned int board_get_width (const board_t * b)
{
        return b -> width;
}

unsigned int board_get_run (const board_t * b)
{
        return b -> run;
}

/// Return true if the given move is valid, false otherwise
/// If true, the board was changed and the move was made.
/// If false, the board remains the same.
bool board_play (board_t * b, unsigned int column, player_t player)
{
        if(board_can_play_move(b, player, column))
        {
            unsigned int height = board_get_height(b);
            unsigned int width = board_get_width(b);

            int counter = 0;
            player_t * piece = malloc(sizeof(player_t)); 

            //go from bottom up, left right
            for(int h = 0; h < height; h++)
                {
                for(int w = 0; w < width; w++)
                    {                
                        if(counter % width == column)
                        {
                            if(board_get(b, h, w, piece))    
                            {
                                if(*piece == PLAYER_EMPTY)
                                {
                                    unsigned int position = h * width + w;
                                    b -> config[position] = player;                                   
                                    free(piece);
                                    return true;
                                }
                            }
                        }    
                        counter++;
                    }           
            }
            free(piece);
        }

        return false;
}


/// Return true if the specified player can make a move on the
/// board. THERE IS NO NEED TO KEEP TRACK OF THE TURN OR TO VALIDATE THIS.
/// The board is not changed.
bool board_can_play (const board_t * b, player_t p)
{
    unsigned int width = board_get_width(b);
    unsigned int height = board_get_height(b);
    player_t * piece = malloc(sizeof(player_t)); 

     //check top row if it's there's a playable empty space
    for(int w = 0; w < width; w++)
    {
        if(board_get(b, height - 1, w, piece))
        {
            if(*piece == PLAYER_EMPTY)
            {
                free(piece);
                return true;
            }                
        }
    }
    free(piece);
    return false;
}



/// Return true if the specified move is valid
/// The board is not changed.
bool board_can_play_move (const board_t * b, player_t p, unsigned int
      column)
{
    unsigned int height = board_get_height(b);
    unsigned int width = board_get_width(b);
    player_t * piece = malloc(sizeof(player_t)); 

    if(column < 0 || column >= width)
    {
        return false;
    }

    //check top row if it's filled
    for(int w = 0; w < width; w++)
    {
        if(w == column)
        {
            if(board_get(b, height - 1, w, piece))
            {
                if(*piece != PLAYER_EMPTY)
                {
                    free(piece);
                    return false;
                }
            }    
        }
    }
    free(piece);
    return true;
}



// / Undo a move (i.e. make the specified board slot empty)
// / Return true if OK, false on error
// / If the function returns true, the board has been modified.
// / If the function returns false, the board has not been modified.
// / There is no need to keep track of the which player last played.
bool board_unplay (board_t * b, unsigned int column)
{   
    unsigned int height = board_get_height(b);
    unsigned int width = board_get_width(b);

    if(column < 0 || column >= width)
    {
        return false;
    }

    int counter = 0;
    player_t * piece = malloc(sizeof(player_t)); 

    //go from top down, left right
    for(int h = height - 1; h >= 0; h--)
        {
        for(int w = 0; w < width; w++)
            {                
                if(counter % width == column)
                {
                    // printf("%d%d\n", h,w);
                    if(board_get(b, h, w, piece))    
                    {
                        if(*piece != PLAYER_EMPTY)
                        {
                            b -> config[counter] = PLAYER_EMPTY;
                            free(piece);
                            return true;
                        }
                    }
                }    
                counter++;
            }           
    }
    free(piece);
    return false;
}


/// Duplicate a board
//      return true on success; New board needs to be freed 
///     with board_destroy.
///
/// Return true on success, false on error
bool board_duplicate (board_t ** newboard, const board_t * old)
{
    unsigned int height = board_get_height(old);
    unsigned int width = board_get_width(old);
    unsigned int run = board_get_run(old);
    player_t * config = old -> config;
    
    if(board_create(newboard, height, width, run, config))
    {
        return true;    
    }
    return false;
}



/// Return -1 if tie, 1 if winner, 0 if no winner
/// If there is a winner, set player to the winning color.
///
/// no winner means that the game can still continue (i.e. open positions)
/// winner means somebody has N tokens in a straight line.
/// tie means there are no more valid moves and there is no winner.
///
/// You can assume that only one of the above conditions is true.
///
int board_has_winner (const board_t * b, player_t * player)
{
    int WIN = 1;
    int TIE = -1;
    int CONTINUE = 0;
    unsigned int height = board_get_height(b);
    unsigned int width = board_get_width(b);
    unsigned int run = board_get_run(b);
    
    player_t * piece = malloc(sizeof(player_t)); 
    unsigned int piece_temp = PLAYER_EMPTY;
    unsigned int win_counter = 1;

    //checking horizontal wins
    //go from bottom up, left right
    for(int h = 0; h < height; h++)
        {
        for(int w = 0; w < width; w++)
        {                
            if(board_get(b, h, w, piece))    
            {
                if(piece_temp == *piece && piece_temp != PLAYER_EMPTY)
                {
                    win_counter++;
                    piece_temp = *piece;
                    if(win_counter == run)
                    {
                        *player = *piece;
                        free(piece);
                        return WIN;
                    }                                 
                }
                else
                {
                    win_counter = 1;
                    piece_temp = *piece;
                }
            }               
        }
        win_counter = 1;
        piece_temp = PLAYER_EMPTY;           
    }
    
    //checking vertical wins
    //go from bottom up, left right
    piece_temp = PLAYER_EMPTY;
    win_counter = 1;
    
    for(int w = 0; w < width; w++)
        {        
        for(int h = 0; h < height; h++)
        {      
            if(board_get(b, h, w, piece))    
            {
                if(piece_temp == *piece && piece_temp != PLAYER_EMPTY)
                {
                    win_counter++;
                    piece_temp = *piece;
                    if(win_counter == run)
                    {
                        *player = *piece;
                        free(piece);
                        return WIN;
                    }                                 
                }
                else
                {
                    win_counter = 1;
                    piece_temp = *piece;
                }
            }
        
    
        }
        win_counter = 1;
        piece_temp = PLAYER_EMPTY;           
    }
    
    //checking left top, bottom down diagonal wins
    piece_temp = PLAYER_EMPTY;
    win_counter = 1;
    for(int h = 0; h < height; h++)
    {
        int h_temp, w;
        for(h_temp = h, w = 0; w < width && h_temp < height; h_temp++, w++)
        {
            // printf("test1: %d,%d\n", h_temp, w);
            if(board_get(b, h_temp, w, piece))    
            {
                if(piece_temp == *piece && piece_temp != PLAYER_EMPTY)
                {   
                    win_counter++;
                    piece_temp = *piece;
                    if(win_counter == run)
                    {
                        *player = *piece;
                        free(piece);
                        return WIN;
                    }
                }
                else
                {
                    win_counter = 1;
                    piece_temp = *piece;
                }
            }
        }
        win_counter = 1;
        piece_temp = PLAYER_EMPTY;
    }

    piece_temp = PLAYER_EMPTY;
    win_counter = 1;
    for(int w = 1; w < width; w++)
    {
        int w_temp, h;
        for(h = 0, w_temp = w; w_temp < width && h < height; h++, w_temp++)
        {
            // printf("test2: %d,%d\n", h, w_temp);
            if(board_get(b, h, w_temp, piece))    
            {
                if(piece_temp == *piece && piece_temp != PLAYER_EMPTY)
                {   
                    win_counter++;
                    piece_temp = *piece;
                    if(win_counter == run)
                    {
                        *player = *piece;
                        free(piece);
                        return WIN;
                    }
                }
                else
                {
                    win_counter = 1;
                    piece_temp = *piece;
                }
            }
        }
        win_counter = 1;
        piece_temp = PLAYER_EMPTY;
    }

      

    //checking right top, left down diagonal wins
    piece_temp = PLAYER_EMPTY;
    win_counter = 1;
    for(int h = 0; h < height; h++)
    {
        int h_temp, w;
        for(h_temp = h, w = width-1; w >= 0 && h_temp < height; h_temp++, w--)
        {
            // printf("test3: %d,%d\n", h_temp, w);
            if(board_get(b, h_temp, w, piece))    
            {
                if(piece_temp == *piece && piece_temp != PLAYER_EMPTY)
                {   
                    win_counter++;
                    piece_temp = *piece;
                    if(win_counter == run)
                    {
                        *player = *piece;
                        free(piece);
                        return WIN;
                    }
                }
                else
                {
                    win_counter = 1;
                    piece_temp = *piece;
                }
            }
        }
        win_counter = 1;
        piece_temp = PLAYER_EMPTY;
    }

    piece_temp = PLAYER_EMPTY;
    win_counter = 1;
    for(int w = 0; w < width; w++)
    {
        int w_temp, h;
        for(h = 0, w_temp = w; w_temp < width && h < height; h++, w_temp--)
        {
            // printf("test4: %d,%d\n", h, w_temp);
            if(board_get(b, h, w_temp, piece))    
            {
                if(piece_temp == *piece && piece_temp != PLAYER_EMPTY)
                {   
                    win_counter++;
                    piece_temp = *piece;
                    if(win_counter == run)
                    {
                        *player = *piece;
                        free(piece);
                        return WIN;
                    }
                }
                else
                {
                    win_counter = 1;
                    piece_temp = *piece;
                }
            }
        }
        win_counter = 1;
        piece_temp = PLAYER_EMPTY;
    }

      
    if(board_can_play (b, *player))
    {
        free(piece);   
        return CONTINUE;
    }


    free(piece);   
    return TIE;

}


/// Return game piece at specified position
/// Returns false if the position is not valid on the board,
/// returns true otherwise and copies the piece at that location on the board
/// to *piece
bool board_get (const board_t * b, unsigned int row, unsigned int column,
      player_t * piece)
{
    unsigned int height = board_get_height(b);
    unsigned int width = board_get_width(b);
    
    if(row < 0 || column >= width || column < 0 || row >= height)
    {
        return false;
    }
    unsigned int position = row * width + column;
    *piece = b -> config[position];    
    return true;
}

/// Clear the board
/// Return true if success, false otherwise
bool board_clear (board_t * b)
{
    unsigned int height = board_get_height(b);
    unsigned int width = board_get_width(b);

    for(int i = 0; i < width * height; i ++)
    {
        b -> config[i] = PLAYER_EMPTY;
    }

    return true;
}



/// Write the board to the given file (for visual aid for the player)
/// Return true if OK, false on error
bool board_print (const board_t * b, FILE * f)
{        
    unsigned int height = board_get_height(b);
    unsigned int width = board_get_width(b);
        
    for(int h = 0; h < height; h++)
    {
        for(int w = 0; w < width; w++)
            {
                //need to flip board around top bottom to match cartesian coordiates visually
                unsigned int position = (height - h - 1) * width + w;
                fprintf(f, "%u", b->config[position]);
            }
            fprintf(f, "\n");

    }

    fprintf(f, "\n"); //take this out later
    return true;
}
