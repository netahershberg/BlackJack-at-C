#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h> 

#include "card_intialize.h"
#include "deck_free.h"
#include "colors.h"
#include "symbols.h"


void pop_message(bool condition, const char *error_message) {
    if ( ! condition) {
        fprintf(stderr, "%s\n", error_message);
        exit(EXIT_FAILURE);
    }
}

void init_deck(void *deck) {
    Deck *d = (Deck *)deck;
    d->head = NULL;
    d->tail = NULL;
    d->len  = 0;
}
    
int my_pow(int n){
    return 1<<(n-1);
}

Card* card_new(int rank, int suit) {
    Card *pNode = malloc(sizeof (Card));
    pop_message(pNode != NULL, "card_new: malloc failed" );
    suit = my_pow(suit);
    uint8_t rank_suit = (rank<<4) | (suit & 0x0F);
    pNode->value = rank_suit;
    pNode->next = NULL;
    return pNode;
}

void card_push(void *deck_void, Card *card) {
    Deck *deck = (Deck *)deck_void;
    pop_message(deck != NULL, "card_push: deck cannot be NULL");
    pop_message(card != NULL, "card_push: node cannot be NULL");

    card->next = NULL;

    if (deck->len == 0) {
        deck->head = card;      // first card
        deck->tail = card;      // also last card
    } else {
        deck->tail->next = card; // link last card to new one
        deck->tail = card;       // update tail
    }
    deck->len++;
}

void fill_deck(GameState *gamestate){
    size_t NUM_SUITS = 4;
    size_t NUM_RANKS  = 13;
    for (size_t i=0; i < NUM_SUITS ; i++){ //4
        for(size_t j=0 ; j<NUM_RANKS  ;j++){ //13
            Card *c = card_new(j+1, i+1);
            card_push(&gamestate->cardlist, c);
        }
    }
}

Card* card_remove_at(Deck *deck, size_t index) {
    pop_message(index < deck->len, "pos can't be len or greater");
    Card *prev = NULL;
    Card *curr = deck->head;
    for (size_t i = 0; i < index && curr != NULL; ++i) {
        prev = curr;
        curr = curr->next;
    }
    if (prev == NULL) {
        //         = deck->head->next;
        deck->head = curr->next;
    }
    else {
       prev->next  = curr->next;
    }
    //update tail
    if (deck->tail == curr) {
        deck->tail = prev;
    }
    deck->len--;
    curr->next = NULL;
    return curr;
}

int get_random_number(Deck *deck){
    if (deck == NULL) {
        fprintf(stderr, "Error: deck pointer is NULL\n");
        return -1;  // error code
    }
    if (deck->len == 0) {
        printf(clean_screen);
        fprintf(stderr, "Error: deck is empty, length is zero\n");
        return -1;  // error code
    }

    return rand() % deck->len;
}

void init_cash_and_pot(GameState *gamestae){
    gamestae->cash =1000;
    gamestae->pot =0;
}

void set_cash_pot(GameState *gamestate, unsigned int payout, int reset_pot){
    gamestate->cash += payout;
    if (reset_pot) {
        gamestate->pot = 0;
    }
}

void put_bet(GameState *gamestate){

    printf(clean_screen);
    if (gamestate->cash > 500){
        printf(clean_screen "\nYour cash is: " GREEN "%-3u" RESET "\n", gamestate->cash);
    }
    else{
        printf("\nYour cash is: " RED "%-3u" RESET "\n", gamestate->cash);
    }
    if (gamestate->cash< 10 && gamestate->pot == 0) {
        printf(RED "You are out of cash to bet and the pot is empty. Game over" RESET);
        cleanup(gamestate);
        print_spacer(40);
        exit(0);
    }
    while (1) {
        if (gamestate->cash == 0 && gamestate->pot > 0) {
            gamestate->bet = 0;
            printf(YELLOW "You are allowed to play with the pot only, so the bet is automatically set to 0.\n" RESET);
            sleep(3);
            break;
        } else {
            printf(CYAN "Enter your bet (between 10 and %u, multiples of 10): " RESET, gamestate->cash);}
        
        int result = scanf("%u", &gamestate->bet);          

        while (getchar() != '\n')

        if (result != 1) {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            continue;
        }
        if (gamestate->bet < 10 && gamestate->pot == 0) {
            printf(RED "Bet must be at least 10.\n" RESET);
            continue;
        }
        if (gamestate->bet % 10 != 0) {
            printf(RED "Bet must be a multiple of 10.\n" RESET);
            continue;
        }
        if (gamestate->bet > gamestate->cash) {
            printf(RED "You can't bet more than you have (%u).\n" RESET, gamestate->cash);
            continue;
        }
        gamestate->cash -= gamestate->bet;
        gamestate->pot  += gamestate->bet;
        break;
    }
}

const char *get_suit_symbol(uint8_t suit_bits) {
    switch (suit_bits) {
        case 0x1: return HEART;    
        case 0x2: return CLUB;     
        case 0x4: return DIAMOND;  
        case 0x8: return SPADE;  
        default:  return "?";
    }
}

const char *rank_to_str(uint8_t rank) {
    static const char *rank_arr[13] = {
        "A", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "J", "Q", "K"
    };
    if (rank >= 1 && rank <= 13){
        return rank_arr[rank - 1];
    }
    else{
        return "?"; 
    }
}

void reset_cards(Player_Hand *player_hand, Dealer_Hand *dealer_hand, Deck *deck) {
    

    Card *lists[] = { player_hand->head, dealer_hand->head };

    for (int i = 0; i < 2; i++) {
        Card *curr = lists[i];
        while (curr) {
            Card *next = curr->next;
            curr->next = NULL;
            if (deck->tail) {
                deck->tail->next = curr;
            } else {
                deck->head = curr;
            }
            deck->tail = curr;
            deck->len++;
            curr = next;
        }
    }
    player_hand->head = player_hand->tail = NULL;
    player_hand->len = 0;

    dealer_hand->head = dealer_hand->tail = NULL;
    dealer_hand->len = 0;
}

void print_spacer(int lines) {
    for (int i = 0; i < lines; i++) {
        printf("\n");
    }
}

