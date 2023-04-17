#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
using namespace std;
#define ERROR(funcion) throw runtime_error(funcion + string(strerror(errno)))
int configura_servidor(const char *puerto) {

    struct addrinfo conf {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_DGRAM
    }, *resultado;

    if(getaddrinfo(NULL, puerto, &conf, &resultado)) ERROR("geraddrinfo()");
    int unsocket = -1;
        while((unsocket = socket(resultado->ai_family,resultado->ai_socktype, resultado->ai_protocol)) == -1) {
            if(unsocket != -1) break;
            cout << "socket() - " + string(strerror(errno)) << endl;
            resultado = resultado->ai_next;
        }

            if(bind(unsocket, resultado->ai_addr, resultado->ai_addrlen))
                ERROR("bind()");

        return unsocket;
}
void informa_paquete(char paquete[], struct sockaddr_storage & dircliente) {
  char strdireccion[NI_MAXHOST + 1];
  inet_ntop(dircliente.ss_family,
    dircliente.ss_family == AF_INET ?
    (void * ) & ((struct sockaddr_in * ) & dircliente) -> sin_addr :
    (void * ) & ((struct sockaddr_in6 * ) & dircliente) -> sin6_addr,
    strdireccion, NI_MAXHOST);

  cout << "Recibidos " << strlen(paquete) << " bytes desde " <<
    strdireccion << endl <<
    '"' << paquete << '"' << endl;

}


int main(int argc, char *argv[]) {

const auto PUERTO_ECO = "32768";
    struct sockaddr_storage dircliente;

    socklen_t longdircliente;

    char *paquete = new char[IP_MAXPACKET];

    int misocket = configura_servidor(PUERTO_ECO), socketcliente;

    while(true) {
        auto bytes = recvfrom(misocket, paquete, IP_MAXPACKET, 0,
        (struct sockaddr *) &dircliente, &longdircliente);

        if(bytes == -1) ERROR("recvfrom()");
            paquete[bytes] = '\0';

            informa_paquete(paquete, dircliente);

            sendto(misocket, paquete, bytes, MSG_WAITALL,
            (struct sockaddr *) &dircliente, longdircliente);
    }

    close(misocket);
    return 0;
}
