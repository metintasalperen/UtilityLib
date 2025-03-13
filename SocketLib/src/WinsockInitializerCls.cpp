#include "WinsockInitializerCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        std::mutex WinsockInitializerCls::Mutex;
        uint32_t WinsockInitializerCls::InstanceCount = 0;

        int WinsockInitializerCls::GetInstanceCount()
        {
            std::lock_guard<std::mutex> lock(Mutex);
            return InstanceCount;
        }
        void WinsockInitializerCls::IncrementInstanceCount()
        {
            std::lock_guard<std::mutex> lock(Mutex);
            InstanceCount++;
        }
        void WinsockInitializerCls::DecrementInstanceCount()
        {
            std::lock_guard<std::mutex> lock(Mutex);
            if (InstanceCount > 0)
                InstanceCount--;
        }
    }
}