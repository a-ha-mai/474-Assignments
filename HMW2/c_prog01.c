/*c_prog01.c
 *
 *      ECE474 introductory C programming assignment Part1
 *
 *     Student Name: Paria Naghavi
 *     Student ID#:  1441396
 *     Student Name: Anna Mai
 *     Student ID#:  215101
 */

/*  Objectives:
       Gain experience and proficiency with C programming.


Printing:   Use the following functions to print:
  print_int(int x)                prints out an integer
  print_usi(unsigned int x)       prints out an unsigned int
  print_newl()                    prints a "newline" character
  print_str(char *x)              prints a string (pointed to by x)
  print_dble(double)              prints out a double precision floating point number
*/

#include "c_prog1.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
/**********************************************************************
                 All variable declarations
**********************************************************************/

int i,j,k,l;      // these make good loop iterators.
int card, suit;

// Part 1.2
int t1, t2;

// Part 1.4
// HINT: Does this need to be modified?
char* st_buffer;

// Part 2.0
shuffle cards[52][2];

//  Part 2.2
unsigned char testhand[7]={0};

// Part 3.1
char * card_names[]={"Ace","2","3","4","5","6","7","8","9","10","Jack","Queen",
                        "King"};
char * suit_names[]={"Hearts","Diamonds","Clubs","Spades"};

// Part 3.2
int dealer_deck_count = 0;  // how many cards have been dealt from a deck

/******************************************************************************
 * 1.1  Write code to print out integers between 1 and N on separate lines
 *
 *      SAMPLE OUTPUT:
 *       1
 *       2
 *       3
 *       (continued)...
 *****************************************************************************/

void count(int N) {
  for(int i = 1; i <= N; i++) {
   // print the current number
    print_int(i);
    // to have the next number on a new line
    print_newl();
  }
}

/******************************************************************************
 * 1.2  Write code to print out integers between 1 and N on separate lines
 *
 *      SAMPLE OUTPUT:
 *       1
 *       2
 *       3
 *       (continued)...
 *       [sum(1,2,3 ... N)] [sum(1^2,2^2,3^2 ... N^2)]
 *****************************************************************************/
// print numbers from 1 to N and their sum and sum of their squares
void sums_and_squares1(int N) {
    int t1 = 0; // Variable for the sum of the numbers
    int t2 = 0; // Variable for the sum of the squares of the numbers

    // Loop from 1 to N
    for(int i = 1; i <= N; i++) {
        print_int(i); // Print the current number
        print_newl(); // Print a newline

        t1 += i;     // Add the current number to t1
        t2 += i * i; // Add the square of the current number to t2
    }

    // Print the final sum and sum of squares of the numbers from 1 to N
    print_int(t1);
    print_int(t2); print_newl();
}


/******************************************************************************
 * 1.3  Write code to print out integers between 1 and N on separate lines
 *
 *      SAMPLE OUTPUT:
 *      sum: [sum(1,2,3 ... N)]
 *      sum of squares: [sum(1^2,2^2,3^2 ... N^2)]
 *****************************************************************************/
// prints the sum and sum of squares for numbers 1-N labeled with
// text: “sum: ” and “sum of squares: ” on separate lines.
void sums_and_squares2(int N){
    char *label1 = "sum: ";
    char *label2 = "sum of squares: ";

    int sum = 0; // Variable for the sum of the numbers
    int sumOfSquares = 0; // Variable for the sum of the squares of the numbers

    // Loop from 1 to N
    for(int i = 1; i <= N; i++) {
        sum += i; // Add the current number to the sum
        sumOfSquares += i * i; // Add the square of the current number to sumOfSquares
    }

    // Print the final sums
    print_str(label1);
    print_int(sum);
    print_newl();

    print_str(label2);
    print_int(sumOfSquares);
    print_newl();
}


/******************************************************************************
 * 1.4  Write a function, char* length_pad(char *st, char* st_buffer, int n),
 *      which takes a string and adds the right amount of spaces so that it's
 *      length is n. It should return a pointer the new string.  If the length
 *      of st is greater than n, truncate the string.
 *      Note: Check the variable definitions above.
 *
 *      Modify 1.3 to use length_pad() so that the numbers are all starting in
 *      col 21 (i.e. all labels have length 20).
 *
 *      SAMPLE OUTPUT:
 *      sum:               [sum(1,2,3 ... N)]
 *      sum of squares:    [sum(1^2,2^2,3^2 ... N^2)]
 *****************************************************************************/


