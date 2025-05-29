#include <zmq.hpp>
#include <iostream>

using namespace std;

const int MAIN_PORT = 4040;
 
void send_message(void* socket, const string& msg) {
    zmq_msg_t req; //  создаем сообщение
    zmq_msg_init_size(&req, msg.size());
    memcpy(zmq_msg_data(&req), msg.c_str(), msg.size());
    zmq_msg_send(&req, socket, 0);
    zmq_msg_close(&req); 
}
 

string receive_message(void* socket) {
    cout << "rec\n";
    zmq_msg_t reply;   // ответ
    zmq_msg_init(&reply);
    int size = zmq_msg_recv(&reply, socket, 0);
    cout << "rec1\n";
    char* str = (char*)malloc(size + 1); // строчка ок и ключ узла
    memcpy(str, zmq_msg_data(&reply), size);
    cout << str << endl;
    string s = string(str);
    cout << "func " << s<< endl;
    zmq_msg_close(&reply);
    return s;
}
 
void connect(void* socket, int id) {
    string address = "tcp://127.0.0.1:" + to_string(MAIN_PORT + id);
    zmq_connect(socket, address.c_str());
}

void bind(void* socket, int id) {
    string address = "tcp://127.0.0.1:" + to_string(MAIN_PORT + id);
    zmq_bind(socket, address.c_str());
}
