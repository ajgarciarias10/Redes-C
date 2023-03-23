#include <iostream>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main() {
struct ifaddrs *ifap, *ifa;
if (getifaddrs(&ifap) != 0) {
cerr << "Error al obtener la información de las interfaces de red" << endl;
return 1;
}
// Recorre la lista de interfaces de red
for (ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {
    // Muestra el nombre de la interfaz
    cout << "Nombre de interfaz: " << ifa->ifa_name << endl;

    // Si la interfaz tiene una dirección IPv4
    if (ifa->ifa_addr->sa_family == AF_INET) {
        // Obtiene la dirección IPv4 como una cadena de texto
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr, addr, INET_ADDRSTRLEN);
        cout << "Dirección IPv4: " << addr << endl;

        // Obtiene la máscara de red IPv4 como una cadena de texto
        struct sockaddr_in *netmask = (struct sockaddr_in *) ifa->ifa_netmask;
        char netmask_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &netmask->sin_addr, netmask_str, INET_ADDRSTRLEN);
        cout << "Máscara de red IPv4: " << netmask_str << endl;
    }

    // Si la interfaz tiene una dirección IPv6
    if (ifa->ifa_addr->sa_family == AF_INET6) {
        // Obtiene la dirección IPv6 como una cadena de texto
        char addr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr, addr,   
        INET6_ADDRSTRLEN);
        cout << "Dirección IPv6: " << addr << endl;

        // Obtiene la máscara de red IPv6 como una cadena de texto
        struct sockaddr_in6 *netmask = (struct sockaddr_in6 *) ifa->ifa_netmask;
        char netmask_str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &netmask->sin6_addr, netmask_str, INET6_ADDRSTRLEN);
        cout << "Máscara de red IPv6: " << netmask_str << endl;
    }

    cout << endl;
}

// Libera la memoria de la estructura ifaddrs
freeifaddrs(ifap);

return 0;
}
