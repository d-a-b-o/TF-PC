#pragma once
#include <iostream>
#include <sys/types.h>
#include <vector>

using namespace std;

struct Data {
  u_int32_t key;
  u_int8_t numHash;
  Data() : key(0), numHash(0) {}
  Data(u_int32_t initKey) : key(initKey), numHash(0) {}
};

class CuckooHashTable {
private:
  static const int MAX_COLLISIONS = 100;
  vector<vector<Data>> tables;
  u_int8_t numTables = 2;
  int tableSize = 10;
  size_t currentSize = 0;
  hash<u_int32_t> hashFn;

  size_t hashData(const Data &data);
  void rehash();

public:
  CuckooHashTable();
  int getCurrentSize();
  void insert(Data data);
  Data search(const u_int32_t &key);
};
