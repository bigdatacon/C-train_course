#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <array>
using namespace std;

struct Nucleotide {
    char symbol;
    size_t position;
    int chromosome_num;
    int gene_num;
    bool is_marked;
    char service_info;
};

const static int Q = 4;
//набор букв
const std::array<char, Q> letters = { 'A','T','G','C' };

struct CompactNucleotide {
    uint32_t position;
    uint16_t gene_num : 15;
    uint8_t is_marked : 1;
    uint8_t chromosome_num : 6;
    uint8_t symbol : 2;
    char service_info;
};

CompactNucleotide Compress(const Nucleotide& n) {

    CompactNucleotide cn;

    cn.is_marked = uint8_t(n.is_marked);
    cn.chromosome_num = uint8_t(n.chromosome_num);
    cn.position = uint32_t(n.position);
    cn.gene_num = uint16_t(n.gene_num);


    for (uint8_t i = 0; i < Q; ++i) {

        if (letters[i] == n.symbol) {
            cn.symbol = uint8_t(i);
        }
    }

    cn.service_info = n.service_info;

    return cn;
}

Nucleotide Decompress(const CompactNucleotide& cn) {

    Nucleotide n;

    n.symbol = letters[cn.symbol];
    n.position = size_t(cn.position);
    n.chromosome_num = int(cn.chromosome_num);
    n.gene_num = int(cn.gene_num);
    n.is_marked = bool(cn.is_marked);
    n.service_info = cn.service_info;

    return n;
}

static_assert(sizeof(CompactNucleotide) <= 8, "Your CompactNucleotide is not compact enough"s);
static_assert(alignof(CompactNucleotide) == 4, "Don't use '#pragma pack'!"s);
bool operator==(const Nucleotide& lhs, const Nucleotide& rhs) {
    return (lhs.symbol == rhs.symbol) && (lhs.position == rhs.position) && (lhs.chromosome_num == rhs.chromosome_num)
        && (lhs.gene_num == rhs.gene_num) && (lhs.is_marked == rhs.is_marked) && (lhs.service_info == rhs.service_info);
}
void TestSize() {
    assert(sizeof(CompactNucleotide) <= 8);
}
void TestCompressDecompress() {
    Nucleotide source;
    source.symbol = 'T';
    source.position = 1'000'000'000;
    source.chromosome_num = 48;
    source.gene_num = 1'000;
    source.is_marked = true;
    source.service_info = '!';

    CompactNucleotide compressedSource = Compress(source);
    Nucleotide decompressedSource = Decompress(compressedSource);

    assert(source == decompressedSource);
}

int main() {
    TestSize();
    TestCompressDecompress();
}
