#ifndef DHOST_LIBRARY_H
#define DHOST_LIBRARY_H

using Library = const void*;

#include <cstdint>
#include <string>

using size_t = std::size_t;

#include <hostfxr.h>
#include <coreclr_delegates.h>

struct DHostTable
{
    Library HostFXR = nullptr;

    hostfxr_initialize_for_runtime_config_fn Initialize         = nullptr;
    hostfxr_get_runtime_delegate_fn          GetDelegate        = nullptr;
    hostfxr_close_fn                         Close              = nullptr;
    load_assembly_fn                         LoadAssembly       = nullptr;
    get_function_pointer_fn                  GetFunctionPointer = nullptr;
};

class DHost
{
    DHostTable m_ctx;

public:
    explicit DHost(const std::string& runtimeconfigPath);

    [[nodiscard]]
    bool LoadAssembly(const std::string& path) const;

    [[nodiscard]]
    void* LoadMethod(const std::string& type, const std::string& method, const std::string& sig) const;

    void Dispose() const;
};

#endif //DHOST_LIBRARY_H
