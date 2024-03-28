#include "library.h"

int main()
{
    DHost ctx("clr.runtimeconfig.json");

    if (!ctx.LoadAssembly("clr.dll"))
    {
        puts("asm");
        return -1;
    }

    const auto main = reinterpret_cast<void(*)()>(ctx.LoadMethod(
        "Program, clr, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null",
        "Entry",
        "System.Action, System.Private.CoreLib, Version=8.0.0.0, Culture=neutral, PublicKeyToken=7cec85d7bea7798e"));
    if (!main)
    {
        puts("fptr");
        return -1;
    }

    main();

    return 0;
}