// This function takes a string st and a buffer st_buffer.
// It fills st_buffer with the first n characters of st, padded with whitespaces if st is shorter than n.
// If st is longer than n, it truncates it.
char* length_pad(char* st, char* st_buffer, int n) {
    // Initialize our index variable
    int i;

    //This loop copies characters from the original string "st" into the buffer "st_buffer".
    for(; i < n && st[i] != '\0'; i++) {
        st_buffer[i] = st[i];
    }


    //This loop fills in the remaining space in "st_buffer" with space characters (' '),
    //starting from where the previous loop left off.

    for( ; i < n; i++) {
        st_buffer[i] = ' ';
    }

    // Add a null terminator at the end of "st_buffer" to mark the end of the string
    st_buffer[i] = '\0';

    // Return the pointer (char*) to the padded string
    return st_buffer;
}

// Define the sums_and_squares3 function
void sums_and_squares3(int N){
    char *l1 = "sum: ";
    char *l2 = "sum of squares: ";

    int sum = 0; // Initialize sum
    int sumOfSquares = 0; // Initialize sum of squares

    // Calculate the sum and the sum of squares
    for(int i = 1; i <= N; i++) {
        sum += i;
        sumOfSquares += i * i;
    }

    char st_buffer[21]; // Buffer to hold the padded strings

    // Print the sum and sum of squares with the labels padded to 20 characters
    print_str(length_pad(l1, st_buffer, 20)); print_int(sum); print_newl();
    print_str(length_pad(l2, st_buffer, 20)); print_int(sumOfSquares); print_newl();
}


/******************************************************************************
 * 2.1 A 'shuffle' is an array of N_DECK pairs of integers. The first of the
 *     pair is the card type (0-13 representing ace, 2, 3, .... King) and the
 *     second representing the suit (hearts, diamonds, clubs, spades).   Thus a
 *     pair of numbers (1-13)(1-4) describes a unique card in the deck.
 *
 *     Write a function to fill a shuffle with N_DECK random integer pairs,
 *     BUT, as with your playing cards, there must be EXACTLY one of each pair
 *     in the shuffle. Use your function to print out all the "cards" of the
 *     shuffle, with 7 cards per line.
 *
 *     To generate a random number use the helper function int randN(int n)
 *     defined at the bottom of this file that returns a random integer between
 *     1 and N.
 *
 *     SAMPLE OUTPUT:
 [ 11   2 ]  [ 11   4 ]  [ 12   1 ]  [ 5   4 ]  [ 4   3 ]  [ 7   3 ]  [ 5   3 ]
 [ 13   4 ]  [ 9   3 ]  [ 2   3 ]  [ 1   1 ]  [ 2   4 ]  [ 3   2 ]  [ 2   1 ]
 [ 13   1 ]  [ 7   4 ]  [ 8   2 ]  [ 4   4 ]  [ 6   4 ]  [ 4   2 ]  [ 1   4 ]
 [ 7   1 ]  [ 3   3 ]  [ 12   2 ]  [ 6   1 ]  [ 12   4 ]  [ 13   3 ]  [ 9   2 ]
 [ 9   1 ]  [ 3   4 ]  [ 9   4 ]  [ 10   2 ]  [ 4   1 ]  [ 8   1 ]  [ 1   3 ]
 [ 3   1 ]  [ 11   3 ]  [ 8   4 ]  [ 10   3 ]  [ 5   1 ]  [ 10   1 ]  [ 13   2 ]
 [ 11   1 ]  [ 7   2 ]  [ 6   3 ]  [ 8   3 ]  [ 12   3 ]  [ 2   2 ]  [ 5   2 ]
 [ 1   2 ]  [ 10   4 ]  [ 6   2 ]
 *****************************************************************************/


