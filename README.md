### 数据结构及简单算法应用库
#### AdvancedStructre
 **用于存储高等数据结构的部分**
 1. BTree——树相关算法
    1. BTree-c为C语言版本实现的B树部分，牺牲平衡度，降低部分树枝深度，提高查询速度。
    2. BRTree-CPlus为C++实现的红黑树部分。
    3. BTreepro为C++版本的B+树实现。
    4. BinaryTree为二叉搜索树结构。
 2. DirectGraph——有向图
 3. hash_mapping——hash相关算法
    1. crc32.h为单独的Crc32倒序查表法实现
    2. hash_base.h为基础哈希表
    3. hash_crc32.h为支持多线程、单线程的hash，后续整理
    4. hash_nolock.h为多级映射的无锁队列实现。
 4. memory——内存相关算法
    1. LRU是缓存算法，用来解决内存链式释放的问题。
    2. circle是圆形循环队列
    3. 链式内存池，但是有内存不连续的问题
 5. StringMatch——字符串匹配库
    1. kmp.h——C和CPlus的KMP算法
    2. AcAutomata.h——AC、BM和ACBM算法。

### Libtool
**各种工具库包括网络方面**
1. Network——网络工具
    1. FTP客户端        
    2. TCP客户端
    3. UDP服务器端、UDP流保活。
    4. Tools: kafka、yyjson使用
        
### SwordPointing
**算法相关**
1. sortAlgorithm——排序算法
   1. 快速排序、三项排序、归并排序、桶排序、
   2. 二分查找、插值查找、二叉树查找
   3. Fibonacci堆
2. leettask——算法题
   1. 回溯算法-拼正方形火柴、单调栈解最大元素、动态规划-上升子序列、俄罗斯套娃问题、书店最高收益问题。