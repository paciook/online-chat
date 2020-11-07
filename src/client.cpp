#include "header.h"
using namespace std;

// Funcion para establecer la conexion del cliente con el servidor.
// Devuelve el socket descriptor de la conexion

int connection_setup(){

    int socket_fd;
    int len;
    struct sockaddr_in remote;
    char buf[MENSAJE_MAXIMO];
    
    /* Crear un socket de dominio INET con TCP (SOCK_STREAM).  */
    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("creando socket");
        exit(1);
    }

    /* Establecer la dirección a la cual conectarse. */
    remote.sin_family = AF_INET;
    remote.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &(remote.sin_addr));

    /* Conectarse. */
    if (connect(socket_fd, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
        perror("conectandose");
        exit(1);
    }

    return socket_fd;
}


// Funcion principal de un cliente.
// El siguiente esquema puede servir como guia. Aprovechen las funciones de string!

int main(void){
    
    /* Conectarse al server (usando INET) */
    int socket_fd = connection_setup();
    
    /* Recibir el primer mensaje del servidor */
    char resp[MENSAJE_MAXIMO];

    leer_de_socket(socket_fd, resp);
    printf("%s\n", resp);

    /* Loop principal que envía mensajes al servidor */
    string msg;

    do {
        cout << "> ";
        getline(cin, msg);
        if(send(socket_fd, msg.data(), msg.length(), 0) < 0) {
            perror("Sending to server");
            exit(1);
        }
    }while(!feof(stdin));

    /* Cerrar el socket. */
    close(socket_fd);

    return 0;

}
