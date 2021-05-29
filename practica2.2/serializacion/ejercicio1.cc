#include "Serializable.h"

#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_NLENGHT 80

class Jugador : public Serializable
{
public:
    Jugador(const char *_n, int16_t _x, int16_t _y) : x(_x), y(_y)
    {
        strncpy(name, _n, MAX_NLENGHT);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        //tamaño de la clase
        int32_t data_size = MAX_NLENGHT * sizeof(char) + 2 * sizeof(int16_t);
        //reserva memoria
        alloc_data(data_size);
        //Puntero que apunte a la direccion de memoria
        char *tmp = _data;
        //Copiar nombre a partir de direccion
        memcpy(tmp, name, MAX_NLENGHT * sizeof(char));
        //Avanzar puntero
        tmp += MAX_NLENGHT * sizeof(char);
        //Copiar x a partir de direccion
        memcpy(tmp, &x, sizeof(int16_t));
        //Avanzar puntero
        tmp += sizeof(int16_t);
        //Copiar y a partir de direccion
        memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char *data)
    {
        //Puntero que apunte a la direccion de memoria
        //para copiar
        char *tmp = data;
        //Copiar nombre
        memcpy(name, tmp, MAX_NLENGHT * sizeof(char));
        //Avanzar puntero
        tmp += MAX_NLENGHT * sizeof(char);
        //Copiar entero x
        memcpy(&x, tmp, sizeof(int16_t));
        //Avanzar puntero
        tmp += sizeof(int16_t);
        //Copiar entero y
        memcpy(&y, tmp, sizeof(int16_t));
        return 0;
    }

public:
    char name[MAX_NLENGHT];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    int fd = open("./data_jugador.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);
    ssize_t size = write(fd, one_w.data(), one_w.size());
    close(fd);
    // 3. Leer el fichero
    fd = open("./data_jugador.txt", O_RDONLY, 0666);
    char buffer[size];
    if (read(fd, buffer, size) == -1)
    {
        return -1;
    };
    close(fd);
    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);
    // 5. Mostrar el contenido de one_r
    std::cout << "Nombre: " << one_r.name << "\nx: " << one_r.x << "\ny: " << one_r.y << "\n";

    return 0;
}
