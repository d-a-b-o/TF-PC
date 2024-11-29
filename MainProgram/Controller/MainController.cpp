#include "MainController.h"

using namespace std;

MainController::MainController() { loadIndexTable(); }

int MainController::addProduct(Product &product) {
  product.ID_Product = indexTable.getCurrentSize() + 1;

  indexTable.insert(Data(product.ID_Product));
  binarySave.write(product);

  return product.ID_Product;
}

Product MainController::searchProduct(int &ID_Product) {
  Data found = indexTable.search(ID_Product);

  if (found.key != ID_Product) {
    throw runtime_error("Invalid id " + to_string(ID_Product));
  }

  return binarySave.read(found.key - 1);
}

void MainController::updateProduct(Product &product) {
  Data found = indexTable.search(product.ID_Product);

  if (found.key != product.ID_Product) {
    throw runtime_error("Invalid id" + to_string(product.ID_Product));
  }

  binarySave.overwrite(product);
}

void MainController::loadIndexTable() {
  fstream file("../Data/data.bin", ios::in | ios::binary);

  if (!file.is_open()) {
    return;
  }

  file.seekg(0, ios::end);
  streampos fileSize = file.tellg();
  int numRecords = fileSize / 250;
  file.seekg(0, ios::beg);

  const size_t blockSize = 100000;
  vector<char> buffer(blockSize * 250);

  for (size_t i = 0; i < numRecords; i += blockSize) {
    size_t recordsToRead = std::min(blockSize, numRecords - i);
    file.read(buffer.data(), recordsToRead * 250);

    for (size_t j = 0; j < recordsToRead; ++j) {
      const char *recordStart = buffer.data() + j * 250;
      string record(recordStart, 250);
      vector<string> cut = Tools::splitString(record, ';');
      string temp = cut[0].substr(4, 8);
      indexTable.insert(Data(stoi(temp)));
      binarySave.addNumRecords();
    }
  }
  file.close();
}
