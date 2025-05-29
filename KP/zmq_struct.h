#include <zmq.h>
#include <string.h>
#include <errno.h>

using namespace std;

struct message_server {
    int mess;
    int login_player;
    int login_partner; 
    int id; // сервер создаст новое соединие для игры 
    int number_game;
    int win;
    string a;
};

struct message_client {
    int line;
    int column;
    int result;//результат атаки
};

void Menu() {
    cout << "1)Create a game\n2)Join the game by name\n3)Expect invitation to the game\n";
}

struct player {
    int login;
    int id;
    void* context;
    void* socket;
    bool busy = false;
    bool weit = false;
};

struct play{ // для хранения сокетов игроков для определенной игры
    int login_player1 = 0;
    int login_player2 = 0;
    void* socket;
    int id = 0; //айди канала для игры
    int number_play; // номер игры
    void* context;
    bool start;
    
};

enum Status{
    CREATE, // регистрация игрока
    CREATE_GAME, //создание игры при последующем присоединении к ней по имени 
    JOIN,// присоединииться к игре по имени
    OK, //универсальное одобрение одобрение сервера
    //RECEIVE_INVATATION,//
    SEND_INVATATION,// отправить приглашение 
    PARTNER, //сообщаем игроку, что партнер найден
    NO_PARTNER, //  при попытке отправить приглашение партнер не был найден
    CHECK_START, // проверка на начало игры
    START,// игра началасть
    NO_START, // игра не началась
    GAME_OVER, //
    GAME, //  при присоединении к игре по имени игра найдена и свободна
    NO_GAME,// игра не найдена при присоединении к игре по имени 
    GAME_BUSY, //при попвтке присоединения к игра по имени, найдена игра, но она занята 
    YES, //   игрок соглашается на игру на сервере
    NO, // отказывается от игры на сервере
    REFUSAL, // игрок отказался играть
    WAIT, //ОПОВЕЩАЕМ СЕРВИС об ожидании
    HIT,//
    KILL//
};

int create_conn(int id, void * socket, void * context) {
    int rc;
    char path[40] = "tcp://*:";
    int new_port = id + 4040;
    char port[40];
    sprintf(port, "%d", new_port);
    strcat(path, port);
    rc = zmq_bind(socket, path);
    if (rc == -1) {
        perror ("create");
        return errno;
    }
    return 1;
}

int send_client(void * socket, message_client* msg) {
    int rc;
    zmq_msg_t message;
    zmq_msg_init(&message);
    rc = zmq_msg_init_size(&message, sizeof(message_client));
    rc = zmq_msg_init_data(&message, msg, sizeof(message_client), NULL, NULL);
    rc = zmq_msg_send(&message, socket, 0);
    if (rc == -1) {
        zmq_msg_close(&message);
        printf("Error: sending timeout\n");
        return 0;
    }
    return 1;
}
int send_server(void * socket, message_server* msg) {
    int rc;
    zmq_msg_t message;
    zmq_msg_init(&message);
    rc = zmq_msg_init_size(&message, sizeof(message_server));
    if (rc == -1) {
        zmq_msg_close(&message);
        printf("Error: sending timeout\n");
        return 0;
    }
    rc = zmq_msg_init_data(&message, msg, sizeof(message_server), NULL, NULL);
    if (rc == -1) {
        zmq_msg_close(&message);
        printf("Error: sending timeout\n");
        return 0;
    }
    rc = zmq_msg_send(&message, socket, 0);
    if (rc == -1) {
        zmq_msg_close(&message);
        printf("Error: sending timeout\n");
        return 0;
    }
    return 1;
}


message_client * receive_client(void * socket) {
    int rc;
    zmq_msg_t ans;
    zmq_msg_init(&ans);
    rc = zmq_msg_recv(&ans, socket, 0);
    if (rc == -1) {
        zmq_msg_close(&ans);
        return NULL;
    }
    message_client * data = (message_client*)zmq_msg_data(&ans);
    rc = zmq_msg_close(&ans);
    cout << rc << "res \n";
    return data;
}

message_server * receive_server(void * socket) {
    int rc;
    zmq_msg_t ans;
    zmq_msg_init(&ans);
    rc = zmq_msg_recv(&ans, socket, 0);
    if (rc == -1) {
        zmq_msg_close(&ans);
        return NULL;
    }
    message_server * data = (message_server*)zmq_msg_data(&ans);
    rc = zmq_msg_close(&ans);
    return data;
}
message_server * receive_server_without(void * socket) {
    int rc;
    zmq_msg_t ans;
    zmq_msg_init(&ans);
    rc = zmq_msg_recv(&ans, socket, ZMQ_DONTWAIT);
    if (rc == -1) {
        zmq_msg_close(&ans);
        return NULL;
    }
    message_server * data = (message_server*)zmq_msg_data(&ans);
    rc = zmq_msg_close(&ans);
    return data;
}

void connect(void* socket, int id) {
    string address = "tcp://127.0.0.1:" + to_string(4040 + id);
    int rc = zmq_connect(socket, address.c_str());
    if (rc == -1) {
        perror ("connect");
        cout << errno << "\n";
    }
}
// g++ pr_son.cpp -lzmq -o server
// после того как пришло приглашение ничего не происходит