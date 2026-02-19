#pragma once 

class Message {
public:
    int sender, receiver, msg;

    double time; 

    void* extraInfo;

    Message(): sender(-1), receiver(-1), msg(-1), time(0.0), extraInfo(nullptr) {}

    Message(int _sender, int _receiver, int _msg, double _time, void* _extraInfo) :
        sender(_sender), receiver(_receiver), msg(_msg), time(_time), extraInfo(_extraInfo) {}
};

inline bool operator ==(const Message& m1, const Message& m2) {
    return m1.time == m2.time;
}

inline bool operator <(const Message& m1, const Message& m2) {
    if(m1.time >= m2.time){
        return false;
    }

    return true;
}

inline bool operator >(const Message& m1, const Message& m2) {
    if(m1.time <= m2.time){
        return false;
    }

    return true;
}