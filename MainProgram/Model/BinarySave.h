#pragma once
#include "../Tools/Tools.h"
#include "Product.h"
#include <fstream>
#include <iosfwd>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

class BinarySave {
private:
  int numRecords;
  const string DATA_ROUTE = "../Data/data.bin";
  const int RECORD_SIZE = 250;

public:
  int getNumRecords();
  void addNumRecords();

  Product read(streampos pos);
  void write(Product &product);
  void overwrite(Product &product);
};
