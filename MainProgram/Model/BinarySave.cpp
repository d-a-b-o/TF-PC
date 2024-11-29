#include "BinarySave.h"

using namespace std;

int BinarySave::getNumRecords() { return numRecords; }

void BinarySave::addNumRecords() { numRecords++; }

Product BinarySave::read(streampos position) {
  Product product;

  fstream file(DATA_ROUTE, ios::in | ios::binary);
  if (!file.is_open()) {
    return Product();
  }

  file.seekg(position * RECORD_SIZE);

  int sizeStr;
  file.read(reinterpret_cast<char *>(&sizeStr), sizeof(sizeStr));

  vector<char> buffer(sizeStr);
  file.read(buffer.data(), sizeStr);
  file.close();

  string record(buffer.begin(), buffer.end());

  vector<string> cut = Tools::splitString(record, ';');

  product.ID_Product = stoi(cut[0]);
  product.name = cut[1];
  product.description = cut[2];
  product.price = stod(cut[3]);
  product.stock = stoi(cut[4]);

  return product;
}

void BinarySave::write(Product &product) {
  fstream file(DATA_ROUTE, ios::out | ios::app | ios::binary);
  if (!file.is_open()) {
    return;
  }

  string record = to_string(product.ID_Product) + ";" + product.name + ";" +
                  product.description + ";" + to_string(product.price) + ";" +
                  to_string(product.stock) + ";";

  record += string(245 - record.length(), ' ') + ";";

  int sizeStr = record.size();

  file.write(reinterpret_cast<const char *>(&sizeStr), sizeof(sizeStr));
  file.write(record.c_str(), sizeStr);
  addNumRecords();

  file.close();
}

void BinarySave::overwrite(Product &product) {
  fstream file(DATA_ROUTE, ios::in | ios::out | ios::binary);
  if (!file.is_open()) {
    return;
  }

  string record = to_string(product.ID_Product) + ";" + product.name + ";" +
                  product.description + ";" + to_string(product.price) + ";" +
                  to_string(product.stock) + ";";

  record += string(245 - record.length(), ' ') + ";";

  int sizeStr = record.size();
  streampos position = product.ID_Product - 1;

  file.seekp(position * RECORD_SIZE);
  file.write(reinterpret_cast<const char *>(&sizeStr), sizeof(sizeStr));
  file.write(record.c_str(), sizeStr);

  file.close();
}
