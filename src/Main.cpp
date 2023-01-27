#include "./Engine/Antares.h"

int main(int argc, char* argv[]) {
    Antares engine;

    engine.Initialize();
    engine.Run();
    engine.Destroy();

    return 0;
}