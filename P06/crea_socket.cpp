#include <stdexcept>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

using namespace std;

int main() {
    // Declaración de la variable "ifaddr"
    struct ifaddrs *ifaddr;
    // Declaramos el buffer
    char ip[INET_ADDRSTRLEN];
    int misocket;
    // Proceso de escucha
    // Creacion de socket Protocolo ipv4 socket de tipo raw
    if ((misocket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        throw runtime_error(strerror(errno));
    }
    cout << "Se ha creado el socket satisfactoriamente" << endl;
    // Obteniendo la dirección IP
    if (getifaddrs(&ifaddr) == -1) {
        throw runtime_error(strerror(errno));
    }
    /**
     * Recorremos las interfaces de red
     */
    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET) {
        //Comparamos si el nombre interfaz es lo en caso de que no lo sea hace lo de abajo
            if (strcmp(ifa->ifa_name, "lo") != 0) {
                // Obtiene la dirección IPv4 como una cadena de texto
                inet_ntop(AF_INET, &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr, ip, INET_ADDRSTRLEN);
            }
        }
    }
    freeifaddrs(ifaddr);
    cout << "Nombre: Manuel Portillo Arcos, Antonio José García Arias";
    cout << "Dirección IPv4: " << ip << endl;
    // Cierre del proceso de escucha
    close(misocket);
    return 0;
}
