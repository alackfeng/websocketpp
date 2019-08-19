
## 开发日志



#### 问题集锦
`````
Q1. Undefined symbols for architecture x86_64:
  "sdk::plugins::chunk_manager::chunk_manager()", referenced from:
      _main in chunk_manager.cpp.o
ld: symbol(s) not found for architecture x86_64

A1. 可能是class有没实现的方法，如 chunk_manager(); 实现即可 chunk_manager() {}



`````