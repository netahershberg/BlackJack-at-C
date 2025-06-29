#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>   
#include "colors.h"
#include "symbols.h"
#include "card_intialize.h"
#include "print_cards_pot.h"
#include "deck_free.h"

void play(GameState *gamestate);
typedef void (*Action)(GameState *); 
void game_start(GameState *gamestate) { play(gamestate);}
void quit_game(GameState *gamestate)  { 
    printf(GREEN "\t\t\nyou Exit the game with %u at your cash wallet\n" RESET, gamestate->cash);
    puts(BLUE "\n\t\tGoodBye, See you next time" RESET);cleanup(gamestate); print_spacer(20);exit(0); }


typedef struct MenuItem {
    const char *description;
    Action action;
} MenuItem;

MenuItem main_menu[] = {
    {BLUE "Start Game" RESET, game_start},
    {RED "Exit" RESET, quit_game},
};

void run_menu(MenuItem *menu, size_t menu_len, GameState *gamestate);
void init_all_decks(GameState *gamestate);
void phase_two(int checker, GameState *gamestate);
void phase_three(int delear_card, int player_card, GameState *gamestate);
int calculate_score(void *deck_ptr);
int get_player_action();
void dealer_draw(GameState *gamestate);

int main(void) {
    srand(time(NULL));
    clear_screen();

    GameState gamestate_struct;
    GameState *pgamestate_struct = &gamestate_struct;
    
    // phase 1 - init decks, cash, and pot.
    init_all_decks(pgamestate_struct);
    fill_deck(pgamestate_struct);
    init_cash_and_pot(pgamestate_struct);

    run_menu(main_menu, sizeof main_menu / sizeof *main_menu, pgamestate_struct );
    
    return 0;
}
void init_all_decks(GameState *gamestate)
{
    init_deck(&gamestate->cardlist);
    init_deck(&gamestate->dealercardlist);
    init_deck(&gamestate->handcardlist);
}

void run_menu(MenuItem *menu, size_t menu_len, GameState *gamestate) {

    static int count_main = 1;   
    while(true){
        if (count_main) {
            puts(MAGENTA BOLD "Welcome to the blackjack game " RED HEART RESET SPADE RED DIAMOND RESET CLUB RESET );
            count_main = 0;  
        }else {
            puts(MAGENTA BOLD "\nDo you want to play again? " RED HEART RESET SPADE RED DIAMOND RESET CLUB RESET );
        }   
        size_t choise;
        int res;
        while (true) {
            for (size_t i = 0; i < menu_len; ++i) {
                printf("%zu) %s\n", i + 1, menu[i].description);
            }
            printf(ORANGE "Enter your choice: " RESET);
            res = scanf("%zu", &choise);
            clear_screen(); 
            if (res != 1) {
                printf( RED BOLD "invalid input" ORANGE "\nYou should choise by using number from the menu\n\n" RESET);
                scanf("%*[^\n]");
                continue;
            }
            --choise;
            if (choise >= menu_len) {
                printf(RED BOLD "invalid choise number\n\n" RESET);
                continue;
            }    
            menu[choise].action(gamestate);
            break;
        }
    }
}

void play(GameState *gamestate) {
    phase_two(1, gamestate); // Show pot animation
    phase_three(2, 2, gamestate); // draw 2 cards to dealer and player
    print_deck_side_by_side_generic(gamestate, 1, 1); // Dealer (hide 2nd card)
    print_deck_side_by_side_generic(gamestate, 0, 0); // Player

    int player_score = calculate_score(&gamestate->handcardlist);
    int dealer_score = calculate_score(&gamestate->dealercardlist);

    int player_blackjack = (player_score == 21 && gamestate->handcardlist.len == 2);
    int dealer_blackjack = (dealer_score == 21 && gamestate->dealercardlist.len == 2);

    // Both have Blackjack
    if (player_blackjack && dealer_blackjack) {
        clear_screen();
        phase_two(0, gamestate);
        print_deck_side_by_side_generic(gamestate, 0, 1);
        print_deck_side_by_side_generic(gamestate, 0, 0);
        printf(ORANGE "Both player and dealer have Blackjack! It's a tie.\n" RESET);
        set_cash_pot(gamestate, 0, 0);  // Keep pot
        reset_cards(&gamestate->handcardlist, &gamestate->dealercardlist, &gamestate->cardlist);
        return;
    }
    // Player has Blackjack
    if (player_blackjack) {
        clear_screen();
        phase_two(0, gamestate);
        print_deck_side_by_side_generic(gamestate, 0, 1);
        print_deck_side_by_side_generic(gamestate, 0, 0);
        printf(GREEN "Blackjack! You win with 21!\n" RESET);
        unsigned int payout = (unsigned int)(gamestate->pot * 2.5);
        set_cash_pot(gamestate, payout, 1); // 1 for reset pot
        reset_cards(&gamestate->handcardlist, &gamestate->dealercardlist, &gamestate->cardlist);
        return;
    }

    // Player Turn
    int player_action = get_player_action(); // 1 = Hit, 2 = Stand

    while (player_action == 1) {
        clear_screen();
        phase_two(0, gamestate);
        phase_three(0, 1, gamestate); // Draw 1 card
        print_deck_side_by_side_generic(gamestate, 1, 1);
        print_deck_side_by_side_generic(gamestate, 0, 0);

        player_score = calculate_score(&gamestate->handcardlist);
        if (player_score == 21) {
            printf(GREEN "You reached 21!\n" RESET);
            dealer_draw(gamestate);
            return;
        } else if (player_score > 21) {
            clear_screen();
            phase_two(0, gamestate);
            print_deck_side_by_side_generic(gamestate, 0, 1);
            print_deck_side_by_side_generic(gamestate, 0, 0);

            if (dealer_blackjack) {
                printf(RED "You bust and the dealer has Blackjack! Dealer wins.\n\n" RESET);
            } else {
                printf(RED "You got more than 21, you bust! Dealer won\n\n" RESET);
            }

            set_cash_pot(gamestate, 0, 1);
            reset_cards(&gamestate->handcardlist, &gamestate->dealercardlist, &gamestate->cardlist);
            return;
        }
        player_action = get_player_action();
    }

    // If player chose to stand with <= 21
    if (player_score <= 21 && player_action == 2) {
        dealer_draw(gamestate);
    }
}


