#pragma once

#include <cassert>
#include <mutex>

struct SingletonFinalizer
{
    using FinalizerFunc = void(*)();

    static void AddFinalizer(FinalizerFunc func);
    static void Finalize();
};

template <typename T>
class Singleton final
{
public:
    static T* GetInstancePtr()
    {
        std::lock_guard lg{ mt };

        if (!instance) Create();

        return instance;
    }
    static T& GetInstanceRef()
    {
        std::lock_guard lg{ mt };

        if (!instance) Create();

        return *instance;
    }

private:
    static void Create()
    {
        instance = new T;
        SingletonFinalizer::AddFinalizer(&Singleton<T>::Destroy);
    }

    static void Destroy()
    {
        delete instance;
        instance = nullptr;
    }

    static inline std::mutex mt;
    static inline T* instance;
};