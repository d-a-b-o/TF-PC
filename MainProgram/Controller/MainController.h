#pragma once
#include "../Model/BinarySave.h"
#include "../Model/CuckooHashTable.h"

#include <cstddef>
#include <string>

using namespace std;

class MainController {
private:
  CuckooHashTable indexTable;
  BinarySave binarySave;

public:
  MainController();
  int addProduct(Product &product);
  Product searchProduct(int &ID_Product);
  void updateProduct(Product &product);
  void loadIndexTable();
  int getNumRecords() { return binarySave.getNumRecords(); }
};
