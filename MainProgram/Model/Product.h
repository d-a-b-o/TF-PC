#pragma once
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/writer.h"
#include <iostream>
#include <string>

using namespace std;

struct Product {
  int ID_Product;
  string name;
  string description;
  double price;
  int stock;

  Product() : ID_Product(0) {}
  Product(int ID, string _name, string _description, double _price, int _stock)
      : ID_Product(ID), name(_name), description(_description), price(_price),
        stock(_stock) {}
};

class ProductJSON {
public:
  static string toJson(const Product &product) {
    rapidjson::Document doc;
    rapidjson::Value &obj = doc.SetObject();
    auto addMember = [&obj, &doc](const char *key, const string &value) {
      using String = rapidjson::GenericStringRef<char>;
      obj.AddMember(String{key}, String{value.c_str()}, doc.GetAllocator());
    };

    obj.AddMember("ID_Product", product.ID_Product, doc.GetAllocator());
    addMember("Name", product.name);
    addMember("Description", product.description);
    obj.AddMember("Price", product.price, doc.GetAllocator());
    obj.AddMember("Stock", product.stock, doc.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
  }
  static Product fromJson(const string &json) {
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    Product product;
    auto getString = [&doc](const char *key) { return doc[key].GetString(); };

    if (doc.HasMember("ID_Product")) {
      product.ID_Product = doc["ID_Product"].GetInt();
    }
    if (doc.HasMember("Name")) {
      product.name = doc["Name"].GetString();
    }
    if (doc.HasMember("Description")) {
      product.description = doc["Description"].GetString();
    }
    if (doc.HasMember("Price")) {
      product.price = doc["Price"].GetDouble();
    }
    if (doc.HasMember("Stock")) {
      product.stock = doc["Stock"].GetInt();
    }

    return product;
  }
};
