#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char *tmp = _data;
    //Copiar tipo a partir de direccion
    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    //Copiar nick a partir de direccion
    memcpy(tmp, &nick, sizeof(std::string));
    tmp += sizeof(std::string);
    //Copiar message a partir de direccion
    memcpy(tmp, &message, sizeof(std::string));
}

int ChatMessage::from_bin(char *bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char *tmp = _data;
    //Copiar tipo a partir de direccion
    memcpy(&type, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    //Copiar nick a partir de direccion
    nick = tmp;
    tmp += sizeof(std::string);
    //Copiar message a partir de direccion
    message = tmp;
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        ChatMessage em;
        Socket *s;

        //Esperamos recibir un mensaje de cualquier socket
        socket.recv(em, s);
        switch (em.type)
        {
        case ChatMessage::LOGIN:
            clients.push_back(std::move(std::make_unique<Socket>(*s)));
            break;
        case ChatMessage::LOGOUT:
        {
            auto it = clients.begin();
            bool found;
            found = false;
            while (it != clients.end() && !found)
            {
                if (**it == *s)
                {
                    found = true;
                    continue;
                }
                ++it;
            }
            if (found)
            {
                std::cout << "Jugador desconectado:" << em.nick << "\n";
                clients.erase(it);
                delete (*it).release();
            }
            else
            {
                std::cout << "El jugador no existe\n";
            }
            break;
        }
        case ChatMessage::MESSAGE:
        {
            for (auto it = clients.begin(); it != clients.end(); it++)
            {
                if (**it == *s)
                {
                    continue;
                }
                socket.send(em, **it);
            }
            break;
        }
        }
    }
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string msg;
        std::getline(std::cin, msg);

        ChatMessage em(nick, msg);
        em.type = ChatMessage::MESSAGE;

        // Enviar al servidor usando socket
        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while (true)
    {
        //Recibir Mensajes de red
        ChatMessage em;
        socket.recv(em);
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << em.nick << ": " << em.message << "\n";
    }
}
