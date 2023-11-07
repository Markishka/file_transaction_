
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

        }
    }fin.close();
    return conv_data;
}

std::vector<std::string> split(const string& input, const string& regex) {

    std::regex re(regex);
    std::sregex_token_iterator
        first{ input.begin(), input.end(), re, -1 },
        last;
    return { first, last };
}

string receive_string() {
    static short msg_size0 = 0;
    int result = recv(Connection, (char*)&msg_size0, 1, NULL);
    if(result == 0) return "";
    if (result < 0) {
        int n = WSAGetLastError();
        cout << "Recv Error #1\n";
        exit(1);
    }

    string msg("");
    msg.resize(msg_size0);
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

    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cout << "Error" << endl;
        exit(1);

    }

   
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


    string msg2;
    while (true) {
        getline(cin, msg2);
        
        if (msg2 == "exit") {
            send_string("exit");
            cout << "You exited" << endl;
            break;
        }
        else if(msg2 == "who"){
            send_string("who");
            cout<<receive_string()<<endl;
            continue;
        }
        else if (msg2 == "run") {
            send_string("run");
            vector<string> myLines = split(readData1(), "\\n");

            send_string(to_string(myLines.size()));
            for (int i = 0; i < myLines.size(); i++)
                send_string(myLines[i]);

            int indexesNum = stoi(receive_string());
            vector<int> indexes(indexesNum);
            for(int i = 0; i < indexesNum; i++)
                indexes[i] = stoi(receive_string());

            if(indexes.size() == 0){
                cout<<"There are no differences!\n";
                continue;
            }
            
            cout<<"The lines on client, that are different:\n";
            if(indexes[indexes.size() - 1] > myLines.size()){

                int i = 0;
                for(; indexes[i] < myLines.size(); i++)
                    cout<<myLines[indexes[i]]<<"\n";
                cout<<"The following line indexes are not awailable on client:\n";
                for(; i < indexes.size(); i++)
                    cout<<i<<" ";
                cout<<"\n";
            }
            else{
 
                for(int i = 0; i < indexes.size(); i++)
                    cout<<myLines[indexes[i]]<<"\n";
            }
        }
        
    }

        return 0;
    }
