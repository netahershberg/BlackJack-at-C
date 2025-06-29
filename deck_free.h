#ifndef DECK_MANAGER_H
#define DECK_MANAGER_H

#include <stdlib.h>
#include "card_intialize.h"  

void free_deck(void *deck);

void cleanup(GameState *gamestate);

#endif 
