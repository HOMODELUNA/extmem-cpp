# ExtMem-C++ 

这是一个C++17 版本的ExtMem 库,用文件模拟磁盘块.之前C版的也有,但是关系不甚明晰.所以重构


我不知道这个东西除了写数据库实验外还有什么别的用途,但就这个用途,我会把它做好.
## 使用方法
不论是用那种方式,先把仓库刨到本地.
### xmake 
```
includes("仓库所在目录")
target("你的项目")
    add_deps("extmem)
```
在文件中:
```cpp
#include "extmem.hpp"

ExtMem::Buffer buffer(64,520);
```
`Buffer` 承担了大部分方法
### cmake
```
add_subdirectory(项目所在目录)
```
