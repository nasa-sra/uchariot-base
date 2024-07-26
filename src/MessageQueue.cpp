#include "MessageQueue.h"

MessageQueue::MessageQueue(const std::string& name) {
    this->_name = name.c_str();

    // ftok to generate unique key 
    _key = ftok(_name, 65); 

    // msgget creates a message queue 
    // and returns identifier 
    _msgid = msgget(_key, 0666 | IPC_CREAT); 
}

void MessageQueue::Write(std::string data) {
    
    _msg._type = 1; 

    strcpy(_msg._content, data.c_str()); 

    // msgsnd to send message 
    msgsnd(_msgid, &_msg, sizeof(_msg), 0); 

    // display the message 
    // printf("Data send is : %s \n", _msg._content); 
}