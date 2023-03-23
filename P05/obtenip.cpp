#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
using namespace std;

void muestraIP (struct addrinfo *direccion)
{
  char strdireccion[NI_MAXHOST + 1];
  inet_ntop (direccion->ai_family,
       direccion->ai_family == AF_INET ?
       (void *) &((struct sockaddr_in *) direccion->ai_addr)->sin_addr :
       (void *) &((struct sockaddr_in6 *) direccion->ai_addr)->
       sin6_addr, strdireccion, NI_MAXHOST);
  cout << strdireccion << endl;
}

int main(int argc, char *argv[]) {
	// Comprueba que se ha proporcionado un argumento
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " Introduce bien el host!" << endl;
        return 1;
    }

    struct addrinfo configuracion, *resultado;
    configuracion = {};
    configuracion.ai_family = AF_UNSPEC;
    configuracion.ai_socktype = SOCK_STREAM;
    
	// Obtiene la información de dirección para el argumento proporcionado
    int getaddrinfo_resultado = getaddrinfo(argv[1], NULL, &configuracion, &resultado);
    if (getaddrinfo_resultado != 0) {
    	// Lanza una excepción si hay un error en getaddrinfo
        throw runtime_error("Error introduce bien el host! " + string(gai_strerror(getaddrinfo_resultado)));
    }
    // Recorre todas las direcciones devueltas por getaddrinfo
    	//struct addrinfo  *direccion puntero de tipo addrinfo (Lo utilizamos para recorre la lista de direcciones de getaddrinfo)
    	//Comprobamos que no sea nulo para que asi podamos seguir recorriendo el puntero 
    	//Y direccion -> ai_next  actualizamos el valor del puntero con el siguiente 
     for (struct addrinfo *direccion = resultado; direccion != NULL; direccion = direccion->ai_next) {
        muestraIP(direccion);// Muestra la dirección IP
    }

    freeaddrinfo(resultado);// Libera la memoria asignada por getaddrinfo
    return 0;
}
