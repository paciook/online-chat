#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <thread>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <mutex>
#define MAX_CLIENTS 10
#define BUF_SIZE 4096
#define SOCK_PATH "mi_socket"
#define MENSAJE_MAXIMO  4096
#define PORT 5432

using namespace std;
int leer_de_socket(int s, char buf[]);
vector<string> split(const string& str, const string& delim);

struct Client {

    string nickname;
    int socket_id;

    Client(string n, int s) : nickname(n), socket_id(s) {}
};