void phase_two(int checker, GameState *gamestate){
    clear_screen();
    
    if (checker){
        put_bet(gamestate);
    }
    print_pot_cash(gamestate,checker);
    
}
    
void phase_three(int delear_card, int player_card, GameState *gamestate){
    
    // fill delear deck
    for(int i =0; i<delear_card; i++){
        int index = get_random_number(&gamestate->cardlist);
        Card *removed_card = card_remove_at(&gamestate->cardlist, index);
        if (removed_card != NULL) {
            card_push(&gamestate->dealercardlist, removed_card);
        }
    }
    // fill player deck
    for(int i =0; i<player_card; i++){
        int index = get_random_number(&gamestate->cardlist);
        Card *removed_card = card_remove_at(&gamestate->cardlist, index);
        if (removed_card != NULL) {
            card_push(&gamestate->handcardlist, removed_card);
        }
    }
}

int calculate_score(void *deck_ptr) {
    Deck *deck = (Deck *)deck_ptr;
    int score = 0;
    int ace_count = 0;

    for (Card *card = deck->head; card != NULL; card = card->next) {
        uint8_t rank = (card->value >> 4) & 0x0F; 
        if (rank == 1) {
            score += 11; 
            ace_count++;
        } else if (rank >= 11 && rank <= 13) {
            score += 10;    
        } else {
            score += rank; 
        }
    }
    while (score > 21 && ace_count > 0) {
        score -= 10;
        ace_count--;
    }
    return score;
}

int get_player_action() {
    int choice;
    int result;
    while (1) {
        printf(YELLOW "\nWhat would you like to do?:\n" RESET);
        printf(CYAN   "1) Hit\n" RESET);
        printf(CYAN   "2) Stand\n" RESET);
        printf(ORANGE   "Enter your choice (1 or 2): " RESET);
        result = scanf("%d", &choice);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        if (result != 1 || (choice != 1 && choice != 2)) {
            printf(RED "Invalid input. Please enter 1 or 2.\n" RESET);
        } 
        else 
        {
            return choice;
        }
    }
    
}

void dealer_draw(GameState *gamestate)
{
    int player_score  = calculate_score(&gamestate->handcardlist);
    int dealer_score  = calculate_score(&gamestate->dealercardlist);

    // Dealer already has better score, doesn't draw
    if (dealer_score >= 17 && dealer_score > player_score && dealer_score <= 21) {
        phase_two(0, gamestate);
        print_deck_side_by_side_generic(gamestate, 0, 1);
        print_deck_side_by_side_generic(gamestate, 0, 0);
        printf(RED "Dealer stands with %d - you lost.\n\n" RESET, dealer_score);
        set_cash_pot(gamestate, 0, 1); // lost, clear pot
        reset_cards(&gamestate->handcardlist, &gamestate->dealercardlist, &gamestate->cardlist);
        return;
    }

    // Dealer hits until reaching 17 or more
    while (dealer_score < 17) {
        phase_three(1, 0, gamestate); // dealer draws
        dealer_score = calculate_score(&gamestate->dealercardlist);
    }

    // Final reveal and comparison
    phase_two(0, gamestate);
    print_deck_side_by_side_generic(gamestate, 0, 1);
    print_deck_side_by_side_generic(gamestate, 0, 0);

    if (dealer_score > 21) {
        printf(GREEN "Dealer busts with %d - you won!\n" RESET, dealer_score);
        unsigned int payout = (unsigned int)(gamestate->pot * 2);       
        set_cash_pot(gamestate, payout, 1); // win, payout and reset pot
    }
    else if (dealer_score > player_score) {
        printf(RED "Dealer stands with %d - you lost.\n" RESET, dealer_score);
        set_cash_pot(gamestate, 0, 1); // lost, clear pot
    }
    else if (dealer_score == player_score) {
        printf(ORANGE "It's a tie!!\n" RESET);
        set_cash_pot(gamestate, 0, 0); // tie, keep pot
    }
    else {
        printf(GREEN "You win! Dealer %d vs you %d.\n" RESET, dealer_score, player_score);
        unsigned int  payout = (unsigned int)(gamestate->pot * 2);
        set_cash_pot(gamestate, payout, 1); // win, payout and reset pot
        }
    reset_cards(&gamestate->handcardlist, &gamestate->dealercardlist, &gamestate->cardlist);
}
