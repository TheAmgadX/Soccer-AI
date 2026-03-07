#pragma once
#include "../game/entity_manager.h"
#include "message.h"
#include "../utils/time_system.h"

#include <set>

class MessageManager {
private:
    std::set<Message> delayed_messages;

    EntityManager *entityMgr = EntityMgr;

    TimeSystem *timer = Timer;

    MessageManager() {}

    MessageManager(const MessageManager &);
    MessageManager &operator=(const MessageManager &);

public:
    static MessageManager *Instance() {
        static MessageManager MessageManager;

        return &MessageManager;
    }

    void Send(const Message &message, Entity *receiver) {
        receiver->HandleMessage(message);
    }

    void SendMessage(int sender, int receiver, MessageEnum msg, double delay,
                     void *extra_info) {
        Message message = Message(sender, receiver, msg, delay, extra_info);
        Entity *receiverEntity = entityMgr->GetEntity(receiver);

        if (!receiverEntity) {
            return;
        }

        if (delay <= 0.0) {
            Send(message, receiverEntity);
            return;
        }

        message.time = delay + timer->g_Time;

        delayed_messages.insert(message);
    }

    void SendDelayedMessages() {
        auto iter = delayed_messages.begin();
        while (iter != delayed_messages.end() && timer->IsPassed(iter->time)) {
            Entity *receiver = entityMgr->GetEntity(iter->receiver);
            Send(*iter, receiver);

            iter = delayed_messages.erase(iter);
        }
    }
};

#define MsgMgr MessageManager::Instance()
