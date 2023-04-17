#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <vector>
using namespace std;
#define ERROR(funcion) throw runtime_error(funcion + string(strerror(errno)))
//Declaro una estructura de tipo usuario
    struct Usuario {
        string id;
        string ip;
        string puerto;
    };

    //Definimos un vector de usuarios
    vector<Usuario> usuconects;
int configura_servidor(const char *puerto) {
struct addrinfo conf {
.ai_flags = AI_PASSIVE,
.ai_family = AF_UNSPEC,
.ai_socktype = SOCK_DGRAM
}, *resultado;
if(getaddrinfo(NULL, puerto, &conf, &resultado)) ERROR("geraddrinfo()");
int unsocket = -1;
while((unsocket = socket(resultado->ai_family,
resultado->ai_socktype, resultado->ai_protocol)) == -1 ) {
if(unsocket != -1) break;
cout << "socket() - " + string(strerror(errno)) << endl;
resultado = resultado->ai_next;
}
if(bind(unsocket, resultado->ai_addr, resultado->ai_addrlen))
ERROR("bind()");
return unsocket;
}
void procesa_mensaje(char paquete[], struct sockaddr_storage & dircliente, socklen_t longdircliente, int unsocket) {
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
   string comando = paquete_string.substr(0,4);
   string datos = paquete_string.substr(4);


   if(comando == "HELO"){
      Usuario newUser;
      newUser = {datos, strdireccion,to_string(ntohs(((struct sockaddr_in*)&dircliente)->sin_port))};
      usuconects.push_back(newUser);
      for(Usuario usuario : usuconects){
         string mensaje = newUser.id + "  se ha unido al chat " + "\n";
        sendto(unsocket, mensaje.c_str(), mensaje.length(), MSG_WAITALL,(struct sockaddr*)&dircliente,longdircliente);
      }
   }else if(comando == "SEND"){
    string id = " ";
    for(Usuario usuario : usuconects){
      if(usuario.ip == strdireccion){
        id = usuario.id;
        string mensaje = id + "  -> " + datos;
        for(Usuario usuario : usuconects){
            sendto(unsocket, mensaje.c_str(), mensaje.length(), MSG_WAITALL,(struct sockaddr*)&dircliente,longdircliente);
        }
      }
    }

   }else if(comando == "QUIT"){
       string id;
       string mensaje= " ";
       for( auto it = usuconects.begin(); it != usuconects.end(); ++it){
          if(it->ip == strdireccion){
            id = it->ip;
            mensaje = id + "  ha abandonado el chat  " + "\n";
              cout << id  <<  "ha abandonado el chat  " << endl;
            usuconects.erase(it);

           break;
          }
      }
    for(Usuario usuario :usuconects){
        sendto(unsocket, mensaje.c_str(), mensaje.length(), MSG_WAITALL,(struct sockaddr*)&dircliente,longdircliente);
    }

   }

}


int main(int argc, char *argv[]) {

const auto PUERTO_ECO = "32768";
    struct sockaddr_storage dircliente;

    socklen_t longdircliente;

    char *paquete = new char[IP_MAXPACKET];

    int misocket = configura_servidor(PUERTO_ECO), socketcliente;

    longdircliente = sizeof(dircliente);
    while(true) {
        auto bytes = recvfrom(misocket, paquete, IP_MAXPACKET, 0,
        (struct sockaddr *) &dircliente, &longdircliente);

        if(bytes == -1) ERROR("recvfrom()");
            paquete[bytes] = '\0';

            procesa_mensaje(paquete, dircliente,longdircliente,misocket);

            sendto(misocket, paquete, bytes, MSG_WAITALL,
            (struct sockaddr *) &dircliente, longdircliente);
    }

    close(misocket);
    return 0;
}
