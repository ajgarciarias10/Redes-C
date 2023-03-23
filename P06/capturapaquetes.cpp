#include <cstring>
#include <iostream>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <iomanip>
using namespace std;
void muestra_payload(char *datos, size_t bytes) {
const auto ANCHO = 60;
stringstream lineahex, lineachar;
char punto = '.';
for(unsigned byte = 0; byte < bytes; byte++) {
    if(byte && ! (byte % 16)) {
        cout << left << setw(ANCHO) << lineahex.str()
        << lineachar.str() << endl;
        lineahex.str(string()); lineachar.str(string());
    }
    if(!byte || !(byte % 16))
        lineahex << setfill('0') << setw(4) << hex << byte;
    if(!(byte % 8)) lineahex << "- ";

        lineahex << setfill('0') << setw(2)
        << hex << (int )(unsigned char) datos[byte] << " ";
        lineachar << (datos[byte] >= 32 && datos[byte] < 128
        ? datos[byte] : punto);
    }
    cout << left << setw(ANCHO) << lineahex.str() << lineachar.str() << endl;
}
//Metodo creado para examinar la cabecera ip que damos en caso de que sea TCP
void examinaTCP(struct iphdr *cabeceraIP) {
//OBTENEMOS la direccion de origen de memoria  de la  cabecera TCP
//  Y se suma la
//  direccion de memoria de la cabeceraip + la longitud de la cabecera convertido en bytes

  struct tcphdr *cabeceraTCP = (struct tcphdr *)(cabeceraIP + cabeceraIP->ihl * 4);
  //Obtenemos lo que contiene la cabecera tcp
  cout << "Puerto de origen: " << ntohs(cabeceraTCP->source) << endl;
  cout << "Puerto de destino: " << ntohs(cabeceraTCP->dest) << endl;
  cout << "Número de secuencia: " << ntohl(cabeceraTCP->seq) << endl;
  cout << "Número de reconocimiento: " << ntohl(cabeceraTCP->ack_seq) << endl;
  //El receptor le indica al cluente cuántos bytes tiene libre el buffer
  cout << "Ventana de recepcion: " << ntohs(cabeceraTCP->window) << endl;
  //Suma de comprobacion
  cout << "Suma de comprobacion: " << ntohs(cabeceraTCP->check) << endl;
   /**
    * Sumamos el dataoffset del encabezado TCP especifica la longuitud del encabezado TCP en palabras de
    * 32 bits(Multiplos de 4 bytes) luego esto sumando con la direccion de memoria de
    * dicho encabezado, obtenemos la direccion del payload en su inicio
    */
  char *payload = (char *)cabeceraTCP + cabeceraTCP->doff * 4;
  int longitud_paquete = ntohs(cabeceraIP->tot_len);
  //Para obtener la longuitud de los datos del paquete
  // Se  hace lo restando la del paquete TCP -  longuitud del encabezado IP - TCP
  int longitud_datos = longitud_paquete - cabeceraIP->ihl * 4 - cabeceraTCP->doff * 4;
  muestra_payload(payload, longitud_datos);
}

void examina_frame(char * paquete, size_t bytes) {
    //Cogemos la cabecera ethernet
  struct ethhdr * cabeceraETH = (struct ethhdr * ) paquete;
  //Obtenemos la cabecera IP
  struct iphdr * cabeceraIP = (struct iphdr * )(paquete + sizeof(struct ethhdr));

  in_addr origen, destino;

  origen.s_addr = cabeceraIP -> saddr;

  destino.s_addr = cabeceraIP -> daddr;

  string protocolo;

  switch (cabeceraIP -> protocol) {
  case 1:
    protocolo = "ICMP";
    break;
  case 6:
    protocolo = "TCP";
    //Llamamos al metodo examina tcp
     examinaTCP(cabeceraIP);
    break;
  case 17:
    protocolo = "UDP";
    break;
  default:
    protocolo = to_string(cabeceraIP -> protocol);
  }
  //Sacamos la longitud total que es un tipo de dato short (16 byte) y la pasamos a formato host para asi mostrarlo
  uint16_t longtotal = ntohs(cabeceraIP -> tot_len);
  //La longuitud de la cabecera se muestra como un multiplo de 4
  uint8_t longcabecera = cabeceraIP -> ihl * 4;
  //TTL ACCEDIENDO A CABECERAIP
  uint8_t ttl = cabeceraIP -> ttl;
  cout << protocolo << "\t" <<
    inet_ntoa(origen) << "\t" <<
    inet_ntoa(destino) << "\t\t" <<
    "Longitud total: " << longtotal << " bytes\t" <<
    //Conversion a 16 bytes
    "Longitud cabecera: " << (uint16_t) longcabecera << " bytes\t" <<
    "TTL: " << (uint16_t) ttl << endl;
}


int main(int argc, char * argv[]) {
  char * paquete = new char[IP_MAXPACKET];
  struct sockaddr_ll origen;
  socklen_t long_origen = sizeof(origen);
  int misocket;
  int interfaz = stoi(argv[1]); // Número de interfaz
  // Socket para recibir paquetes IP desde la capa de enlace
  if (misocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP)); misocket == -1) {
    throw runtime_error(strerror(errno));
  }
  cout << "Prot." << "\t" << "IP origen" << "\t" <<
    "IP destino" << "\t\t" << "Bytes" << endl;
  // El programa no se dentendrá hasta ^C
  while (true) {
    auto bytes = recvfrom(misocket, paquete, IP_MAXPACKET, 0,
      (sockaddr * ) & origen, & long_origen);
    if (bytes == -1) {
      throw runtime_error(strerror(errno));
    }
    if (origen.sll_ifindex == interfaz)
      examina_frame(paquete, bytes);
  }

}
