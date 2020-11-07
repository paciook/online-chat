#include "header.h"
using namespace std;

/* Dado un socket, un nickname y el estado de login, registra un nuevo cliente con el nickname dado si el 
   mismo no se encuentra en uso. 
   En caso contrario, envia un mensaje indicando la falla. Además, actualiza 
   la variable log con el nuevo estado de login del cliente. */
Client addClient(int s, const string& nickname){

    Client newClient;

    newClient.socket_id = s;
    newClient.nickname = nickname;

    return newClient;
}


/* Dado un cliente, lo elimina del sistema y cierra su socket adecuadamanete(ver shutdown()) */
void deleteClient(Client& c){

   /* COMPLETAR */
    return;
}

/* Dado un nick, devuelve un puntero al cliente encontrado con dicho nickname. En caso de no existir,
   el puntero es NULL */
Client* getClient(const string& nick) {
    return NULL;
}

/* Dado un cliente y un mensaje, envía dicho mensaje a traves del socket asociado al cliente */
void send(Client* c, const string& msg) {
    
    if(send(c->socket_id, msg.data(), strlen(msg.data()), 0) < 0)
        perror("enviando mensaje");

    return;

}

/* Dado un socket y un mensaje, envía dicho mensaje a traves del socket asociado al cliente */
void send(int socket_id, const string& msg) {
    
    if(send(socket_id, msg.data(), msg.length(), 0) < 0)
        perror("enviando mensaje");

    return;

}


/* Funcion que ejecutan los threads */
void connection_handler(int socket_desc){

    send(socket_desc, "Ingrese nombre de usuario");

    char resp[MENSAJE_MAXIMO];

    leer_de_socket(socket_desc, resp);
    
    string str;

    str += resp;

    while(getClient(str) != NULL){
        send(socket_desc, "Nombre ya registrado, utilice otro");
        
        leer_de_socket(socket_desc, resp);
        str = "";
        str += resp;
    }
    
    Client oCliente = addClient(socket_desc, str);

    /* Main loop */
    while(1) {

        /* leer socket, salir si hubo error*/
        if(leer_de_socket(oCliente.socket_id, resp) == -1)
            break;

        if(resp[0] == '-')
            printf("CLAAVE\n");

        printf("[%s]: %s\n", oCliente.nickname.data(), resp);

        /* Parsear el buffer recibido*/
        /* COMPLETAR */

        /* Detectar el tipo de mensaje (crudo(solo texto) o comando interno(/..),
           y ejecutar la funcion correspondiente segun el caso */
        /* COMPLETAR */

    }
   
    return;  
}


int connection_setup(){

	int listening_socket;
    struct sockaddr_in local, remote;

    /* Crear un socket de dominio INET con TCP (SOCK_STREAM).  */
    if ((listening_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("creando socket");
        exit(1);
    }

    /* Establecer la dirección a la cual conectarse para escuchar. */
    local.sin_family = AF_INET;
    local.sin_port = htons(PORT);
    local.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening_socket, (struct sockaddr *)&local, sizeof(local)) < 0) {
        perror("haciendo bind");
        exit(1);
    }

    /* Escuchar en el socket y permitir 5 conexiones en espera. */
    if (listen(listening_socket, 5) == -1) {
        perror("escuchando");
        exit(1);
    }

    printf("Server is on! \n");
    return listening_socket;
}


int main(void)
{
      

    // Abrimos un socket para escuchar conexiones entrantes
    int s = connection_setup();
  
    std::vector<int> vSockets;

    int i = 0;
    int s1;
    thread threads[MAX_CLIENTS];

    while(1) {  

        // Main loop del servidor
        // Aqui se aceptan conexiones y handlea a cada cliente a partir de un thread

        if ((s1 = accept(s, NULL, NULL)) == -1) {
            perror("aceptando la conexión entrante");
            exit(1);
        }

        vSockets.push_back(s1);
        threads[i] = thread(connection_handler, s1);
        i++;

    }

    /* Cerramos las conexiones pendientes. */
    while(vSockets.size() != 0){
        close(vSockets[vSockets.size() - 1]);
        vSockets.pop_back();
    }
  
    /* Cerramos la conexión que escucha. */
    close(s);

    return 0;
}

