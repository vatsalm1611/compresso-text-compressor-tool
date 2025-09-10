

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "compression.h"

using namespace std;

std::vector<codeTable> codelist;
int n;
std::string decodeBuffer(char buffer);
std::string int2string(int n);
int match(const std::string &a, const std::string &b, int limit);
int fileError(std::ifstream &fp);

int main(int argc, char **argv)
{
    std::string inputFile, outputFile;
    if (argc <= 2)
    {
        std::cout << "***Huffman Decoding***\n";
        if (argc == 2)
        {
            inputFile = argv[1];
            outputFile = inputFile + decompressed_extension;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        inputFile = argv[1];
        outputFile = argv[2];
    }

    std::ifstream fin(inputFile, std::ios::binary);
    if (!fin.is_open())
    {
        std::cout << "[!]Input file cannot be opened.\n";
        return -1;
    }

    std::cout << "\n[Reading File Header]";
    unsigned char buffer;
    if (!fin.read(reinterpret_cast<char *>(&buffer), sizeof(unsigned char)))
        return (fileError(fin));
    N = buffer;
    if (N == 0)
        n = 256;
    else
        n = N;
    std::cout << "\nDetected: " << n << " different characters.";

    codelist.resize(n);
    if (!fin.read(reinterpret_cast<char *>(codelist.data()), sizeof(codeTable) * n))
        return (fileError(fin));
    std::cout << "\nReading character to Codeword Mapping Table";

    if (!fin.read(reinterpret_cast<char *>(&buffer), sizeof(char)))
        return (fileError(fin));
    padding = buffer;
    std::cout << "\nDetected: " << (int)padding << " bit padding.";

    std::ofstream fout(outputFile, std::ios::binary);
    if (!fout.is_open())
    {
        std::cout << "[!]Output file cannot be opened.\n";
        fin.close();
        return -2;
    }

    std::cout << "\n\n[Reading data]\nReplacing codewords with actual characters";
    while (fin.read(reinterpret_cast<char *>(&buffer), sizeof(char)))
    {
        std::string decoded = decodeBuffer(buffer);
        fout.write(decoded.c_str(), decoded.size());
    }
    fin.close();
    fout.close();
    std::cout << "\nEverything fine..\nOutput file " << outputFile << " written successfully.\n";
    return 0;
}

std::string decodeBuffer(char b)
{
    int i = 0, j = 0, t;
    static int k;
    static int buffer; // buffer larger enough to hold two b's
    std::string decoded;
    /*
    Logic:
    buffer = [1 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0]
              k
    b   =        [ 1 0 1 1 0 0 1 1 ]
    //put b in integer t right shift k+1 times then '&' with buffer; k=k+8;
    buffer = [1 0 0 1 1 0 1 1 0 0 1 1 0 0 0 0]
                    k
    */

    t = (int)b;
    t = t & 0x00FF;      // mask high byte
    t = t << 8 - k;      // shift bits keeping zeroes for old buffer
    buffer = buffer | t; // joined b to buffer
    k = k + 8;           // first useless bit index +8 , new byte added
    if (padding != 0)    // on first call
    {
        buffer = buffer << padding;
        k = 8 - padding; // k points to first useless bit index
        padding = 0;
    }

    // loop to find matching codewords

    while (i < n)
    {
        if (!match(codelist[i].code, int2string(buffer), k))
        {
            decoded += codelist[i].x;
            t = strlen(codelist[i].code); // matched bits
            buffer = buffer << t;         // throw out matched bits
            k = k - t;                    // k will be less
            i = 0;                        // match from initial record

            if (k == 0)
                break;
            continue;
        }
        i++;
    }

    return decoded;

} // fun

int match(const std::string &a, const std::string &b, int limit)
{
    std::string b_sub = b.substr(0, std::min((int)a.size(), limit));
    return a.compare(b_sub);
}

std::string int2string(int n)
{
    std::string temp;
    for (int i = 15; i >= 0; i--)
    {
        int andd = 1 << i;
        int k = n & andd;
        temp += (k == 0) ? '0' : '1';
    }
    return temp;
}

int fileError(std::ifstream &fp)
{
    std::cout << "[!]File read Error.\n[ ]File is not compressed using huffman.\n";
    fp.close();
    return -3;
}
