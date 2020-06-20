#include "singleton.h"
#include <array>

namespace
{
    constexpr int kMaxFinalizersSize{ 256 };

    std::mutex g_mutex;
    int g_num_finalizers_size;
    std::array<SingletonFinalizer::FinalizerFunc, kMaxFinalizersSize> gFinalizers;
}

void SingletonFinalizer::AddFinalizer(FinalizerFunc func)
{
    std::lock_guard<std::mutex> lock(g_mutex);

    assert(g_num_finalizers_size < kMaxFinalizersSize);
    gFinalizers[g_num_finalizers_size++] = func;
}

void SingletonFinalizer::Finalize()
{
    std::lock_guard<std::mutex> lock(g_mutex);

    for (int i = g_num_finalizers_size - 1; i >= 0; --i)
    {
        (*gFinalizers[i])();
    }
    g_num_finalizers_size = 0;
}