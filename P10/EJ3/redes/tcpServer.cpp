#include <iostream>

#include <cstring>

#include <sys/socket.h>

#include <netinet/ip.h>

#include <arpa/inet.h>

#include <netdb.h>

#include <unistd.h>

#include <sstream>

#include <fstream>

using namespace std;

#define ERROR(funcion) throw runtime_error(funcion + string(strerror(errno)))

//Metodo configuracion servidor
int configura_servidor(const char * puerto) {
  struct addrinfo conf = {
    .ai_flags = AI_PASSIVE,
    .ai_family = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM
  }, * res;

  if (getaddrinfo(NULL, puerto, & conf, & res)) ERROR("geraddrinfo()");

  int unsocket = -1;

  while (unsocket = socket(res -> ai_family,
      res -> ai_socktype, res -> ai_protocol)) {
    if (unsocket != -1) break;
    cout << "socket() - " + string(strerror(errno)) << endl;
    res = res -> ai_next;
  }

  if (bind(unsocket, res -> ai_addr, res -> ai_addrlen)) ERROR("bind()");

  if (listen(unsocket, 10)) ERROR("listen()");

  return unsocket;
}

//Metodo notificacion de que se ha conectado el cliente todo esto se reproduce en el servidor
void notifica_servidor(struct sockaddr_storage * direccion) {

  char strdireccion[NI_MAXHOST + 1];

  inet_ntop(direccion -> ss_family,
    direccion -> ss_family == AF_INET ?
    (void * ) & ((struct sockaddr_in * ) direccion) -> sin_addr :
    (void * ) & ((struct sockaddr_in6 * ) direccion) -> sin6_addr,
    strdireccion, NI_MAXHOST);

  cout << "Conexión desde " << strdireccion << endl;
}

//Metodo que envia una respuesta al cliente
void envia_respuesta(int socketcliente, string contenido) {
  string respuesta = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(contenido.length()) + "\r\n\r\n" + contenido;
  //enviamos
  send(socketcliente, respuesta.c_str(), respuesta.length(), 0);
  //cerramos
  close(socketcliente);
}
//Extrae_peticion  del cliente
string extrae_peticion(int socketcliente) {

  char buffer[1024];
  //Recibe el mensaje que lo metemos en el buffer
  int lectura = recv(socketcliente, buffer, sizeof(buffer), 0);
  //Pasamos el buffer a cadena
  string mensajClient(buffer);
  //Sacamos la posicion del GET + 4 caracteres
  size_t pos1 = mensajClient.find("GET") + 4;
  //Busca la posicion del siguiente espacio en blanco despues de la ruta
  size_t pos2 = mensajClient.find_first_of(" ", pos1);
  //Todo esto para sacar la ruta que solocita el cliente por ejemplo /index.html
  string ruta_solicitada = mensajClient.substr(pos1, pos2 - pos1);

  return ruta_solicitada;
}
//Carga el archivo el nombre que se le pasa es el de la ruta
string carga_archivo(string nombre) {
  //Metodo que abre un archivo, lo lee su contenido lo almacena en un buffer y  devuelve lo almacenado en el buffer.
  string contenido;
  ifstream archivo(nombre.c_str());

  if (archivo.is_open()) {
    stringstream buffer;
    buffer << archivo.rdbuf();
    contenido = buffer.str();
    archivo.close();
  } else {
    contenido = "Archivo no encontrado";
  }
  return contenido;

}

int main(int argc, char * argv[]) {
    //Se suele utilizar del 1024 en adelante
  const auto PUERTO_TIME = "1025";
  struct sockaddr_storage dircliente;
  socklen_t longdircliente;
  //Configuramos el servidor
  int misocket = configura_servidor(PUERTO_TIME);

  while (true) {

    longdircliente = sizeof dircliente;
    //Aceptamos el cliente
    auto socketcliente = accept(misocket,(struct sockaddr * ) & dircliente, & longdircliente);
    //Si el socket no es igual a menos signfica que ha tenido conexion
    if (socketcliente != -1) {
        //Notificamos al servidor del cliente que se ha unido
      notifica_servidor( & dircliente);
      //Extraemos la peticion
      string peticion = extrae_peticion(socketcliente);
      //Obtenemos el contenido en dos casos
      string contenido;
        //En caso de que no haya  puesto la ruta completa
      if (peticion == "/") {
        contenido = carga_archivo("index.html");
        //Y en caso de que haya enviado la peticion directamente
      } else {
        contenido = carga_archivo(peticion);
      }
      //Enviamos la respuesta del servidor
      envia_respuesta(socketcliente, contenido);
    }
  }
  close(misocket);
  return 0;
}
