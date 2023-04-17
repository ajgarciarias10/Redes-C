#include <iostream>

#include <cstring>

#include <sys/socket.h>

#include <netdb.h>

#include <unistd.h>

#include <netinet/ip.h>

#include <arpa/inet.h>

#include <vector>

#define ERROR(funcion) throw runtime_error(funcion + string(strerror(errno)))

using namespace std;

//Declaro una estructura de tipo usuario
struct Usuario {
  string id;
  sockaddr_storage direccion;
  socklen_t longdireccion;
};
//Socket a utilizar
int servSocket;

int configura_servidor(const char * puerto) {
  struct addrinfo conf {
      .ai_flags = AI_PASSIVE,
      .ai_family = AF_UNSPEC,
      .ai_socktype = SOCK_DGRAM
  }, * resultado;

  if (getaddrinfo(NULL, puerto, & conf, & resultado)) ERROR("geraddrinfo()");
  int primSocket = -1;
    while ((primSocket = socket(resultado->ai_family, resultado->ai_socktype, resultado->ai_protocol))) {
      if(primSocket != -1) break;
        cout << "socket() - " + string(strerror(errno)) << endl;
        resultado = resultado->ai_next;
    }
  if (bind(primSocket, resultado -> ai_addr, resultado -> ai_addrlen)) ERROR("bind()");
  return primSocket;

}
void procesa_mensaje(char paquete[], struct sockaddr_storage & dircliente, socklen_t longdircliente,vector<Usuario>&usuarios) {
  char strdireccion[NI_MAXHOST + 1];
  inet_ntop(dircliente.ss_family,
    dircliente.ss_family == AF_INET ?
    (void * ) & ((struct sockaddr_in * ) & dircliente) -> sin_addr :
    (void * ) & ((struct sockaddr_in6 * ) & dircliente) -> sin6_addr,
    strdireccion, NI_MAXHOST);

  cout << "Recibidos " << strlen(paquete) << " bytes desde " <<
    strdireccion << endl <<
    '"' << paquete << '"' << endl;

  string paquete_string(paquete);
  string comando = paquete_string.substr(0, 4);
  string datos = paquete_string.substr(4);
  string nombre = " ";
  if (comando == "HELO") {
    Usuario newUser;
    newUser = {datos,dircliente,longdircliente};
    usuarios.push_back(newUser);
    nombre = datos;
    //Añadimos cada usuario al la estructura de usuario
    for (auto& usuario: usuarios) {
      string mensaje_helo = "[Sistema] -> "+ datos + "  se ha unido al chat " + "\n";
      sendto(servSocket, mensaje_helo.c_str(), mensaje_helo.length(), MSG_WAITALL, (struct sockaddr * ) & usuario.direccion, usuario.longdireccion);
    }
  } else if (comando == "SEND") {
    string id = " ";
    char strdireccion_usuario[NI_MAXHOST + 1];
    for (auto it = usuarios.begin(); it != usuarios.end(); ++it) {
     inet_ntop(it->direccion.ss_family,
        it->direccion.ss_family == AF_INET ?
        (void*)&((struct sockaddr_in*)&it->direccion)->sin_addr :
        (void*)&((struct sockaddr_in6*)&it->direccion)->sin6_addr,
        strdireccion_usuario, NI_MAXHOST);
      if (string(strdireccion_usuario) == string(strdireccion)) {
        id = it -> id;
        break;
      }
    }
      string mensaje = id + "  -> " + datos;

     cout << id << "  -> " << datos << endl;

    for (auto& usuario: usuarios) {
       sendto(servSocket, mensaje.c_str(), mensaje.length(), MSG_WAITALL, (struct sockaddr * ) & usuario.direccion, usuario.longdireccion);
    }
  }
  else if (comando == "QUIT") {
    string id;
     string mensaje_ADIOS ="";
    char strdireccion_usuario[NI_MAXHOST + 1];
    for (auto it = usuarios.begin(); it != usuarios.end(); ++it) {
     inet_ntop(it->direccion.ss_family,
        it->direccion.ss_family == AF_INET ?
        (void*)&((struct sockaddr_in*)&it->direccion)->sin_addr :
        (void*)&((struct sockaddr_in6*)&it->direccion)->sin6_addr,
        strdireccion_usuario, NI_MAXHOST);
      if (string(strdireccion_usuario) == string(strdireccion)) {
        id = it -> id;
        mensaje_ADIOS = id + "  ha abandonado el chat  " + "\n";
        usuarios.erase(it);
        break;
      }
    }
     cout << id << " " << "ha abandonado el chat  " << endl;

    for (auto& usuario: usuarios) {
      sendto(servSocket, mensaje_ADIOS.c_str(), mensaje_ADIOS.length(), MSG_WAITALL, (struct sockaddr * ) & usuario.direccion, usuario.longdireccion);
    }
  }
}

int main(int argc, char * argv[]) {
//Cositas del servidor
  const auto PUERTO_ECO = "32768";
  struct sockaddr_storage dircliente;
  socklen_t longdircliente;
  char * paquete = new char[IP_MAXPACKET];
  //Cremos el socket del servidor
  servSocket = configura_servidor(PUERTO_ECO);
   //Definimos un vector de usuarios
   vector < Usuario > usuconects;

  while (true) {
    auto bytes = recvfrom(servSocket, paquete, IP_MAXPACKET, 0,
      (struct sockaddr * ) & dircliente, & longdircliente);
    if (bytes == -1) ERROR("recvfrom()");
    paquete[bytes] = '\0';

    //procesar mensaje recibido
    procesa_mensaje(paquete, dircliente, sizeof(dircliente),usuconects);
  }

  close(servSocket);
  return 0;
}

