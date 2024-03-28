# DHost

A simple .NET native-host wrapper for C++

## Sample

- clr.csproj

```xml
<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net8.0</TargetFramework>
    <ImplicitUsings>enable</ImplicitUsings>
    <Nullable>enable</Nullable>
  </PropertyGroup>

</Project>
```

- Program.cs

```c#
internal static class Program
{
	public static void Entry()
	{
		Console.WriteLine($"Hello from '{typeof(Program).AssemblyQualifiedName}'");
	}

	public static void Main()
	{
		// dummy
	}
}
```

- main.cpp

```c++
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
```
