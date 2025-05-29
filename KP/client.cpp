#include <iostream>
#include <vector>
#include <unistd.h>
#include "zmq_struct.h"
#include <string>

using namespace std;

int main (){
    int login;
    cout << "Enter your login:" << endl;
    cin >> login;
    void* context_server = zmq_ctx_new();
    void* socket_server = zmq_socket(context_server, ZMQ_REQ);
    connect(socket_server, 0);
    message_server request = {CREATE, login};
    cout << request.mess << " " << request.login_player << endl;
    send_server(socket_server, &request);
    message_server* answer = receive_server(socket_server);
    //cout << reply->mess << endl;
    void* my_context = zmq_ctx_new();
    void* my_socket = zmq_socket(my_context, ZMQ_REQ);
    if (answer->mess == OK) {
        connect(my_socket, answer->id);
        zmq_close(socket_server);
        zmq_ctx_destroy(context_server);
    } else if (answer->mess == NO) {
        cout << "A player with this login already exists" << endl;
        return 0;
    } else {
        cout << "Error" << endl;
        return 0;
    }
    int my_id = answer->id;
    int command;
    //int partner_login;
    play my_play;
    int test_number_play;
    int test_login_partner;
    //my_play.login_player1 = login; //определяем в каждом случае отдельно
    //первый тот, кто отправил или создал, второй - присоединился
    while(true) {
        cout << "Choose an action" << endl;
        Menu();
        cin >> command;
        if (command == 1) {
            cout << "Enter game number:" << endl;
            cin >> test_number_play;
            int action;
            cout <<  "Click\n1) send an invitation to the game\n2) wait to join the game by its name" << endl;
            cin >> action;
            if (action == 1) { //отправим приглашение 
                cout << "Enter your partner's login to send an invitation to the game" << endl;
                cin >> test_login_partner;
                message_server request = {SEND_INVATATION, login, test_login_partner, my_id, test_number_play};
                send_server (my_socket, &request);
                //cout << "OK1" << endl;
                message_server* answer = receive_server(my_socket);
                ///cout << "OK2" << endl;
                if (answer->mess == NO_PARTNER) { 
                    cout << "No partner found for this game, please choose another action" << endl;
                    continue;
                } else if (answer->mess == PARTNER) { //  переделать
                    //my_play.context = zmq_ctx_new();
                    //my_play.socket = zmq_socket(my_context, ZMQ_REP);

                    connect(my_play.socket, answer->id);// соединяемся с игрой по айди
                
                } else if(answer->mess == REFUSAL) {
                    cout << "The player abandoned the game" << endl;
                    continue;
                }

            } else if (action == 2) { //ожидаем присоединения)
                message_server msg = {CREATE_GAME, login, 0, my_id, my_play.number_play};
                send_server (my_socket, &msg);
                //cout << "OK1" << endl;
                message_server* reply = receive_server(my_socket);
                if (reply->mess == OK) {
                    sleep(5);
                    //cout << "11111" << endl;
                    message_server msg = {CHECK_START, login, 0, my_id, my_play.number_play};
                    send_server (my_socket, &msg);
                    
                    message_server* reply = receive_server(my_socket);
                    if(reply->mess == START) {
                        my_play.context = zmq_ctx_new();
                        my_play.socket = zmq_socket(my_context, ZMQ_REP);
                        connect(my_play.socket, reply->id);
                        my_play = {login, reply->login_partner};
                    } else if  (reply->mess == NO_START){
                        cout <<  "No one wants to join the game yet" << endl;
                        return 0;
                    }
                }
            } else {
                cout << "Unknown action" << endl;
            }   
        } else if (command == 2) { // присоединяемся к игре по имени
        int number_play;
            cout << "Enter the number of the game you want to join" << endl;
            cin >> number_play;
            message_server msg = {JOIN, login, 0, my_id, number_play};
            send_server (my_socket, &msg);
            message_server* invat = receive_server(my_socket);
            if (invat->mess == OK) {
                cout << "Game found" << endl;
            } else  if (invat->mess == NO){
                cout << "Game not found, try starting the game again" << endl;
            }
        } else if (command == 3) {
            cout << "waiting for an invitation" << endl;  //уведомляем об ожидании 
            message_server msg = {WAIT, login}; // уведомляем об ожидании  сервис
            send_server (my_socket, &msg);
            message_server* wait = receive_server(my_socket); // ожидаем send_inventation
            if (wait->mess == SEND_INVATATION) {
                cout << "send\n";
                cout << "You have received a message: a player with login" << wait->login_partner << "invites you to the game "<< wait->number_game << ", enter your answer (yes/no)";
                string answer;
                cin >> answer; 
                if (answer == "yes") {
                    message_server reply = {YES};
                    send_server(my_socket, &reply);
                } else if (answer == "no") {
                    message_server reply = {NO};
                    send_server(my_socket, &reply);
                }
            }
            // переписать: нужно обработать сообщение сенд инвент +
            if (my_play.login_player1 = 0) {
                cout << "invitation not received" << endl;
                return 0; 
            }
        } else {
            cout << "Unknown command" << endl;
        }
        if (my_play.start) { // проверяем найден ли напарник и можно начинать игру
            //игра неверно, нужно начинать игру в каждом ифе отдельно, тк меняются позиции
        } 
    }
}
// переписать везде заполнение структуры игры