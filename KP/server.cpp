//если взаиможействие через канал связи происходит не между двумя узлами, то нужно учитывать, что один узел должен быть
//с флагом реквест - отправляет сообщение первым, респонд - отвечает на сообщение, далее взаимодействие должно проходить в формате диалога, 
// реквесту считать, ему нужно сначала отправить что-то
// мы не можем пользоваться таким планом, потому что тогда оба клиента будут респонд,тогда создадим отдельный канал
#include <iostream>
#include <vector>
#include <unistd.h>
#include "zmq_struct.h"
#include <string>
#include <cstdlib>

using namespace std;



int main (){
    vector<player> base_player; 
    vector<play> base_play; 
    int number_connections = 0;
    void* context_server = zmq_ctx_new();
    void* socket_server = zmq_socket(context_server, ZMQ_REP);
    player new_player;
    create_conn(number_connections, socket_server, context_server);
    while (true) {
        //cout << "OOOO" << endl;
        message_server* request = receive_server_without(socket_server);
        //std::cout << "mes: " << msg_server << endl;
        //sleep(5);
        if (request != NULL) {
            //cout << request->mess << " " << request->login_player << " " << request->login_partner << " " << request->id << " " << request->number_game << " " << request->win << endl;
            if(request->mess == CREATE) {// создание нового игрока 
                //cout << "create!!!!" << endl;
                bool repeat = false; // проверим есть ли такой айди 
                for (int i = 0; i < base_player.size(); i++) {
                    if (base_player[i].login == request->login_player) {
                        repeat = true;
                    }
                } 
                if (repeat) {
                    message_server answer = {NO}; // если логин уже существует
                    send_server(socket_server, &answer);
                } else {
                    void* context_new_player = zmq_ctx_new();
                    void* socket_new_player = zmq_socket(context_new_player, ZMQ_REP);
                    number_connections++;
                    create_conn(number_connections, socket_new_player, context_new_player);
                    new_player = {request->login_player, number_connections, context_new_player, socket_new_player};
                    base_player.push_back(new_player);
                    message_server answer = {OK, 0, 0, number_connections};
                    send_server(socket_server, &answer);
                    continue;
                }
            }
        }
        for (int i = 0; i < base_player.size(); i++) {
            //cout << base_player[i].login << endl;
            message_server* request = receive_server_without(base_player[i].socket);
            //cout << "OK1" << endl;
            if (request != NULL){
                //cout << request->mess << endl;
                //cout << "OK2" << endl;
                if(request->mess == SEND_INVATATION) {// создание новой игры
                    bool partner = false;
                    for (int j = 0; j < base_player.size(); j++) {
                        //cout << "OK3 " << partner << endl;
                        if ((request->login_partner == base_player[j].login) && (base_player[j].busy == false)){ 
                            partner = true;                    
                            //должны отправить ему приглашение
                            message_server* join = receive_server(base_player[j].socket); // ожидает wait
                            if (join->mess == WAIT) {
                                message_server invat = {SEND_INVATATION, 0, base_player[i].login, 0, request->number_game};
                                send_server(base_player[j].socket, &invat);
                                message_server* reply = receive_server(base_player[j].socket); // ждем ответ на приглашение
                                if (reply->mess == YES) { // создать отдельный сокет для игры, отправить его в айди игрокам, подтверждения получения ждать не нужно 
                                    number_connections++;
                                    message_server msg_player1 = {PARTNER, base_player[i].login, base_player[j].login, number_connections}; //тот, кто отправил запрос
                                    send_server(base_player[i].socket, &msg_player1);
                                    message_server msg_player2 = {PARTNER, request->login_partner, request->login_player, number_connections};
                                    send_server(base_player[j].socket, &msg_player2);
                                    // в игру нужно занести имя игры
                                    void* context_game = zmq_ctx_new();
                                    void* socket_game = zmq_socket(context_game, ZMQ_REP);
                                    play game = {base_player[i].login, base_player[j].login};
                                    cout << "ser: " << number_connections << " " << game.login_player1 << " " << game.login_player2 << " " << endl; 
                                    create_conn(number_connections, context_game, context_game);
                                    game.number_play = reply->number_game;
                                    game.context =context_game;
                                    game.socket = socket_game;
                                    game.start = true;
                                    game.id = number_connections;
                                    base_play.push_back(game);
                                    base_player[j].busy = true;
                                    base_player[i].busy = true;
                                    break;
                                } else if (reply->mess == NO) {
                                    message_server reply = {REFUSAL};
                                    send_server(base_player[i].socket, &reply);
                                } 
                            } 
    
                        }
                    }
                    //cout << "OK4 " << partner << endl;
                    if (partner == false) {
                        //cout << "SEND" << endl;
                        message_server reply = {NO_PARTNER};
                        send_server(base_player[i].socket, &reply);
                    }
                } else if (request->mess ==CHECK_START) {
                    bool check = false;
                    for (int j = 0; j < base_play.size(); j++) {
                        if ((base_play[j].number_play == request->number_game)&& (base_play[j].start)) { 
                            // тк при присоединении по номеру игры мы находим ее в базе и отмечаем,
                            // как начатую и записываемся вторым игроком
                            check = true;
                            create_conn(number_connections, base_play[j].socket, base_play[j].context);
                            message_server reply = {START, request->login_player, base_play[j].login_player2, base_play[j].id, request->number_game};
                            send_server(base_player[i].socket, &reply);
                            break;
                        }
                    }
                    if (!check) {
                        message_server reply = {NO_START};
                        send_server(base_player[i].socket, &reply);
                        for (int  i = 0; i < base_play.size(); i++) {
                            if (base_play[i].number_play == request->number_game) {
                                base_play.erase(base_play.begin() + i); 
                            }
                        }
                    }
                } else if (request->mess == CREATE_GAME) {//
                    //cout << "111" << endl;
                    number_connections++;
                    //void* context_game = zmq_ctx_new();
                    //cout << "222" << endl;
                    //void* socket_game = zmq_socket(context_game, ZMQ_REP);
                    //cout << "333" << endl;
                    //cout << msg << endl;
                    //cout << msg->login_player << endl;
                    //cout << msg->number_game << endl;
                    play game = {request->login_player};
                    game.number_play = request->number_game;
                    base_play.push_back(game);
                    message_server answer = {OK};
                    send_server(base_player[i].socket, &answer);
                    // TODO: send ok to client, not work!!!!!!!!!!!!!!!!!!!!
                } else if (request->mess == JOIN) {
                    for (int k = 0; k < base_play.size(); k++) {
                        if (base_play[k].number_play = request->number_game)  {
                            if (base_play[k].login_player2 = 0) {
                                message_server reply = {GAME};
                                send_server(base_player[i].socket, &reply);
                                //игра найдена и она свободна
                            }  else {
                                message_server reply = {GAME_BUSY};
                                send_server(base_player[i].socket, &reply);
                                // игра найдена, но несвобода
                            }
                        } else {
                            message_server reply = {NO_GAME};
                            send_server(base_player[i].socket, &reply);
                            // игра не найдена
                        }
                    }
                } else if (request->mess == WAIT) { // ожидаем приглашение
                    base_player[i].weit = true;//это мы проверим при  отправки сообщения
                    //message_server reply = {OK};
                   // send_server(base_player[i].socket, &reply);
                   // не отправляем сообщение, тк оно отправится, когда будет присылаться приглашение, 
                   //поэтому канал занимать нельзя
                }
        
            }

        }   
    }
}
// проверить индекся в циклах, нужно ли заменит на другие буквы
//изменить номера сокетов, для игры у клиентов отдельный канал