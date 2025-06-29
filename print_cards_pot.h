#ifndef PRINT_CARDS_POT_H
#define PRINT_CARDS_POT_H

#define CARD_WIDTH 33
#define CARD_HEIGHT 7
#define MAX_DECK_CARDS 52

void print_pot_cash( GameState *gamestate, int checker);
void print_pot_loop(GameState *gamestate, int index , int checker);
void build_card_lines(char card_lines[CARD_HEIGHT][CARD_WIDTH + 1], const char *rank, const char *symbol,int hide);
void print_deck_side_by_side_generic(GameState *gamestate, int hide_second_card, int is_dealer);
int calculate_score(void *deck_ptr);
void clear_screen();

#endif 
