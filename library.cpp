#include "library.h"

#include <iostream>
#include <vector>

#include <nethost.h>
#include <coreclr_delegates.h>
#include <cstring>
#include <hostfxr.h>

#include <dlfcn.h>

using ustring = std::basic_string<char_t>;

namespace
{
#if _WIN32
    std::wstring atow(const std::string& a)
    {
        std::wstring ws(a.size(), L' ');
        ws.resize(std::mbstowcs(&ws[0], a.data(), a.size()));
        return ws;
    }

#define USTR(a) (atow(a))
#else
#define USTR(a) (a)
#endif

    Library OpenLibrary(const std::string& path)
    {
#if _WIN32
        return LoadLibraryW(USTR(path.data()));
#else
        return dlopen(path.data(), RTLD_LAZY);
#endif
    }

    template<typename T>
    T GetExport(const Library lib, const std::string& name)
    {
#if _WIN32
        return reinterpret_cast<T>(GetProcAddress(lib, name.data()));
#else
        return reinterpret_cast<T>(dlsym(const_cast<void*>(lib), name.data()));
#endif
    }

    void CloseLibrary(const Library lib)
    {
#if _WIN32
        FreeLibrary(lib);
#else
        dlclose(const_cast<void*>(lib));
#endif
    }
}

DHost::DHost(const std::string& runtimeconfigPath)
{
    std::vector<char_t> buffer(32767);
    size_t              size = buffer.size();
    if (get_hostfxr_path(buffer.data(), &size, nullptr))
        throw std::runtime_error("Couldn't get hostfxr path");

    const auto lib = OpenLibrary(buffer.data());

    const auto init = GetExport<hostfxr_initialize_for_runtime_config_fn>(lib, "hostfxr_initialize_for_runtime_config");
    const auto get = GetExport<hostfxr_get_runtime_delegate_fn>(lib, "hostfxr_get_runtime_delegate");
    const auto close = GetExport<hostfxr_close_fn>(lib, "hostfxr_close");
    if (!init || !get || !close)
        throw std::runtime_error("Couldn't find hostfxr exports");

    DHostTable ctx;

    ctx.HostFXR     = lib;
    ctx.Initialize  = init;
    ctx.GetDelegate = get;
    ctx.Close       = close;

    hostfxr_handle cxt{};
    {
        if (ctx.Initialize(USTR(runtimeconfigPath).data(), nullptr, &cxt) || !cxt)
            throw std::runtime_error("Couldn't initialize hostfxr");

        load_assembly_fn ld;
        if (ctx.GetDelegate(cxt, hdt_load_assembly, reinterpret_cast<void**>(&ld)) || !ld)
            throw std::runtime_error("Couldn't retrieve delegate (hdt_load_assembly)");
        ctx.LoadAssembly = ld;

        get_function_pointer_fn fp;
        if (ctx.GetDelegate(cxt, hdt_get_function_pointer, reinterpret_cast<void**>(&fp)) || !fp)
            throw std::runtime_error("Couldn't retrieve delegate (hdt_get_function_pointer)");
        ctx.GetFunctionPointer = fp;
    }
    ctx.Close(cxt);

    m_ctx = ctx;
}

bool DHost::LoadAssembly(const std::string& path) const
{
    char* buffer = realpath(path.data(), nullptr);
    std::string str(buffer);

    const auto rc = m_ctx.LoadAssembly(USTR(str).data(), nullptr, nullptr);

    free(buffer);
    return rc == 0;
}

void* DHost::LoadMethod(const std::string& type, const std::string& method, const std::string& sig) const
{
    void* fp;
    if (m_ctx.GetFunctionPointer(
            USTR(type).data(),
            USTR(method).data(),
            USTR(sig).data(),
            nullptr,
            nullptr,
            &fp) || !fp)
        return nullptr;
    return fp;
}

void DHost::Dispose() const
{
    CloseLibrary(m_ctx.HostFXR);
}

