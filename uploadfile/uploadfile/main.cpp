#include "Module.h"
#include "fstream"
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include "mongoose.h"
#include "httphandler.h"

using namespace std;



IModule* machine;

typedef IModule* (*PInitialModule)(std::string featdbfile);

int main(int argc, char** argv)
{
    //≥ı ºªØ
    void* handle = NULL;
    const char* myso = "./libCBIR_linux64.so";

    if ((handle = dlopen(myso, RTLD_NOW)) == NULL) {
        printf("dlopen - %sn", dlerror());
        exit(-1);
    }

    PInitialModule init = (PInitialModule)dlsym(handle, "InitialModule");
    std::cout << "start localhost....." << endl;
    if (init != nullptr) {
        machine = init("");//InitialModule("");
    }
    std::cout << "end localhost....." << endl;

    _fn_data* pfn_data = new _fn_data();
    pfn_data->machine = machine;
    pfn_data->bmultithreading = true;
    struct mg_mgr mgr;                            // Event manager
    mg_log_set("4");                              // Set to 3 to enable debug
    mg_mgr_init(&mgr);                            // Initialise event manager
    mg_http_listen(&mgr, s_listen_on, http_server_handler, pfn_data);  // Create HTTP listener
    for (;;) mg_mgr_poll(&mgr, 1000);             // Infinite event loop
    mg_mgr_free(&mgr);
    
    return 0;
}