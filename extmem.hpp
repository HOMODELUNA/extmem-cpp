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
#include <exception>
#include <stdexcept>

#include<iostream>
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
    //查找一个合适的块指针,如果有,返回之,没有,返回nullptr
    uint8_t* available_block_start()const;
    const size_t numAllBlk; /* buffer 中总共的块数 */
    unsigned long numIO = 0; /*  IO 的次数*/    
    const size_t buffer_size;
    size_t numFreeBlk; /* Number of available blocks in the buffer */
    std::unique_ptr<uint8_t[]> data; /* Starting address of the buffer */
};
namespace detail{
enum class Unsafe{YES};
}

constexpr auto UNSAFE = detail::Unsafe::YES;
enum class BlockState : unsigned char {
    AVAILABLE = 0,
    UNAVAILABLE = 1
};
// Block是buffer从数据的视图,本身不占有buffer中的信息,
// 因此请勿将block移出buffer的生存期,否则会造成悬垂引用.
// 我为了性能考虑,没有把buffer的底层视图变为共享指针.
struct Buffer::Block{

    Block(Block&& rv):original(rv.original){
        data = rv.data;
        rv.data = nullptr;
    }
    Block(const Block&) = delete;//请勿复制Block
    Block& operator=(const Block&) = delete; 
    
    ~Block(){
        if (data == nullptr){return;}
        *(data-1) = uint8_t(BlockState::AVAILABLE);
        original.numFreeBlk +=1;
    };

    size_t size()const{return original.block_size;}
    uint8_t* begin()const{return data;}
    uint8_t* end()const{return data + size();}
    uint8_t& operator[](size_t index){return data[index];}
    uint8_t operator[](size_t index)const{return data[index];}

    //
    /**
     * @brief 将它含有的内存视作某某类型的引用
     * 
     * 请不要在这里放有需要释放的资源,因为它底下不过是一片未解释的内存
     * 这个版本会进行运行时大小检查,如果要求的视图多于块的大小,那么会报错
     * 对无检查的版本,请用 <b> block.view<T,UNSAFE>() </b>
     * @tparam T 被视作的类型
     * @return T& 
     */
    template<typename T>
    T & view()const{
        if(sizeof(T) > size()){
            throw std::length_error("length exceeded when requiring a view");
        }
        return * reinterpret_cast<T*>(begin());
    }

    template<typename T,detail::Unsafe>
    T & view()const{
        return * reinterpret_cast<T*>(begin());
    }
    template<typename T,detail::Unsafe>
    T * ptr()const{
        return  reinterpret_cast<T*>(begin());
    }
    void backup(uint64_t addr)const;
    friend Buffer;
    private : 
    uint8_t * data = nullptr;
    Buffer& original;
    Block(Buffer& _original,uint8_t* _data):original(_original),data(_data){}
};





}


#endif // EXTMEM_HPP




