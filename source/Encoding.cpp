/*
Author : vatsalm1611
Date : 2024-March-12
*/

#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "compression.h"

// Internal and Leaf nodes
#define LEAF 0
#define INTERNAL 1

struct node
{
    char x;
    int freq;
    std::string code;
    int type;
    node *left;
    node *right;
    node(char c) : x(c), freq(1), code(""), type(LEAF), left(nullptr), right(nullptr) {}
    node(node *l, node *r) : x('@'), freq(l->freq + r->freq), code(""), type(INTERNAL), left(l), right(r) {}
};

node *ROOT = nullptr;

void makeTree(std::unordered_map<char, node *> &nodes, std::vector<node *> &heapNodes);
void genCode(node *p, const std::string &code, std::unordered_map<char, node *> &nodes);
void addSymbol(char c, std::unordered_map<char, node *> &nodes);
void writeHeader(std::ofstream &f, const std::unordered_map<char, node *> &nodes);
void writeBit(int b, std::ofstream &f);
void writeCode(char ch, std::ofstream &f, const std::unordered_map<char, node *> &nodes);
std::string getCode(char ch, const std::unordered_map<char, node *> &nodes);

int main(int argc, char **argv)
{
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
        std::unordered_map<char, node *> nodes;

        std::vector<node *> nodes;
        char ch;
        while (fin.get(ch))
            addSymbol(ch, nodes);
        fin.close();

        std::vector<node *> heapNodes;
        for (auto &kv : nodes)
            heapNodes.push_back(kv.second);
        makeTree(nodes, heapNodes);
        std::cout << "Constructing Huffman-Tree....................\n";
        makeTree(nodes);
        genCode(heapNodes[0], "", nodes);
        if (!nodes.empty())
            genCode(nodes[0], "");

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
            writeHeader(fout, nodes);
        }
        writeHeader(fout, nodes);
        std::cout << "\nReading input file " << inputFile << ".......................";
        std::cout << "\nWriting file " << outputFile << "........................";
        while (fin.get(ch))
            writeCode(ch, fout, nodes);
        fin.close();
        fout.close();

        std::cout << "\n***Done***\n";
        return 0;
    }

    // ---- Function Definitions ----

    void writeHeader(std::ofstream & f, const std::vector<node *> &nodes)
    {
        int temp = 0, i = 0;
        for (const auto &p : nodes)
        {
            temp += static_cast<int>(p->code.length()) * (p->freq);
            if (p->code.length() > MAX)
                std::cout << "\n[!] Codewords are longer than usual.";
            i++;
        }
        temp %= 8;
        if (i == 256)
            N = 0;
        else
            N = i;
        for (const auto &p : nodes)
        {
            record.x = p->x;
            strncpy(record.code, p->code.c_str(), sizeof(record.code));
            f.write(reinterpret_cast<char *>(&record), sizeof(codeTable));
        }
        char padding = 8 - static_cast<char>(temp);
        f.write(reinterpret_cast<char *>(&padding), sizeof(char));
        std::cout << "\nPadding=" << (int)padding;
        for (i = 0; i < padding; i++)
            writeBit(0, f);
    }

    void writeCode(char ch, std::ofstream &f, const std::vector<node *> &nodes)
    {
        std::string code = getCode(ch, nodes);
        for (char bit : code)
        {
            writeBit(bit == '1' ? 1 : 0, f);
        }
    }

    void writeBit(int b, std::ofstream &f)
    {
        static char byte = 0;
        static int cnt = 0;
        char temp;
        if (b == 1)
        {
            temp = 1;
            temp = temp << (7 - cnt);
            byte = byte | temp;
        }
        cnt++;
        if (cnt == 8)
        {
            f.write(&byte, sizeof(char));
            cnt = 0;
            byte = 0;
        }
    }

    std::string getCode(char ch, const std::vector<node *> &nodes)
    {
        for (const auto &p : nodes)
        {
            if (p->x == ch)
                return p->code;
        }
        return "";
    }

    void addSymbol(char c, std::vector<node *> &nodes)
    {
        for (auto &n : nodes)
        {
            if (n->x == c)
            {
                n->freq++;
                return;
            }
        }
        nodes.push_back(new node(c));
    }

    void makeTree(std::vector<node *> & nodes)
    {
        auto cmp = [](node *a, node *b)
        { return a->freq > b->freq; };
        std::priority_queue<node *, std::vector<node *>, decltype(cmp)> minHeap(cmp, nodes);

        while (minHeap.size() > 1)
        {
            node *left = minHeap.top();
            minHeap.pop();
            node *right = minHeap.top();
            minHeap.pop();
            node *parent = new node(left, right);
            minHeap.push(parent);
        }
        if (!minHeap.empty())
        {
            nodes.clear();
            nodes.push_back(minHeap.top());
            ROOT = minHeap.top();
        }
    }

    void genCode(node * p, const std::string &code)
    {
        if (!p)
            return;
        if (p->type == LEAF)
        {
            p->code = code;
            return;
        }
        genCode(p->left, code + "0");
        genCode(p->right, code + "1");
    }

    // (Removed unreachable/duplicate code after last function definition)
    auto cmp = [](node *a, node *b)
    { return a->freq > b->freq; };
    std::priority_queue<node *, std::vector<node *>, decltype(cmp)> minHeap(cmp, nodes);

    while (minHeap.size() > 1)
    {
        node *left = minHeap.top();
        minHeap.pop();
        node *right = minHeap.top();
        minHeap.pop();
        node *parent = new node(left, right);
        minHeap.push(parent);
    }
    if (!minHeap.empty())
    {
        nodes.clear();
        nodes.push_back(minHeap.top());
        ROOT = minHeap.top();
    }
}

void genCode(node *p, const std::string &code)
{
    if (!p)
        return;
    if (p->type == LEAF)
    {
        p->code = code;
        return;
    }
    genCode(p->left, code + "0");
    genCode(p->right, code + "1");
}
