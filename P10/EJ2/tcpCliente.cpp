
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
using namespace std;
void muestraIP (struct addrinfo *direccion,char servidor[])
{
  char strdireccion[NI_MAXHOST + 1];
        inet_ntop (direccion->ai_family,direccion->ai_family == AF_INET ?(void *) &((struct sockaddr_in *) direccion->ai_addr)->sin_addr :
       (void *) &((struct sockaddr_in6 *) direccion->ai_addr)->
       sin6_addr, strdireccion, NI_MAXHOST);

    struct sockaddr_in *ipv4 = (struct sockaddr_in*)direccion->ai_addr;
    const auto  port = ntohs(ipv4->sin_port);


string family = "";
const auto   familia = direccion->ai_family ;
   if(familia == 2){
   family ="AF_INET";
   }else{
    family = "AF_INET6";
   }
string tipoDsock= "";
const auto tipoDeSocket = direccion->ai_socktype;
if(tipoDeSocket == 1){
 tipoDsock= "SOCK_STREAM";
}else{
  tipoDsock= "SOCK_DGRAM";

}


  cout << "Ip:" << strdireccion << endl;
  cout << "Puerto:" << port << endl;
  cout << "Familia: " << family <<endl;
  cout << "Tipo de Socket: " <<  tipoDsock<<endl;


  //Utilizamos el socket
  int socket_server = socket(direccion->ai_family ,direccion->ai_socktype ,direccion->ai_protocol);
//Conexion con el servidor
    if(connect(socket_server, direccion->ai_addr,direccion->ai_addrlen) == -1) {
        close(socket_server);
        throw runtime_error(strerror(errno));
    }
    else {
    string buffer = "GET / HTTP/1.1\r\nHOST: " + string(servidor) + "\r\n\r\n";
    int enviado = send(socket_server, buffer.c_str(), buffer.length(), 0);
    if (enviado == -1) {
      cout << "Error" << endl;
    }
    char buffer_respuesta[1024];
    int respuesta = recv(socket_server,buffer_respuesta,sizeof(buffer_respuesta),0);
    cout<<buffer_respuesta<<endl;
         close(socket_server);





    }
}


int main(int argc, char *argv[]) {

const char *servicio = "http";
struct addrinfo configuracion = {};
configuracion.ai_family = AF_UNSPEC;
configuracion.ai_socktype = SOCK_STREAM;
configuracion.ai_protocol = IPPROTO_TCP;


//Lo almacenamos el resultado como un puntero en el utltimo parametro de getaddrinfo
struct addrinfo *resultado;
int estadoDeConex = getaddrinfo(argv[1],servicio,&configuracion,&resultado);

if(estadoDeConex!=-0){
    cout<< "Error al obtener informacion de esa direccion"<<endl;
 }

//Recorremos el puntero resultado para obtener toda la informacion de la direccion

for(struct addrinfo *puntero = resultado; puntero!=NULL; puntero = puntero->ai_next){
    //Caso de ser IPV4
      if (puntero->ai_family == AF_INET) {
         muestraIP(puntero,argv[1]);

     }

}
freeaddrinfo(resultado);


return 0;


}
