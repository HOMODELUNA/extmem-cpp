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
    buffer_size(_buf_size),block_size(_block_size),numAllBlk(_buf_size / (_block_size + 1))
{
    numIO = 0;
    numFreeBlk = numAllBlk;
    data = std::unique_ptr<byte[]>(new byte [buffer_size]{byte{0} });
    if (!data){
        perror("Buffer Initialization Failed!\n");
    }
}

uint8_t* Buffer::available_block_start()const {
    for(auto blkPtr = data.get(); blkPtr < data.get() + (block_size + 1) * numAllBlk;blkPtr += block_size + 1) {
        if(*blkPtr == byte(BlockState::AVAILABLE)){
            return blkPtr+1;
        }   
    }
    return nullptr;
}

std::optional<Buffer::Block> Buffer::get_block(){
    if (numFreeBlk == 0)
    {
        perror("Buffer is full!\n");
        return std::nullopt;
    }

    auto blkPtr = available_block_start();
    if(!blkPtr){
        perror("No available buffer!\n");
        return std::nullopt;
    }
    *(blkPtr-1) = byte(BlockState::UNAVAILABLE);
    numFreeBlk--;
    return  std::optional( Block(*this,blkPtr))   ;
}

int Buffer::dump_block(Block block , uint64_t addr){

    char filename[40];

    sprintf(filename, "%d.blk", addr);
    std::ofstream out = std::ofstream(filename,std::ios::binary);

    if (!out)
    {
        perror("Writing Block Failed!\n");
        return -1;
    }
    out.write((char*)block.data,block_size);

        
    numIO++;
    out.close();
    return 0;
}
std::optional<Buffer::Block> Buffer::read_block(uint64_t addr){
    char filename[40];
    sprintf(filename, "%d.blk", addr);

    
    
    std::ifstream in (filename,std::ios::binary);

    if (!in){
        perror("Reading Block Failed!\n");
        return std::nullopt;
    }
    uint8_t* block_ptr = available_block_start();
    if(!block_ptr){
        perror("No available buffer!\n");
        return std::nullopt;
    }
    *(block_ptr-1) = byte(BlockState::UNAVAILABLE);
    in.read((char*)block_ptr,block_size);
    
    numFreeBlk--;
    numIO++;
    return std::optional(Block(*this,block_ptr));
}

void Buffer::Block::backup(uint64_t addr)const{
    char filename[40];

    sprintf(filename, "%d.blk", addr);
    std::ofstream out = std::ofstream(filename,std::ios::binary);

    if (!out)
    {
        perror("Writing Block Failed!\n");
    }

    out.write((char*)data,original.block_size);
    out.close();
}
}
