
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <array>
#include <cstdlib>
#include <ctime>
#include <regex>
#include <vector>
#pragma warning(disable: 4996)
using namespace std;

SOCKET connection;

bool isWhiteSpace(char c){
    static char buffer[1] = "";
    const regex r("\\s");
    buffer[0] = c;
    return regex_match(buffer, buffer+1, r);
}

string readData(){
    string path = "myFile2.txt";
    ifstream fin;
    fin.open(path);
    char prev = 0;
    char data1;
    string chang_data;
    if (!fin.is_open()) {
        std::cout << "is not open" << endl;
    }
    else {
        std::cout << "open" << endl;
        while (fin.get(data1))
        {
            data1 = toupper(data1);
            if(isWhiteSpace(prev) && isWhiteSpace(data1))
                continue;
            chang_data += data1;
            prev = data1;

        }
    }fin.close();
    return chang_data;
}

void printTime(){
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    string data(buffer);
    cout << data << " ";


}

std::vector<std::string> split(const string& input, const string& regex) {
    std::regex re(regex);
    std::sregex_token_iterator
        first{input.begin(), input.end(), re, -1},
        last;
    return {first, last};
}

string receive_string() {
    static short msg_size0 = 0;
    int result = recv(connection, (char*)&msg_size0, 1, NULL);
    if (result == 0) return "";
    if (result < 0) {
        int n = WSAGetLastError();
        cout << "Recv Error #1\n";  
        exit(1);
    }

    string msg("");
    msg.resize(msg_size0);
    if (recv(connection, (char*)msg.c_str(), msg_size0, NULL) != msg_size0) {
        cout << "Recv Error #2\n";
        exit(1);
    }
    return msg;
}

void send_string(const string& input){
    int size = input.size();
    if(send(connection, (const char*) &size, 1, NULL) != 1){
        cout << "Send Error #1\n";
        exit(1);
    }
    if(send(connection, input.c_str(), size, NULL) != size){
        cout << "Recv Error #2\n";
        exit(1);
    }
}

    
int main() {
    
    cout << "\nThis is the programme for the 19-th variant \nTransfer of files";
    cout << "It was coded by Yurchyshyn Markiian \n";

    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error" << std::endl;
        exit(1);
    }
    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1044);
    addr.sin_family = AF_INET;

  
    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    connection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
    if (connection == 0){
        cout << "Error #2\n";
        exit(1);
    }
    cout << "CLient connected!\n";

    while (true) {
        string msg = receive_string();
        printTime();

        if (msg == "exit") {
            cout<<"Client disconected!!\n";
            break;
        }
        cout << msg.size() << " " << msg << endl;

        if (msg == "who") 
            send_string("It was coded by Yurchyshyn Markiian 19-th variant \nTransfer of files");

        if (msg == "run"){
            

            vector<string> myLines = split(readData(), "\\n");

            int linesCount = stoi(receive_string());
            int minimum = myLines.size() > linesCount ? linesCount : myLines.size();
            vector<int> response(0);
            for(int i = 0; i < minimum; i++){
                if(myLines[i] != receive_string())
                    response.push_back(i);
            }
            if(linesCount > myLines.size()){
                int delta = linesCount - myLines.size();
                for(int i = 0; i < delta; i++){
                    receive_string();
                    response.push_back(minimum + i + 1);
                }
            }
            else{
                int delta = myLines.size() - linesCount;
                for(int i = 0; i < delta; i++)
                    response.push_back(minimum + i + 1);
            }
            
            send_string(to_string(response.size()));
            for(int i = 0; i < response.size(); i++)
                send_string(to_string(response[i]));
        }

   
    }
    return 0;
}
    
