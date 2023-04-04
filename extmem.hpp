/*
 * extmem.h
 * Zhaonian Zou
 * Harbin Institute of Technology
 * Jun 22, 2011
 */

#ifndef EXTMEM_HPP
#define EXTMEM_HPP

#include <memory>
#include<optional>
#include <cstdint>
namespace ExtMem{

struct Buffer{
    struct Block;
    Buffer(Buffer&&) = default;
    Buffer(const Buffer&) = delete;//请勿复制构造Buffer
    Buffer operator=(const Buffer&) = delete; 

    Buffer(size_t bufSize, size_t blkSize);
    //总共的块数
    size_t total_blocks()const{return numAllBlk;}
    //可用的块数
    size_t available_blocks()const {return numFreeBlk;}
    
    uint8_t* begin()const{return data.get();}
    uint8_t* end()const{return data.get() + buffer_size;}
    size_t size()const{return buffer_size;}
    uint8_t& operator[](int index){return data[index];}
    uint8_t operator[](int index)const{return data[index];}
    
    // 总共调用IO的次数
    size_t iotimes()const{return numIO;}
    
    /* 请求一个新块,当请求失败时,返回nullopt;
    */
    std::optional<Block> get_block();
    // 将块存储进磁盘,同时从buffer中去除
    // 
    // 注意: 这个方法需要消耗 Block
    int dump_block(Block block,uint64_t addr);
    std::optional<Block> read_block(uint64_t addr);
    
    const size_t block_size; /* 每个 Block 的大小*/
    protected:
    const size_t numAllBlk; /* buffer 中总共的块数 */
    unsigned long numIO = 0; /*  IO 的次数*/    
    const size_t buffer_size;
    size_t numFreeBlk; /* Number of available blocks in the buffer */
    std::unique_ptr<uint8_t[]> data; /* Starting address of the buffer */
};
enum class BlockState : unsigned char {
    AVAILABLE = 0,
    UNAVAILABLE = 1
};
// Block是buffer从数据的视图,本身不占有buffer中的信息,
// 因此请勿将block移出buffer的生存期,否则会造成悬垂引用.
// 我为了性能考虑,没有把buffer的底层视图变为共享指针.
struct Buffer::Block{

    Block(Block&&) = default;
    Block(const Block&) = delete;//请勿复制Block
    Block& operator=(const Block&) = delete; 
    
    ~Block(){
        *(data-1) = uint8_t(BlockState::AVAILABLE);
        original.numFreeBlk +=1;
    };

    size_t size()const{return original.block_size;}
    uint8_t* begin()const{return data;}
    uint8_t* end()const{return data + size();}
    uint8_t& operator[](size_t index){return data[index];}
    uint8_t operator[](size_t index)const{return data[index];}

    friend Buffer;
    private : 
    uint8_t * data;
    Buffer& original;
    Block(Buffer& _original,uint8_t* _data):original(_original),data(_data){}
};

}


#endif // EXTMEM_HPP






// unsigned char *getNewBlockInBuffer(Buffer *buf);

// /* Set a block in a buffer to be available. */
// void freeBlockInBuffer(unsigned char *blk, Buffer *buf);

// /* Drop a block on the disk */
// int dropBlockOnDisk(unsigned int addr);

// /* Read a block from the hard disk to the buffer by the address of the block. */
// unsigned char *readBlockFromDisk(unsigned int addr, Buffer *buf);

// /* Read a block in the buffer to the hard disk by the address of the block. */
// int writeBlockToDisk(unsigned char *blkPtr, unsigned int addr, Buffer *buf);
