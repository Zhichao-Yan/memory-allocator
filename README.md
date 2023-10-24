<!--
 * @Author: yan yzc53@icloud.com
 * @Date: 2023-10-22 13:51:09
 * @LastEditors: yan yzc53@icloud.com
 * @LastEditTime: 2023-10-24 14:07:04
 * @FilePath: /memory-allocator/README.md
 * @Description: Readme.md
 * @QQ: 1594047159@qq.com
 * Copyright (c) 2023,All Rights Reserved. 
-->
# memory-allocator
------------------
### What is it
memory-allocator is a very simple memory allocator

------------------
### What is in it
* `myallocator.h`
* `myallocator.c`

------------------
### What does it implement
* `void *mymalloc(size_t size)`
* `void myfree(void *block)`
* `void *mycalloc(size_t num, size_t nsize)`
* `void *myrealloc(void *block, size_t size)`
------------------
### How to build it as a shared library in Linux/Unix
* compile and link in one step
`gcc -o liballocator.so -fPIC -shared myallocator.c`
* compile and link in seperate steps
`gcc -fPIC -c myallocator.c`
`gcc -o liballocator.so -shared myallocator.o`
------------------
### How to use the shared library
1. compile main.c with the shared library
`gcc -o main main.c ./liballocator.so`
2. execute it
`./main`

`or`

1. compile main.c with the shared library
`gcc -o main1 main.c -L. -lmyallocator`
2. put `libmyallocator.so` in dynamic library search path like `/usr/local/lib`,`/lib`,`/usr/lib`
3. execute it
`./main`

------------------
### Reference Links
1. [man7.org](https://man7.org/linux/man-pages/man3/malloc.3.html)
2. [arjunsreedharan.org](https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory)

------------------



