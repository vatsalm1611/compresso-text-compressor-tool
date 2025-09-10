
# File Compression Using Huffman's Algorithm

## Overview


This project implements file compression and decompression using **Huffman's algorithm** in **modern C++ (C++17+)**. 
The program reads the input file, analyzes character frequencies, builds a **Huffman tree** using a **min-heap (priority queue)**, and replaces 
each character with a unique **binary codeword**. The process is **lossless**, meaning the original file can 
be perfectly reconstructed.

---

## Features

- Compresses text files using **Huffman coding** for efficient storage.
- Decompresses `.spd` files back to their original text form.
- Written in **modern C++ (C++17+)** using streams, containers, hashmaps (**unordered_map**), and a **min-heap** for Huffman tree construction.
- Uses a **min-heap (priority queue)** and **hashmaps** for efficient Huffman tree construction and fast symbol/code lookups.
- **Cross-platform**: tested on both Windows and Linux.

---

## Usage

### Compression
```
./encode <file-to-compress>
```
This produces a compressed file named `<file-to-compress>.spd`.

### Decompression
```
./decode <file-to-compress>.spd
```
This produces a decompressed file named `<file-to-compress>.txt`.

---

## File Structure

```
N (1 byte)               → Total number of unique characters
[	Character (1 byte)]      [Binary codeword string form (up to MAX bytes, where MAX is the maximum codeword length defined in the code)]
[	Character (1 byte)]      [Binary codeword string form (up to MAX bytes, where MAX is the maximum codeword length defined in the code)]
... repeated N times ...
p (1 byte)                → Padding count (0–7)
p bits of 0’s             → Padding bits (0’s) added to align with byte boundary
DATA                      → Encoded file data
```

*Note:* Padding ensures the encoded bit sequence aligns with full bytes. 
For example, 4 bytes + 3 bits will be padded to 5 bytes to maintain proper alignment.

---

## Example

Text: `aabcbaab`

| Content                           | Comment                               |
|-----------------------------------|---------------------------------------|
| 3                                 | N = 3 (a, b, c)                       |
| a "1"                             | character and corresponding code      |
| b "01"                            | character and corresponding code      |
| c "00"                            | character and corresponding code      |
| 4                                 | Padding count                         |
| [0000]                            | Padding (4 zeroes)                    |
| [1] [1] [01] [00] [01] [1] [1] [01]| Actual encoded data                   |

*Note: Code assignments may vary depending on implementation; this is one valid encoding.*

---

## Algorithm Overview

1. **First Pass:** Read the input file and build a frequency table for all characters.
2. Use a **min-heap (priority queue)** to efficiently combine the lowest-frequency nodes.
3. Construct the **Huffman tree** using the min-heap.
4. Traverse the tree to assign codewords to each character.
5. Write the mapping table (character → codeword) and padding info to the output file.
6. **Second Pass:** Read the input file again and write the codeword for each character to the output file.
7. The process is fully automated and implemented in modern C++ (using streams, containers, hashmaps, and idiomatic code).

---

## Contributing

Please feel free to submit issues and pull requests. Bug reports and testing on different platforms are appreciated. 
The code has been tested on both Linux and Windows.

---

## Author

**Vatsal Mishra**  
[GitHub](https://github.com/vatsalm1611) | [LinkedIn](https://www.linkedin.com/in/vatsalm1611)
