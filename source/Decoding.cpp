#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "compression.h"

using namespace std;

std::unordered_map<std::string, char> codeToChar;
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

    std::vector<codeTable> codelist(n);
    if (!fin.read(reinterpret_cast<char *>(codelist.data()), sizeof(codeTable) * n))
        return (fileError(fin));
    std::cout << "\nReading character to Codeword Mapping Table";
    // Populate hashmap for fast codeword lookup
    for (const auto &entry : codelist)
    {
        codeToChar[std::string(entry.code)] = entry.x;
    }

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
    static int k = 0;
    static int buffer = 0;
    std::string decoded;
    int t = (int)b;
    t = t & 0x00FF;
    t = t << (8 - k);
    buffer = buffer | t;
    k = k + 8;
    if (padding != 0)
    {
        buffer = buffer << padding;
        k = 8 - padding;
        padding = 0;
    }

    // Incrementally build codeword from buffer bits and check hashmap
    while (k > 0)
    {
        std::string candidate;
        for (int i = 15; i >= 16 - k; --i)
        {
            int andd = 1 << i;
            int bit = buffer & andd;
            candidate += (bit == 0) ? '0' : '1';
            if (codeToChar.count(candidate))
            {
                decoded += codeToChar[candidate];
                int tlen = candidate.length();
                buffer = buffer << tlen;
                k -= tlen;
                break;
            }
        }
        // If no match found, break to read more bits
        break;
    }
    return decoded;
}

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
