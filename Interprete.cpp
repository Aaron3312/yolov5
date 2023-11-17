#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

struct ProcessInfo {
    pid_t pid;
    string name;
};

void executeCommand(const char* command) {
    pid_t pid = fork();

    if (pid == 0) {
        execlp(command, command, nullptr);
        perror("Error al ejecutar el comando");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    } else {
        perror("Error al crear el proceso hijo");
    }
}

void listFiles() {
    cout << "Archivos en el directorio actual:" << endl;
    executeCommand("ls");
}

void systemInfo() {
    cout << "Información del sistema:" << endl;
    executeCommand("uname -a");
}

void changeDirectory(const char* path) {
    if (chdir(path) == 0) {
        cout << "Directorio cambiado a: " << path << endl;
    } else {
        perror("Error al cambiar de directorio");
    }
}

void showCurrentDirectory() {
    char currentDirectory[256];
    if (getcwd(currentDirectory, sizeof(currentDirectory)) != nullptr) {
        cout << "Directorio actual: " << currentDirectory << endl;
    } else {
        perror("Error al obtener el directorio actual");
    }
}

void printMessage(const string& message) {
    cout << "Mensaje: " << message << endl;
}

int main() {
    string input;

    while (true) {
        cout << "MiTerminal> ";
        getline(cin, input);

        if (input == "exit") {
            cout << "Saliendo de la terminal..." << endl;
            break;
        } else if (input == "ls") {
            listFiles();
        } else if (input == "uname") {
            systemInfo();
        } else if (input.substr(0, 2) == "cd") {
            string path = input.substr(3);
            changeDirectory(path.c_str());
        } else if (input == "pwd") {
            showCurrentDirectory();
        } else if (input.substr(0, 5) == "echo ") {
            string message = input.substr(5);
            printMessage(message);
        } else {
            cout << "Comando no reconocido: " << input << endl;
        }
    }

    return 0;

