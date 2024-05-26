#include <iostream>
#include <memory>

#include "ExampleApp.h"

int main() {
    std::unique_ptr<kyun::AppBase> app;
    app = std::make_unique<kyun::ExampleApp>();

    if (!app->Initialize()) {
        std::cout << "Initialization failed." << std::endl;
        return -1;
    }

    return app->Run();
}
