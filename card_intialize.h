#ifndef card_intialize_h
#define card_intialize_h
#include<inttypes.h>
#include<stdbool.h>
#include <stdlib.h>

#define clean_screen "\033[3J\033[2J\033[H"

typedef struct Card Card;
struct Card {
    uint8_t value;
    Card *next;
};

typedef struct Deck Deck;
struct Deck {
    Card *head;
    Card *tail;
    size_t len;
};

typedef struct Player_Hand Player_Hand;
struct Player_Hand {
    Card *head;
    Card *tail;
    size_t len;
};

typedef struct Dealer_Hand Dealer_Hand;
struct Dealer_Hand {
    Card *head;
    Card *tail;
    size_t len;
};

typedef struct GameState GameState;
struct GameState{
    Deck cardlist;
    Dealer_Hand dealercardlist;
    Player_Hand handcardlist;
    unsigned int cash;
    unsigned int pot;
    unsigned int bet;
} ;

void pop_message(bool condition, const char *error_message);
void init_deck(void *deck);
int my_pow(int n);
Card* card_new(int rank, int suit);
void card_push(void *deck_void, Card *card);
void fill_deck(GameState *gamestate);
Card* card_remove_at(Deck *deck, size_t index);
void print_bit_u8(uint8_t val);
int get_random_number(Deck *deck);
void init_cash_and_pot(GameState *gamestae);
void set_cash_pot(GameState *gamestate, unsigned int value, int reset_pot);
void put_bet(GameState *gamestate);
const char *get_suit_symbol(uint8_t suit_bits);
const char *rank_to_str(uint8_t rank);
void reset_cards(Player_Hand *player_hand, Dealer_Hand *dealer_hand, Deck *deck);
void print_spacer(int lines);

#endif