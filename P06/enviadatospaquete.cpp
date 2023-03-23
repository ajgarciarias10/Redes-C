#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
using namespace std;

struct paquete_ping {
struct iphdr cabeceraIP;
struct icmphdr cabeceraICMP;
char payload[32];
};


uint16_t suma_comprobacion(uint16_t * palabras, uint16_t bytes) {
uint32_t suma = 0;
while(bytes > 1) {
suma += *palabras++;
bytes -= 2;
}
if(bytes == 1) suma += *(uint8_t *) palabras;
suma = (suma >> 16) + (suma & 0xFFFF);
return ~(suma + (suma >> 16));
};

struct paquete_ping monta_paquete(
char * origen, char * destino, unsigned short n) {
struct paquete_ping ping = {
.cabeceraIP {
.ihl = 5,
.version = 4,
.tot_len = htons(sizeof(struct iphdr) +
sizeof(struct icmphdr) + 32),
.id = (unsigned short) rand(),
.frag_off = 0,
.ttl = 24,
.protocol = IPPROTO_ICMP,
.saddr = inet_addr(origen),
.daddr = inet_addr(destino)
},
.cabeceraICMP {
.type = ICMP_ECHO,
.code = 0,
.checksum = 0,
.un {
.echo {
.id = htons((unsigned short) getpid()),
.sequence = htons(n)
}
}
},
};
strcpy(ping.payload, "123-ABCDEFGHIJKLMNOPQRSTUVWXYZ");
ping.cabeceraICMP.checksum = suma_comprobacion(
(unsigned short *)&ping.cabeceraICMP,
sizeof ping.cabeceraICMP + 32);
return ping;
};




int main(int argc, char *argv[]) {

	struct timeval t1, t2;
    int misocketenv,misocketenv2;
    unsigned numseq = 1;
    if(argc < 3) throw runtime_error("Facilitar IP de origen y de destino");
    struct sockaddr_in destino = {};
    destino.sin_family = AF_INET;
    destino.sin_addr.s_addr = inet_addr(argv[2]);
    if(misocketenv=socket(AF_INET, SOCK_RAW, IPPROTO_RAW); misocketenv==-1) {
    throw runtime_error(strerror(errno));
    }
    if(misocketenv2=socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); misocketenv2==-1) {
    throw runtime_error(strerror(errno));
    }

    //Asignamos al bucle un limite de paquetes
    for(numseq=1; numseq<10; numseq++){
        struct paquete_ping ping = monta_paquete(argv[1], argv[2], numseq++);
        if(sendto(misocketenv, &ping, sizeof(struct paquete_ping), 0,
        (struct sockaddr *)&destino, sizeof(destino)) < 1) {
        throw runtime_error(strerror(errno));
        }
        
         if(sendto(misocketenv2, &ping, sizeof(struct paquete_ping), 0,
        (struct sockaddr *)&destino, sizeof(destino)) < 1) {
        throw runtime_error(strerror(errno));
        }
        
        // Obtenemos el tiempo transcurrido de envío 
        gettimeofday(&t1, NULL);

        struct sockaddr_in remitente;
        socklen_t remitente_longitud = sizeof(remitente);
        struct paquete_ping respuesta;

        if (recvfrom(misocketenv2, &respuesta, sizeof(respuesta), 0, (struct sockaddr *)&remitente, &remitente_longitud) > 0) {
            // La respuesta fue recibida, realiza las acciones necesarias
            // Puedes imprimir los datos de la respuesta recibida
            cout << "Respuesta recibida desde " << inet_ntoa(remitente.sin_addr) << endl;
            cout << "Origen: " << inet_ntoa(*(in_addr*)&respuesta.cabeceraIP.saddr) << endl;
            cout << "Destino: " << inet_ntoa(*(in_addr*)&respuesta.cabeceraIP.daddr) << endl;
            cout << "Número de secuencia: " << numseq << endl;
            // Obtenemos el tiempo transcurrido de respuesta
            gettimeofday(&t2, NULL);
            // Calculamos tiempo transcurrido en microsegundos
            double tiempo_transcurrido = (t2.tv_usec - t1.tv_usec);
			cout << "Tiempo transcurrido: " << tiempo_transcurrido << endl;
        }

        sleep(1);
    }

    close(misocketenv);
    return 0;
}
