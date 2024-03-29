# DHost

A simple .NET native-host wrapper for C++

## Limitations

- Only supports .NET 5.0 or higher

## How to build

### Summary

```shell
$ mkdir build
$ cd build
$ cmake -DNETHOST_RUNTIME_IDENTIFIER="linux-x64" -DNETHOST_RUNTIME_VERSION="8.0.3" ../
$ make # use what you configured with cmake
```

### CMake Variables

> [!WARNING]
> Target platform not altered even if mismatched with `NETHOST_RUNTIME_IDENTIFIER`.
> To change target platform, Use cross-platform tools such as MINGW.

- `NETHOST_RUNTIME_IDENTIFIER` : .NET RID (See [Catalog](https://learn.microsoft.com/en-us/dotnet/core/rid-catalog))
- `NETHOST_RUNTIME_VERSION` : .NET Runtime Version (See [Version List](https://www.nuget.org/packages/runtime.linux-x64.Microsoft.NETCore.DotNetAppHost#versions-body-tab))

## How to use (Sample)

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

> [!WARNING]
> When specifying type, Assembly qualified name is required.

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
