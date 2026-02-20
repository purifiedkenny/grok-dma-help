#include "mongoose.h"
#include <thread>
#include <mutex>
#include <string>
#include <sstream>

class WebRadar {
private:
    struct mg_mgr mgr;
    bool running = false;
    std::thread serverThread;

    static void EventHandler(struct mg_connection* c, int ev, void* ev_data);
    void SendData(struct mg_connection* c);
    void SendHtml(struct mg_connection* c);
    void SendFile(struct mg_connection* c, struct mg_http_message* hm);

public:
    WebRadar();

    void Start();
    void Stop();

    ~WebRadar() {
        Stop();
    }
};