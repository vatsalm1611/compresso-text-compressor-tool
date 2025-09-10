/*
Author : vatsalm1611
Date : 2024-March-12
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "compression.h"

// Internal and Leaf nodes
#define LEAF 0
#define INTERNAL 1

typedef struct node
{
    char x;
    int freq;
    char *code;
    int type;
    struct node *next;
    struct node *left;
    struct node *right;

} node;

// Head and root nodes  in the linked list.
node *HEAD, *ROOT;

void printll();
void makeTree();
void genCode(node *p, char *code);
void insert(node *p, node *m);
void addSymbol(char c);
void writeHeader(std::ofstream &f);
void writeBit(int b, std::ofstream &f);
void writeCode(char ch, std::ofstream &f);
char *getCode(char ch);

node *newNode(char c)
{
    node *temp;
    temp = new node;
    temp->x = c;
    temp->type = LEAF;
    temp->freq = 1;
    temp->next = NULL;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

int main(int argc, char **argv)
{
    HEAD = nullptr;
    ROOT = nullptr;
    std::string inputFile, outputFile;
    if (argc <= 2)
    {
        std::cout << "\n***Automated File Compression***\n";
        std::cout << "\nCreating new compressed file...............\n";
        inputFile = argv[1];
        outputFile = inputFile + compressed_extension;
    }
    else
    {
        inputFile = argv[1];
        outputFile = argv[2];
    }

    std::ifstream fin(inputFile, std::ios::binary);
    if (!fin.is_open())
    {
        std::cout << "Error, Input file does not exists, Check the file name\n";
        return -1;
    }

    std::cout << "Initiating the compression sequence.................\n";
    std::cout << "Reading input file " << inputFile << "\n";
    char ch;
    while (fin.get(ch))
        addSymbol(ch);
    fin.close();

    std::cout << "Constructing Huffman-Tree....................\n";
    makeTree();
    std::cout << "Assigning codewords..........................\n";
    genCode(ROOT, (char *)"\0");

    std::cout << "Compressing the file.........................\n";
    fin.open(inputFile, std::ios::binary);
    if (!fin.is_open())
    {
        std::cout << "\n[!]Input file cannot be opened.\n";
        return -1;
    }
    std::ofstream fout(outputFile, std::ios::binary);
    if (!fout.is_open())
    {
        std::cout << "\n[!]Output file cannot be opened.\n";
        return -2;
    }

    std::cout << "\nReading input file " << inputFile << ".......................";
    std::cout << "\nWriting file " << outputFile << "........................";
    std::cout << "\nWriting File Header..............................";
    writeHeader(fout);
    std::cout << "\nWriting compressed content............................";
    while (fin.get(ch))
        writeCode(ch, fout);
    fin.close();
    fout.close();

    std::cout << "\n***Done***\n";
    return 0;
}

void writeHeader(std::ofstream &f)
{
    // mapping of codes to actual words
    codeTable record;
    node *p;
    int temp = 0, i = 0;
    p = HEAD;
    // Determine the uniwue symbols and padding of bits
    while (p != NULL)
    {
        temp += (strlen(p->code)) * (p->freq); // temp stores padding
        if (strlen(p->code) > MAX)
            printf("\n[!] Codewords are longer than usual.");
        temp %= 8;
        i++;
        p = p->next;
    }

    if (i == 256)
        N = 0; // if 256 diff bit combinations exist, then alias 256 as 0
    else
        N = i;

    f.write(reinterpret_cast<char *>(&N), sizeof(unsigned char));
    std::cout << "\nN=" << i;

    p = HEAD;
    // Start from the HEAD and wirte all character with its corresponding codes
    while (p != NULL)
    {
        record.x = p->x;
        strcpy(record.code, p->code);
        f.write(reinterpret_cast<char *>(&record), sizeof(codeTable));
        p = p->next;
    }

    padding = 8 - (char)temp; // int to char & padding = 8-bitsExtra
    f.write(reinterpret_cast<char *>(&padding), sizeof(char));
    std::cout << "\nPadding=" << (int)padding;
    // do actual padding
    for (i = 0; i < padding; i++)
        writeBit(0, f);
}

void writeCode(char ch, std::ofstream &f)
{
    char *code;
    // getting code of each character form the file
    code = getCode(ch);
    while (*code != '\0')
    {
        // Write into file 1 if the code is one
        if (*code == '1')
            writeBit(1, f);
        // Write 0 bit in file the corrensponding code is 0
        else
            writeBit(0, f);
        code++;
    }
    return;
}

// Function to wite the bit value to the file
void writeBit(int b, std::ofstream &f)
{ // My Logic: Maintain static buffer, if it is full, write into file
    static char byte;
    static int cnt;
    char temp;
    if (b == 1)
    {
        temp = 1;
        temp = temp << (7 - cnt); // right shift bits
        byte = byte | temp;
    }
    cnt++;

    if (cnt == 8) // buffer full
    {
        //		printf("[%s]",bitsInChar(byte));
        f.write(&byte, sizeof(char));
        cnt = 0;
        byte = 0; // reset buffer
        return;   // buffer written to file
    }
    return;
}

// getting correaponding code from the linked list, whose character is given
char *getCode(char ch)
{
    node *p = HEAD;
    while (p != NULL)
    {
        // LOGIC: check if the passed character matches with the linked list, if yes return its corresponding code
        if (p->x == ch)
            return p->code;
        p = p->next;
    }
    return NULL; // not found
}

// Inserting a node according to its freq in the linked list
void insert(node *p, node *m)
{ // insert p in list as per its freq., start from m to right,
    // we cant place node smaller than m since we dont have ptr to node left to m
    if (m->next == NULL)
    {
        m->next = p;
        return;
    }
    while (m->next->freq < p->freq)
    {
        m = m->next;
        if (m->next == NULL)
        {
            m->next = p;
            return;
        }
    }
    p->next = m->next;
    m->next = p;
}

// Adding the symbols to the linked list
void addSymbol(char c)
{ // Insert symbols into linked list if its new, otherwise freq++
    node *p, *q, *m;
    int t;

    if (HEAD == NULL)
    {
        HEAD = newNode(c);
        return;
    }
    p = HEAD;
    q = NULL;
    if (p->x == c) // item found in HEAD
    {
        p->freq += 1;
        if (p->next == NULL)
            return;
        if (p->freq > p->next->freq)
        {
            HEAD = p->next;
            p->next = NULL;
            insert(p, HEAD);
        }
        return;
    }

    while (p->next != NULL && p->x != c)
    {
        q = p;
        p = p->next;
    }

    if (p->x == c)
    {
        p->freq += 1;
        if (p->next == NULL)
            return;
        if (p->freq > p->next->freq)
        {
            m = p->next;
            q->next = p->next;
            p->next = NULL;
            insert(p, HEAD);
        }
    }
    else // p->next==NULL , all list traversed c is not found, insert it at beginning
    {
        q = newNode(c);
        q->next = HEAD; // first because freq is minimum
        HEAD = q;
    }
}

// Generating huffman tree
void makeTree()
{
    node *p, *q;
    p = HEAD;
    while (p != NULL)
    {
        q = newNode('@');
        q->type = INTERNAL; // internal node
        q->left = p;        // join left subtree/node
        q->freq = p->freq;
        if (p->next != NULL)
        {
            p = p->next;
            q->right = p; // join right subtree /node
            q->freq += p->freq;
        }
        p = p->next;   // consider next node frm list
        if (p == NULL) // list ends
            break;
        // insert new subtree rooted at q into list starting from p
        // if q smaller than p
        if (q->freq <= p->freq)
        { // place it before p
            q->next = p;
            p = q;
        }
        else
            insert(q, p); // find appropriate position
    } // while
    ROOT = q; // q created at last iteration is ROOT of h-tree
}

// Genreating Huffman codes of the characters
void genCode(node *p, char *code)
{
    char *lcode, *rcode;
    static node *s;
    static int flag;
    if (p != NULL)
    {
        // sort linked list as it was
        if (p->type == LEAF) // leaf node
        {
            if (flag == 0) // first leaf node
            {
                flag = 1;
                HEAD = p;
            }
            else // other leaf nodes
            {
                s->next = p;
            } // sorting LL
            p->next = NULL;
            s = p;
        }

        // assign code
        p->code = code; // assign code to current node
        //	printf("[%c|%d|%s|%d]",p->x,p->freq,p->code,p->type);
        std::string lcode = std::string(code) + "0";
        std::string rcode = std::string(code) + "1";
        // recursive DFS
        genCode(p->left, (char *)lcode.c_str());
        genCode(p->right, (char *)rcode.c_str());
    }
}
