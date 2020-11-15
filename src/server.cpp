#include "header.h"
using namespace std;


/* Creo los vectores necesarios */
vector<int> vSockets;
vector<thread> threadPool;
vector<Client*> onlineClients;

/* Creo los mutex necesarios */
mutex socketListMutex;
mutex clientsListMutex;
mutex createUserMutex;

/* Dado un socket, un nickname y el estado de login, registra un nuevo cliente con el nickname dado si el 
   mismo no se encuentra en uso. 
   En caso contrario, envia un mensaje indicando la falla. Además, actualiza 
   la variable log con el nuevo estado de login del cliente. */


void addClient(Client& c){
    
    clientsListMutex.lock();
        onlineClients.push_back(&c);
    clientsListMutex.unlock();

    return;
}


/* Dado un cliente, lo elimina del sistema y cierra su socket adecuadamanete(ver shutdown()) */
void deleteClient(Client& c){

    shutdown(c.socket_id, 2);
    
    socketListMutex.lock();
    
        for(int i = 0; i < vSockets.size(); i++)
            if(vSockets[i] == c.socket_id)
                vSockets.erase(vSockets.begin() + i);
    
    socketListMutex.unlock();
    
    clientsListMutex.lock();
        
        for(int i = 0; i < onlineClients.size(); i++)
            if(onlineClients[i]->nickname == c.nickname)
                onlineClients.erase(onlineClients.begin() + i);
    
    clientsListMutex.unlock();
    

    return;
}

/* Dado un nick, devuelve un puntero al cliente encontrado con dicho nickname. En caso de no existir,
   el puntero es NULL */
Client* getClient(const string& nick) {
    string newNick = nick;
    for (auto & c: newNick) c = toupper(c);
    clientsListMutex.lock();
        for(int i = 0; i < onlineClients.size(); i++){
            string usedNick = onlineClients[i]->nickname;
            for (auto & c: usedNick) c = toupper(c);

            if(usedNick == newNick){
                clientsListMutex.unlock();
                return onlineClients[i];
            }
        }
    clientsListMutex.unlock();
    return NULL;
}

bool createUser(Client& c){
    createUserMutex.lock();
        if(getClient(c.nickname) != NULL){
            createUserMutex.unlock();
            return false;
        }
        addClient(c);
    createUserMutex.unlock();
    return true;
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

void list(Client c){

    string formatedMsg = "Usuarios conectados:\n";

    clientsListMutex.lock();
    for(int i = 0; i < onlineClients.size(); i++){
        formatedMsg += onlineClients[i]->nickname + "\n";
    }
    clientsListMutex.unlock();

    send(&c, formatedMsg);
    return;
}



/* Difunde un mensaje */
void broadcast(const string& msg, Client oCliente){
    string formatedMsg = "[" + oCliente.nickname + "]: " + msg;
    clientsListMutex.lock();
        for(int i = 0; i < onlineClients.size(); i++)
            if(onlineClients[i]->nickname != oCliente.nickname)
                send(onlineClients[i], formatedMsg);
    clientsListMutex.unlock();
    return;
}

void parse(const string& msg, Client& c){

    std::vector<string> splitedMsg = split(msg, " ");
    
    if(splitedMsg[0] == "/list"){
        list(c);
        return;
    }
    if(splitedMsg[0] == "/spam"){
        for(int x = 0; x < 10; x++) broadcast("@paciook siganme en instagram gracias\n", c);
        return;
    }

    send(&c, "Comando no reconocido o malos parámetros");
    return;
}

Client login(int s){
    
    send(s, "Ingrese nombre de usuario");

    char resp[MENSAJE_MAXIMO];

    leer_de_socket(s, resp);

    string str;
    str += resp;

    Client c(resp, s);

    while(!createUser(c)){
        send(s, "Nombre ya registrado, utilice otro");
        
        leer_de_socket(s, resp);
        str = "";
        str += resp;

        c = Client(resp, s);
    }
    send(s, "REGISTRADO PERRI");

    return c;
}

/* Funcion que ejecutan los threads */
void connection_handler(int socket_desc){

    Client oCliente = login(socket_desc);

    /* Main loop */
    char resp[MENSAJE_MAXIMO];
    while(1) {

        /* leer socket, salir si hubo error*/
        if(leer_de_socket(oCliente.socket_id, resp) == -1)
            break;

        string str = "";
        str += resp;

        printf("[%s]: %s\n", oCliente.nickname.data(), resp);

        /* Parsear el buffer recibido*/

        if(resp[0] == '/')
            parse(str, oCliente);
        else
            broadcast(str, oCliente);

    }

    deleteClient(oCliente);
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
  
    int s1;

    while(1) {  

        // Main loop del servidor
        // Aqui se aceptan conexiones y handlea a cada cliente a partir de un thread

        if ((s1 = accept(s, NULL, NULL)) == -1) {
            perror("aceptando la conexión entrante");
            exit(1);
        }
  
        socketListMutex.lock();
            vSockets.push_back(s1);
        socketListMutex.unlock();

        threadPool.push_back(thread(connection_handler, s1));

    }

    /* Cerramos las conexiones pendientes. */
    while(vSockets.size() != 0){
        socketListMutex.lock();
            shutdown(vSockets[vSockets.size() - 1], 2);
            vSockets.pop_back();
        socketListMutex.unlock();
    }
  
    /* Cerramos la conexión que escucha. */
    close(s);

    return 0;
}

