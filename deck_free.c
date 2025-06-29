#include "deck_free.h"

void free_deck(void *deck) {
    Deck *d = (Deck *)deck;
    Card *curr = d->head;
    while (curr) {
        Card *next = curr->next;
        free(curr);
        curr = next;
    }
    d ->head = NULL;
    d ->tail = NULL;
    d ->len = 0;
}

void cleanup(GameState *gamestate) {
    free_deck(&gamestate->cardlist);
    free_deck(&gamestate->dealercardlist);
    free_deck(&gamestate->handcardlist);
}
