#include "header.h" 
using namespace std;

/* Lee un mensaje del socket s y lo guarda en buf. Tener en cuenta que posiblemente sea necesario
   agregar el caracter de terminacion */
   
int leer_de_socket(int socket_id, char buf[]) {
    /* COMPLETAR */
    int n;
    n = recv(socket_id, buf, MENSAJE_MAXIMO, 0);
    if (n == 0) 
        return -1;
    if (n < 0) { 
    	perror("recibiendo");
    	return 0;
    }
    buf[n] = '\0';  /* Agregar caracter de fin de cadena a lo recibido. */
    return 0;
}




/* Dado un string y un delimitador como " ", devuelve un vector de strings donde los elementos son los tokens que
   resultan de la separación segun el delimitador. */

vector<string> split(const string& str, const string& delim)
{
    vector<string> v;
    string aux = str;
    size_t pos = -1;
    string token;
    do{
        token = aux.substr(0, pos);
        v.push_back(token);
        aux.erase(0, pos + delim.length());
    }while ((pos = aux.find(delim)) != std::string::npos);
    return v;
}

