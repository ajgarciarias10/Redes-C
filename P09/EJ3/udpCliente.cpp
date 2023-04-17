#include <iostream>

#include <cstring>

#include <sys/socket.h>

#include <netinet/ip.h>

#include <arpa/inet.h>

#include <unistd.h>

#include <signal.h>

#include <thread>

#include <chrono>

using namespace std;

#define ERROR(funcion) throw runtime_error(funcion + string(strerror(errno)))

const auto PUERTO_ECO = 32768;

bool salida = false;

void sigint_handler(int sig){
    salida = true;
}

void enviarHola(int socket_servidor,struct sockaddr_in destino, string mensaje_helo){

      if (sendto(socket_servidor, mensaje_helo.c_str(), mensaje_helo.length(), 0,
         (struct sockaddr * ) & destino, sizeof destino) == -1){
          ERROR("sendto()");
    }

}

void salirDelServidor(int socket_servidor,struct sockaddr_in destino){
  string mensaje_quit = "QUIT";
  if (sendto(socket_servidor, mensaje_quit.c_str(), strlen(mensaje_quit.c_str()), 0,
          (struct sockaddr * ) & destino, sizeof destino) == -1)
        ERROR("sendto()");

}

void recibir_mensajes(int socket_servidor, struct sockaddr_in destino, socklen_t longdestino) {
char * respuesta = new char[IP_MAXPACKET];
  while (!salida) {
    auto bytes = recvfrom(socket_servidor, respuesta, IP_MAXPACKET, 0,
      (struct sockaddr * ) & destino, & longdestino);
    if (bytes == -1) ERROR("recvfrom()");
    respuesta[bytes] = '\0';
    cout << respuesta << endl;

  }
}


int main(int argc, char * argv[]) {



  int socket_servidor = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_servidor == -1) ERROR("socket()");
  struct sockaddr_in destino {
    .sin_family = AF_INET,
      .sin_port = htons(PUERTO_ECO),
  };
  inet_aton(argv[1], & destino.sin_addr);
  socklen_t longdestino = sizeof destino;

  signal(SIGINT,sigint_handler);
string mensaje_helo = "HELO " + string(argv[2]);
  //Enviamos hola la 1º vez al servidor
  enviarHola(socket_servidor,destino,mensaje_helo);
  //Recibimos mensajes del servidor
  std::thread hilo_recibe(recibir_mensajes,socket_servidor,destino, longdestino);
   //Mientras no se produzca el control c
   while (!salida) {
    //Para el send
    string entradaTeclado;
    getline(cin, entradaTeclado);

    if(entradaTeclado == "QUIT"){

          salirDelServidor(socket_servidor,destino);
          salida = true;

    }else{
        string mensaje_send = "SEND " + entradaTeclado;
        if (sendto(socket_servidor, mensaje_send.c_str(), strlen(mensaje_send.c_str()), 0,
            (struct sockaddr * ) & destino, sizeof destino) == -1)
           ERROR("sendto()");
        this_thread::sleep_for(chrono::milliseconds(100));
    }


  }
  salirDelServidor(socket_servidor,destino);



   close(socket_servidor);
  return 0;
}
