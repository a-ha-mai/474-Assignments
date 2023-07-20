#ifdef __cplusplus
 extern "C" {
#endif
#ifndef C_PROG2_H_INCLUDED
#define C_PROG2_H_INCLUDED

/*
 *      ECE 474 HW 3
 *
 *
 *      Student Name:Paria Naghavi
 *      Student ID:1441396
 *
 *      WRITE ANY NEW FUNCTION PROTOTYPES IN THIS FILE
 *
 *      Note: You will need to modify the definition of Person as well.
 *
 */
#include <stdio.h>
#include <stdint.h>

/*************************************************************************
   Part 2.1:  Bitwise operations
*************************************************************************/
long mangle(long);

int  bit_check(int, int, int);

/*************************************************************************
   Part 2.2:  Basic Pointer declaration and usage
*************************************************************************/
char * pmatch(char);

/*************************************************************************
   Part 2.3:  Pointer Arithmetic
*************************************************************************/
char nlet(char*);

int ldif(char, char);

/*************************************************************************
   Part 2.4   struct and sizeof
*************************************************************************/
// Modify this struct definition according to the instructions
// HINT: How long do each of these fields need to be?



typedef struct Person {
    char FirstName[21];   // Max 20 char string for FirstName (+1 for null character)
    char LastName[31];    // Max 30 char string for LastName (+1 for null character)
    char StreetAddr[81];  // Max 80 char string for StreetAddr (+1 for null character)
    char ZipCode[6];      // Max 5 char string for ZipCode (+1 for null character)
    double Height;        // Height in meters
    float Weight;         // Weight in kg
    long int DBirth;      // Birthday (days since 1-Jan-1900)
} Person;

int personSize(Person);
void float_to_ints(float, int*);
// Modify this definition
// NOTE: This is used to store the output string of per_print.
// How long does this need to be?
// the same as the size person struct- retuen value of personSize
char personbuf[151];
char* per_print(Person*, char*);
#endif // C_PROG2_H_INCLUDED
#ifdef __cplusplus
}
#endif


