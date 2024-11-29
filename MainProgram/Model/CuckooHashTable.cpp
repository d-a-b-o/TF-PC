#include "CuckooHashTable.h"

using namespace std;

const Data EMPTY_SPACE = Data();

size_t CuckooHashTable::hashData(const Data &data) {
  return (hashFn(data.key) + data.numHash * hashFn(data.key) / tableSize) %
         tables[data.numHash].size();
}

void CuckooHashTable::rehash() {
  vector<vector<Data>> oldTables = tables;

  tables = vector(numTables, vector<Data>(tableSize *= 2, EMPTY_SPACE));

  currentSize = 0;

  for (auto &table : oldTables) {
    for (Data &data : table) {
      data.numHash = 0;
      insert(data);
    }
  }
}

CuckooHashTable::CuckooHashTable() {
  tables = vector(numTables, vector<Data>(tableSize, EMPTY_SPACE));
}

int CuckooHashTable::getCurrentSize() { return currentSize; }

void CuckooHashTable::insert(Data data) {
  Data found = search(data.key);
  int collisionCount = 0;

  if (found.key == data.key) {
    return;
  }

  do {
    size_t pos = hashData(data);
    Data current = tables[data.numHash][pos];
    tables[data.numHash][pos] = data;
    data = current;
    if (data.key == 0) {
      currentSize++;
      return;
    }

    if (data.numHash == numTables - 1) {
      data.numHash = 0;
    } else {
      data.numHash++;
    }

    collisionCount++;
  } while (collisionCount < MAX_COLLISIONS);

  rehash();
  insert(data);
}

Data CuckooHashTable::search(const u_int32_t &key) {
  Data aux = Data(key);
  size_t pos;

  aux.numHash = 0;
  pos = hashData(aux);
  if (tables[0][pos].key == key) {
    return tables[0][pos];
  }

  aux.numHash = 1;
  pos = hashData(aux);
  if (tables[1][pos].key == key) {
    return tables[1][pos];
  }

  return EMPTY_SPACE;
}
