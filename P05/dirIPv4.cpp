#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

int main(int argc, char *argv[]) {
    struct in_addr unaIP;
    
    unaIP.s_addr = 1234567890;
    /*
      Hemos introducido IPresultante que va  almacenar en un buffer la direccion convertida
    */
    char IPresultante[INET_ADDRSTRLEN];
    cout << "Esta representación en binario:  " << 
    
    		unaIP.s_addr 
    		
    	<< " Se nota en Ipv4 como: " << 
    	     /*
    	     Cambiamos inet_ntoa por inet_ntop y introducimos nuevos parámetros
    	     			AF_INET constante utilizada para direcciones IPv4, 
    	     			llamamos al puntero unaIP(que apunta a in_addr), 
    	     			la variable IPresultante ,
    	     			y  el buffer de direcciones  como ultimo parámetro
        	*/
        	/*
        	Esta funcion lo que hace es  una conversion de la representacion binaria de una ip       
            a la representacion como caracter de la ip (de tipo IPv4)	
        	*/
			inet_ntop(AF_INET, &unaIP, IPresultante,INET_ADDRSTRLEN) 
			
         << endl;
    
    if(argc < 2) {
        cout << "Uso: " << argv[0] << " dirección IP" << endl;
        exit(-1);
    }
    
    struct in_addr dirIPv4;
        	     /*
    	     Cambiamos inet_aton por inet_pton 
    	     y introducimos nuevos parámetros
    	     			AF_INET utilizado para direcciones IPv4, 
    	     			argv[1] sería el segundo parametro pasado por consola,
    	     			&dirIPv4 seria el puntero donde vamos almacenar la respuesta en formato caracter
    	     			
        	*/
			/*
        		Esta funcion lo que hace es  una conversion de la representacion como caracter de la ip(de tipo IPv4) a 
        		la representacion binaria de la ip, si se produce el cambio (si es == a 1 ) ha tenido exito 
        	*/
    if(auto salida = inet_pton(AF_INET, argv[1], &dirIPv4) == 1) {
        //Mostramos El cambio
        cout << "La dirección IPv4 " << argv[1] << " se representa como " << dirIPv4.s_addr << endl;
    } else {
        cout << "La dirección " << argv[1] << " no parece válida" << endl;
    }
}
