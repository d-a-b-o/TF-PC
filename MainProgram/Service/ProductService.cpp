#include "../Controller/MainController.h"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/mime.h>
#include <pistache/net.h>
#include <pistache/router.h>

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

using namespace std;

class Service {
private:
  // Número del puerto donde el servidor correrá
  uint16_t m_portNum;
  // Número de hilos para manejar peticiones concurrentes
  unsigned int m_numThreads;
  // Direccion del servidor
  Pistache::Address m_address;
  // Servidor Http
  shared_ptr<Pistache::Http::Endpoint> m_endPoint;
  // Enrutador para definir rutas Http
  Pistache::Rest::Router m_router;

  // Controlador para manejar la logica relacionada con Productos
  MainController dbController;

  // Configura las rutas Http
  void configureRoutes();

  using Request = Pistache::Rest::Request;
  using Response = Pistache::Http::ResponseWriter;

  // Manejan las operaciones especificas sobre los Productos
  void getProduct(const Request &request, Response response);
  void updateProduct(const Request &request, Response response);
  void createProduct(const Request &request, Response response);
  void handleCors(const Request &request, Response response);

  void addCorsHeaders(Response &response);

public:
  Service(uint16_t portNum = 8000,
          unsigned int numThreads = thread::hardware_concurrency())
      : m_portNum(portNum), m_numThreads(numThreads),
        m_address("localhost", portNum),
        m_endPoint(make_shared<Pistache::Http::Endpoint>(m_address)) {}
  void run();
};

void Service::configureRoutes() {
  Pistache::Rest::Routes::Get(
      m_router, "/product/:ID_Product",
      Pistache::Rest::Routes::bind(&Service::getProduct, this));
  Pistache::Rest::Routes::Put(
      m_router, "/product/:ID_Product",
      Pistache::Rest::Routes::bind(&Service::updateProduct, this));
  Pistache::Rest::Routes::Put(
      m_router, "/product",
      Pistache::Rest::Routes::bind(&Service::createProduct, this));
  Pistache::Rest::Routes::Post(
      m_router, "/product",
      Pistache::Rest::Routes::bind(&Service::createProduct, this));

  Pistache::Rest::Routes::Options(
      m_router, "/*", Pistache::Rest::Routes::bind(&Service::handleCors, this));
}

void Service::addCorsHeaders(Response &response) {
  response.headers()
      .add<Pistache::Http::Header::AccessControlAllowOrigin>(
          "*") // Permitir cualquier origen
      .add<Pistache::Http::Header::AccessControlAllowMethods>(
          "GET, POST, PUT, DELETE, OPTIONS") // Métodos permitidos
      .add<Pistache::Http::Header::AccessControlAllowHeaders>(
          "Content-Type, Accept"); // Encabezados permitidos
}

void Service::handleCors(const Request &request, Response response) {
  addCorsHeaders(response);
  response.send(
      Pistache::Http::Code::Ok); // Respuesta vacía para solicitudes preflight
}

void Service::getProduct(const Request &request, Response response) {
  try {
    int ID_Product = request.param(":ID_Product").as<int>();
    Product product = dbController.searchProduct(ID_Product);

    addCorsHeaders(response);
    response.send(Pistache::Http::Code::Ok, ProductJSON::toJson(product),
                  MIME(Application, Json));
  } catch (const runtime_error &bang) {
    response.send(Pistache::Http::Code::Not_Found, bang.what(),
                  MIME(Text, Plain));
  } catch (...) {
    response.send(Pistache::Http::Code::Internal_Server_Error, "Internal error",
                  MIME(Text, Plain));
  }
}

void Service::updateProduct(const Request &request, Response response) {
  try {
    int ID_Product = request.param(":ID_Product").as<int>();
    string json = request.body();
    Product product = ProductJSON::fromJson(json);
    dbController.updateProduct(product);

    addCorsHeaders(response);
    response.send(Pistache::Http::Code::Ok,
                  "Product " + to_string(ID_Product) + " updated.",
                  MIME(Text, Plain));
  } catch (const runtime_error &bang) {
    response.send(Pistache::Http::Code::Not_Found, bang.what(),
                  MIME(Text, Plain));
  } catch (...) {
    response.send(Pistache::Http::Code::Internal_Server_Error, "Internal error",
                  MIME(Text, Plain));
  }
}

void Service::createProduct(const Request &request, Response response) {
  try {
    const string json = request.body();
    Product product = ProductJSON::fromJson(json);
    const int ID_Product = dbController.addProduct(product);

    addCorsHeaders(response);
    response.send(Pistache::Http::Code::Ok,
                  "Product " + to_string(ID_Product) + " created.",
                  MIME(Text, Plain));
  } catch (const runtime_error &bang) {
    response.send(Pistache::Http::Code::Not_Found, bang.what(),
                  MIME(Text, Plain));
  } catch (...) {
    response.send(Pistache::Http::Code::Internal_Server_Error, "Internal error",
                  MIME(Text, Plain));
  }
}

void Service::run() {
  cout << "Starting on port " << m_portNum << " with " << m_numThreads
       << " thread.\n";
  m_endPoint->init(Pistache::Http::Endpoint::options().threads(m_numThreads));
  configureRoutes();
  m_endPoint->setHandler(m_router.handler());
  m_endPoint->serve();
}

int main() {
  try {
    Service service;
    service.run();
  } catch (const exception &bang) {
    cerr << bang.what() << "\n";
    return 1;
  } catch (...) {
    return 1;
  }

  return 0;
}
