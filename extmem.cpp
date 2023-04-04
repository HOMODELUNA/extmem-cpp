/*
 * extmem.h
 * Zhaonian Zou
 * Harbin Institute of Technology
 * Jun 22, 2011
 */


#include "extmem.hpp"
#include <memory>
#include <span>
#include <iostream>
#include <fstream>
namespace ExtMem{
using byte = uint8_t;

Buffer::Buffer(size_t _buf_size,size_t _block_size):
    buffer_size(_buf_size),block_size(_block_size),numAllBlk(buffer_size / (block_size + 1))
{
    numIO = 0;
    numFreeBlk = numAllBlk;
    data = std::unique_ptr<byte[]>(new byte [buffer_size]{byte{0} });
    if (!data){
        perror("Buffer Initialization Failed!\n");
    }
}

std::optional<Buffer::Block> Buffer::get_block(){
    if (numFreeBlk == 0)
    {
        perror("Buffer is full!\n");
        return std::nullopt;
    }

    auto blkPtr = data.get();

    while (blkPtr < data.get() + (block_size + 1) * numAllBlk)
    {
        if (*blkPtr == byte(BlockState::AVAILABLE))
            break;
        else
            blkPtr += block_size + 1;
    }

    *blkPtr = byte(BlockState::UNAVAILABLE);
    numFreeBlk--;
    return  std::optional( Block(*this,blkPtr + 1))   ;
}

int Buffer::dump_block(Block block , uint64_t addr){

    char filename[40];
    unsigned char *bytePtr;

    sprintf(filename, "%d.blk", addr);
    std::ofstream out = std::ofstream(filename);

    if (!out)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }

    for (auto c : block){
        out.put(c);
    }
        
    numIO++;
    out.close();
    return 0;
}
std::optional<Buffer::Block> Buffer::read_block(uint64_t addr){
    char filename[40];
    unsigned char *blkPtr, *bytePtr;
    char ch;

    if (numFreeBlk == 0)
    {
        perror("Buffer Overflows!\n");
        return std::nullopt;
    }

    blkPtr = data.get();

    while (blkPtr < data.get() + (block_size + 1) * numAllBlk)
    {
        if (*blkPtr == byte(BlockState::AVAILABLE))
            break;
        else
            blkPtr += block_size + 1;
    }

    sprintf(filename, "%d.blk", addr);
    FILE *fp = fopen(filename, "r");

    if (!fp)
    {
        perror("Reading Block Failed!\n");
        return std::nullopt;
    }

    *blkPtr = byte(BlockState::UNAVAILABLE);
    blkPtr++;
    bytePtr = blkPtr;

    while ((ch = fgetc(fp)) != EOF && bytePtr < blkPtr + block_size)
    {
        *bytePtr = ch;
        bytePtr++;
    }

    fclose(fp);
    numFreeBlk--;
    numIO++;
    return std::optional(Block(*this,blkPtr));
}


}
