#ifdef MOCK_SDL
#include <iostream>
#include "../attolass.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    setup();
    loop();
    return 0;
}
#endif