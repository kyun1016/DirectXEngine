#include <iostream>
#include <memory>

#include "ExampleApp.h"

int main() {
    std::unique_ptr<AppBase> app;
    app = std::make_unique<ExampleApp>();

    if (!app.Initialize()) {
        std::cout << "Initialization failed." << std::endl;
        return -1;
    }

    return app.Run();
}
