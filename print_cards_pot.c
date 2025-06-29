#include <stdio.h>
#include <unistd.h> 
#include <string.h>

#include "colors.h"
#include "symbols.h"
#include "card_intialize.h"
#include "deck_free.h"
#include "print_cards_pot.h"

void clear_screen() {
    fflush(stdout);
    printf(clean_screen);
    fflush(stdout);
}

void print_pot_cash(GameState *gamestate, int checker){
    unsigned int *p_pot = &gamestate->pot;
    unsigned int *p_bet = &gamestate->bet;
    unsigned int *p_cash = &gamestate->cash;
    if (!p_pot || !p_cash || !p_bet) {
        printf("Error: NULL pointer passed.\n");
        return;
    }
    if (checker){
        for (int i = 1; i <= 4; i++) {
            print_pot_loop(gamestate,i,checker);
            sleep(1);
        }
    }
    else{
        print_pot_loop(gamestate,4,1);
    }
    if (*p_cash > 500){
        printf("Player's Cash: " GREEN "%-3u" RESET "\n", *p_cash);}
    else{
        printf("\nPlayer's Cash: " RED "%-3u" RESET "\n", *p_cash);}
    printf("Bet: %-3u\n", *p_bet);
    printf("\n");
}

void print_pot_loop(GameState *gamestate, int index, int checker){
    unsigned int *p_pot = &gamestate->pot;
    clear_screen();
    if (checker){
        printf("\t Pot: " YELLOW "%-3u" RESET "\n", *p_pot);
        switch (index) {
            case 1:
                printf("\t|"YELLOW"  %-3u"RESET"  |\n", *p_pot);
                printf("\t|       |\n");
                printf("\t|_______|\n");
                break;
            case 2:
                printf("\t|       |\n");
                printf("\t|"YELLOW"  %-3u"RESET"  |\n", *p_pot);
                printf("\t|_______|\n");
                break;
            case 3:
                printf("\t|       |\n");
                printf("\t|       |\n");
                printf("\t|_"YELLOW" %-3u"RESET"__|\n", *p_pot);
                break;
            case 4:
                printf("\t|       |\n");
                printf("\t|" YELLOW "$$$$$$$" RESET "|\n");
                printf("\t|" YELLOW "$$$$$$$" RESET "|\n");
                break;
            default:
                printf("Invalid index.\n");
                break;}
            }

}

void build_card_lines(char card_lines[CARD_HEIGHT][CARD_WIDTH + 1],const char *rank,const char *symbol,int hide){
    if (hide) {
        snprintf(card_lines[0], CARD_WIDTH + 1, "┌─────────┐");
        snprintf(card_lines[1], CARD_WIDTH + 1, "│ ?       │");
        snprintf(card_lines[2], CARD_WIDTH + 1, "│         │");
        snprintf(card_lines[3], CARD_WIDTH + 1, "│    ?    │");
        snprintf(card_lines[4], CARD_WIDTH + 1, "│         │");
        snprintf(card_lines[5], CARD_WIDTH + 1, "│       ? │");
        snprintf(card_lines[6], CARD_WIDTH + 1, "└─────────┘");
        return;
    }

    int is_red = (strcmp(symbol, HEART) == 0 || strcmp(symbol, DIAMOND) == 0);

    snprintf(card_lines[0], CARD_WIDTH + 1, "┌─────────┐");

    if (is_red) {
        snprintf(card_lines[1], CARD_WIDTH + 1, "│ " RED "%-2s" RESET "      │", rank);
        snprintf(card_lines[2], CARD_WIDTH + 1, "│         │");
        snprintf(card_lines[3], CARD_WIDTH + 1, "│   " RED " %s" RESET "    │", symbol);
        snprintf(card_lines[4], CARD_WIDTH + 1, "│         │");
        snprintf(card_lines[5], CARD_WIDTH + 1, "│      " RED "%-2s" RESET " │", rank);
    } else {
        snprintf(card_lines[1], CARD_WIDTH + 1, "│ %-2s      │", rank);
        snprintf(card_lines[2], CARD_WIDTH + 1, "│         │");
        snprintf(card_lines[3], CARD_WIDTH + 1, "│    %s    │", symbol);
        snprintf(card_lines[4], CARD_WIDTH + 1, "│         │");
        snprintf(card_lines[5], CARD_WIDTH + 1, "│       %-2s│", rank);
    }
    snprintf(card_lines[6], CARD_WIDTH + 1, "└─────────┘");
}

void print_deck_side_by_side_generic(GameState *gamestate,int hide_dealer_second_card, int show_dealer)      /* 1 = dealer, 0 = player     */
{
    char cards[MAX_DECK_CARDS][CARD_HEIGHT][CARD_WIDTH + 1];
    int  count = 0;
    Card *curr;
    if (show_dealer) {
        if (hide_dealer_second_card)
            printf("Dealer hand score - ?\n");
        else
            printf("Dealer hand score - %d\n",
                   calculate_score(&gamestate->dealercardlist));
        curr = gamestate->dealercardlist.head;
    } else {
        printf("Player hand score - %d\n",
               calculate_score(&gamestate->handcardlist));
        curr = gamestate->handcardlist.head;
    }
    while (curr && count < MAX_DECK_CARDS) {
        uint8_t byte  = curr->value;
        uint8_t rank  = (byte >> 4) & 0xF;
        uint8_t suit  =  byte & 0xF;
        const char *rank_str = rank_to_str(rank);
        const char *suit_sym = get_suit_symbol(suit);
        int hide = (hide_dealer_second_card && show_dealer && count == 1);
        build_card_lines(cards[count], rank_str, suit_sym, hide);
        curr = curr->next;
        ++count;
    }
    for (int line = 0; line < CARD_HEIGHT; ++line) {
        for (int i = 0; i < count; ++i) {
            printf("%s ", cards[i][line]);
        }
        putchar('\n');
    }
}