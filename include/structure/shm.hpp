#ifndef __OPENRM_STRUCTURE_SHARED_MEMORY_HPP__
#define __OPENRM_STRUCTURE_SHARED_MEMORY_HPP__
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <string.h>

namespace rm {

inline key_t __gen_hash_key__(const std::string& __str__) {
    unsigned long __hash = 5381;
    const char* __cstr = __str__.c_str();
    int __c;
    while ((__c = *__cstr++))
        __hash = ((__hash << 5) + __hash) + __c;
    return (key_t)__hash;
}

inline void* __shm_alloc__(key_t __key__, size_t __size__) {
    int __shmid = shmget(__key__, __size__, IPC_CREAT | IPC_EXCL | 0666);
    if (__shmid == -1) {
        __shmid = shmget(__key__, 0, 0);
        if (__shmid == -1) { perror("shmget"); return NULL; }
    }
    void* __shm_ptr = shmat(__shmid, NULL, 0);
    if (__shm_ptr == (void*)-1) { perror("shmat"); return NULL; }

    return __shm_ptr;
}

inline void __shm_free__(key_t __key__) {
    int shmid = shmget(__key__, 0, 0);
    if (shmid == -1) { perror("shmget"); return; }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) { perror("shmctl"); return; }
}

template <class T>
T* SharedMemory(std::string __name__, size_t __num__ = 1UL) {
    key_t __shm_key = __gen_hash_key__(__name__);
    T* __ptr = (T*)__shm_alloc__(__shm_key, __num__ * sizeof(T));
    memset(__ptr, 0, __num__ * sizeof(T));
    return __ptr;
}

inline void SharedFree(std::string __name__) {
    key_t __shm_key = __gen_hash_key__(__name__);
    __shm_free__(__shm_key);
}

}

#endif