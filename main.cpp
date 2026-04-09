#include "App.h"

int main()
{
    App app;

    if (!app.Start())
        return 1;

    app.Run();
    return 0;
}
