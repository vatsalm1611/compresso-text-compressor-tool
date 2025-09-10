/*
Author : vatsalm1611
Date : 2024-March-12
*/

#define MAX 16
// Padding is applied to ensure that the encoded bit sequence for each character aligns with full bytes.
// For example: 4 bytes + 3 bits will be treated as 5 bytes to maintain proper byte boundaries.
char padding;
unsigned char N;

// Code table regarding every character in the file
typedef struct codeTable
{
    char x;
    char code[MAX];
} codeTable;

char compressed_extension[] = ".spd";
char decompressed_extension[] = ".txt";
