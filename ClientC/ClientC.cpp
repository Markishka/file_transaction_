
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <regex>
#include <fstream>
#pragma warning(disable: 4996)
using namespace std;

SOCKET Connection;

bool isWhiteSpace(char c) {
    static char buffer[1] = "";
    const regex r("\\s");
    buffer[0] = c;
    cout << buffer;
    return regex_match(buffer, buffer + 1, r);
}

string readData1() {
    char prev = 0;

    string path = "myFile1.txt";
    ifstream fin;
    fin.open(path);
    char data;
    std::string conv_data;
    if (!fin.is_open()) {
        cout << "is not open" << endl;
    }
    else {
        cout << "open" << endl;

        while (fin.get(data))
        {
            data = toupper(data);
            if (isWhiteSpace(prev) && isWhiteSpace(data))
                continue;
            conv_data += data;
            prev = data;

        }cout << conv_data << endl;
    }fin.close();
}

std::vector<std::string> split(const string& input, const string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator
        first{ input.begin(), input.end(), re, -1 },
        last;
    return { first, last };
}

string receive_string() {
    static short msg_size0 = 0;
    if (recv(Connection, (char*)&msg_size0, 1, NULL) != 1) {
        cout << "Recv Error #1\n";
        exit(1);
    }

    string msg("");
    msg.reserve(msg_size0);
    if (recv(Connection, (char*)msg.c_str(), msg_size0, NULL) != msg_size0) {
        cout << "Recv Error #2\n";
        exit(1);
    }
    return msg;
}

void send_string(const string& input) {
    int size = input.size();
    if (send(Connection, (const char*)&size, 1, NULL) != 1) {
        cout << "Send Error #1\n";
        exit(1);
    }
    if (send(Connection, input.c_str(), size, NULL) != size) {
        cout << "Recv Error #2\n";
        exit(1);
    }
}


int main(int argc, char* argv[])
{
    // Winsock init
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cout << "Error" << endl;
        exit(1);

    }

    // Connecting
    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1044);
    addr.sin_family = AF_INET;

    Connection = socket(AF_INET, SOCK_STREAM, NULL);
    if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::cout << "Error: failed to connect to server.\n";
        return 1;
    }
    std::cout << "Connected!\n";

    // Messaging start
    string msg2;
    while (true) {
        getline(cin, msg2);
        int msg_size1 = msg2.size();
        
         if (msg2 == "Exit") {
            cout << "You exited" << endl;
            break;
           
        }
      
        else if (msg2 == "run") {
             send(Connection, conv_data.c_str(), sizeof(path), NULL);
         }
        
    }
        system("pause");
        return 0;
    }