void fill(int shuffle[N_DECK][2]) {
    // Initialize array with all possible cards
    //This line assigns a card type to each card.

    int cards[N_DECK][2];
    for (int i = 0; i < 52; i++) {
        cards[i][0] = i % 13 + 1; // Card type (1-13)
        cards[i][1] = i / 13 + 1; // Card suit (1-4)
    }

    // Shuffle the array
    srand(time(NULL)); // Initialize random seed
    for (int i = 51; i > 0; i--) {
    //This line is generating a random index j from 0 to i.
    // The function randN(n) generates a random number from 1 to n (inclusive),
    // so randN(i + 1) generates a random number from 1 to i+1 (inclusive).
    // We then subtract 1 to get a number from 0 to i, because our array is 0-indexed.*/
        int j = randN(i + 1) - 1; // Get a random index (subtract 1 because our array is 0-indexed)
        // Swap cards[i] and cards[j]
        //  j is a random index generated by the randN function,
        // where the value of j is between 0 and i, inclusive.
        //  Fisher-Yates shuffle
        int temp[2] = {cards[i][0], cards[i][1]};
        cards[i][0] = cards[j][0];
        cards[i][1] = cards[j][1];
        cards[j][0] = temp[0];
        cards[j][1] = temp[1];
    }
    // Copy the shuffled array to the output
    for (int i = 0; i < 52; i++) {
        shuffle[i][0] = cards[i][0];
        shuffle[i][1] = cards[i][1];
    }
    // Print the shuffle
    for(int i = 0; i < 52; i++) {
        printf("[ %d %d ] ", shuffle[i][0], shuffle[i][1]);
        if ((i + 1) % 7 == 0) { // New line every 7 cards
            printf("\n");
        }
    }
}



/******************************************************************************
 * 2.2 A 'hand' is an array of seven unsigned chars.  Each char represents one
 *     card.  We use a four bit field in the char for each of the two numbers
 *     above: the four most significant bits [7...4] represent the card number
 *     (1-13) and the lower four [3...0] represent the suit.
 *
 *     Write functions to:
 *     a) Convert two integers (from  a shuffle for example) into a char as
 *        above. If the ints are invalid (e.g. convert(294802984,138142) is not
 *        a card) return CARD_ERROR
 *     b) Test if a char equals a valid integer pair
 *     c) Get the integer suit from a char
 *     d) Get the integer card from a char
 *
 *     Both functions a and b must return CARD_ERROR if they get invalid input
 *     (such as suit > 4).
 *
 *     Write code for the functions convert(), valid_card(), gcard(), and
 *     gsuit() below.
 *****************************************************************************/


#define CARD_ERROR 0xFF // Use 0xFF as an error value

// Function to convert card and suit into an unsigned char
unsigned char convert(int card, int suit) {
    // Check for valid inputs
    if (card < 1 || card > 13 || suit < 1 || suit > 4) {
        return CARD_ERROR;
    }

 // Shift card left 4 bits and combine with suit using bitwise OR
    unsigned char result = (card << 4) | suit;
    return result;
}

// Function to check if a given char represents a valid card
int valid_card(unsigned char card) {
    // Extract the card and suit values
    int card_value = (card & 0xF0) >> 4; // Mask lower bits and shift right
    int suit_value = card & 0x0F; // Mask upper bits

    // Check if the values are within the valid ranges
    if (card_value < 1 || card_value > 13 || suit_value < 1 || suit_value > 4) {
        return CARD_ERROR;
    }
     // If we reach here, the card is valid
    return 0;
}

// Function to get card value
int gcard(unsigned char card) {
    // Extract the card value
    int card_value = (card & 0xF0) >> 4; // Mask lower bits and shift right
    // Check if the value is within the valid range
    if (card_value < 1 || card_value > 13) {
        return CARD_ERROR;
    }

    return card_value;
}

// Function to get suit value
int gsuit(unsigned char card) {
    // Extract the suit value
    int suit_value = card & 0x0F; // Mask upper bits
    // Check if the value is within the valid range
    if (suit_value < 1 || suit_value > 4) {
        return CARD_ERROR;
    }

    return suit_value;
}


/******************************************************************************
 * 3.1 Write a function names(int card, int suit, char answer[]) which places a
 *     string of the name and suit of a card in the array answer[]. For
 *     example: name(11,1) → “Jack of Hearts” name(8,2) → “8 of Diamonds”. Use
 *     the arrays defined above card_names[] and suit_names[]
 *
 *     Hint: Use pointers to copy the characters one-by-one into the array
 *     answer[] to build up the final string.
 *****************************************************************************/

