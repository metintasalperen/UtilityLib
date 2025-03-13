#ifndef WINSOCKINITIALIZERCLS_H
#define WINSOCKINITIALIZERCLS_H

#include <mutex>
#include <cstdint>

namespace UtilityLib
{
    namespace Socket
    {
        class WinsockInitializerCls
        {
        private:
            static std::mutex Mutex;
            static uint32_t InstanceCount;

        public:
            static int GetInstanceCount();
            static void IncrementInstanceCount();
            static void DecrementInstanceCount();
        };
    }
}

#endif