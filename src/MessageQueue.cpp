#include "MessageQueue.h"

MessageQueue::MessageQueue(const std::string& name,
                           std::function<void(std::string)> callback) {
    _name = name.c_str();

    _key = ftok(_name, 65);

    _msgid = msgget(_key, 0666 | IPC_CREAT);
    if (_msgid == -1) {
        Utils::LogFmt("MessageQueue Failed to create message queue Error: %s",
                      strerror(errno));
    }

    _callback = callback;
    _recieveThread = std::thread(&MessageQueue::recieve, this);
}

void MessageQueue::Close() {
    Utils::LogFmt("Closing message queue");
    _running = false;
    _recieveThread.join();
}

void MessageQueue::recieve() {
    while (_running) {
        struct msqid_ds ms_data;
        msgctl(_msgid, IPC_STAT, &ms_data);
        if (ms_data.msg_qnum > 0) {
            // clear older messages
            char buf[2560];
            int bytesRead;
            for (int i = 0; i < ms_data.msg_qnum; i++) {
                bytesRead = msgrcv(_msgid, &buf, sizeof(buf), 1, IPC_NOWAIT);
                if (bytesRead == -1) {
                    Utils::LogFmt(
                        "MessageQueue::Recieve - Error recieving: %s",
                        strerror(errno));
                }
            }
            std::string msg(buf+sizeof(long), bytesRead-sizeof(long));
            _callback(msg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}