void names(int card, int suit, char answer[]) {
    strcpy(answer, card_names[card - 1]); // copy card name to answer
    strcat(answer, " of ");               // concatenate " of " to answer
    strcat(answer, suit_names[suit - 1]); // concatenate suit name to answer
}

/******************************************************************************
 * 3.2 Write a function to deal a hand of M (0<M<8) cards from a shuffle. Use a
 *     global variable int dealer deck count to keep track of how many cards
 *     have been dealt from the deck.
 *
 *     To test this write a function printhand() that prints out a hand of
 *     cards
 *
 *     SAMPLE OUTPUT:
 *
 *     ----testing deal: hand:  0
 *     Deck count:  0
 *     --------dealt hand:
 *     the hand:
 *         Jack of Diamonds
 *         Jack of Spades
 *         Queen of Hearts
 *         5 of Spades
 *         4 of Clubs
 *         7 of Clubs
 *         5 of Clubs
 *
 *****************************************************************************/

void deal(int M, unsigned char hand[7], int deck[N_DECK][2]) {
     for (int i = 0; i < M; i++) {
        if (dealer_deck_count >= N_DECK) {
         // If the deck count exceeds the total number of cards in the deck,
            // set the hand element to indicate an error
            hand[i] = CARD_ERROR;
        // Otherwise, retrieve the card and suit from the deck
        // and convert them to card values using the convert() function
        } else {
            int card = deck[dealer_deck_count][0];
            int suit = deck[dealer_deck_count][1];
            hand[i] = convert(card, suit);
            dealer_deck_count++;
        }
    }
}

void printhand(int M, unsigned char* hand, char* buff1) {
    // Print header for the hand
    print_str("the hand:") ; print_newl();
    for (int i = 0; i < M; i++) {
    // If the card value indicates an error, print "Invalid Card"
        if (hand[i] == CARD_ERROR) {
            print_str("Invalid Card");
        // Otherwise, retrieve the card and suit values from the hand
        } else {
        // Use the names function to convert card and suit values to a string representation
            names(gcard(hand[i]), gsuit(hand[i]), buff1);
            // Print the card name
            print_str(buff1);
        }
        print_newl();
    }
}

/******************************************************************************
 * 3.3 Write functions to identify pairs of cards in a hand (two cards with the
 *     same number), three-of-a-kind (three cards with the same number), and
 *     four-of-a-kind (four cards with the same number).
 *****************************************************************************/
int pairs(int M, unsigned char hand[]) {
   int count = 0; // Initialize the count of pairs to 0
    for (int i = 0; i < M - 1; i++) {
        for (int j = i + 1; j < M; j++) {
    // If the card values of hand[i] and hand[j] are equal, increment the count
            if (gcard(hand[i]) == gcard(hand[j])) {
                count++;
            }
        }
    }
    return count;  // Return the count of pairs
}

int trip_s(int M, unsigned char hand[]) {
    int count = 0;
    for (int i = 0; i < M - 2; i++) {
        for (int j = i + 1; j < M - 1; j++) {
            for (int k = j + 1; k < M; k++) {
            // If the card values of hand[i], hand[j], and hand[k] are equal, increment the count
                if (gcard(hand[i]) == gcard(hand[j]) && gcard(hand[j]) == gcard(hand[k])) {
                    count++;
                }
            }
        }
    }
    return count;// Return the count of triplets
}

int four_kind(int M, unsigned char hand[]) {
    int count = 0;
    for (int i = 0; i < M - 3; i++) {
        for (int j = i + 1; j < M - 2; j++) {
            for (int k = j + 1; k < M - 1; k++) {
                for (int l = k + 1; l < M; l++) {
                // If the card values of hand[i], hand[j], hand[k], and hand[l] are equal, increment the count
                    if (gcard(hand[i]) == gcard(hand[j]) && gcard(hand[j]) == gcard(hand[k]) && gcard(hand[k]) == gcard(hand[l])) {
                        count++;
                    }
                }
            }
        }
    }
    return count;// Return the count of four of a kind
}
//  Predefined helper function to return a random integer between 1 and n
int randN(int n) {
    double x;
    x = 1.0 + (double) n * rand() / RAND_MAX;
    return((int)x);
}



