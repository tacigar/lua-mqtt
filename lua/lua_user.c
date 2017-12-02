#include "lua.h"
#include "lua_user.h"
#ifdef LUA_USE_WINDOWS
#  include <windows.h>
#endif
#ifdef LUA_USE_POSIX
#  include <pthread.h>
#endif

#ifdef LUA_USE_WINDOWS
static struct
{
    CRITICAL_SECTION lockSct;
    BOOL initialized;
} g_lock;

void luaLockInitial(lua_State *L)
{
    if (!g_lock.initialized) {
        InitializeCriticalSection(&g_lock.lockSct);
        g_lock.initialized = TRUE;
    }
}

void luaLockFinal(lua_State *L)
{
    if (g_lock.initialized) {
        DeleteCriticalSection(&g_lock.lockSct);
        g_lock.initialized = FALSE;
    }
}

void luaLock(lua_State *L)
{
    EnterCriticalSection(&g_lock.lockSct);
}

void luaUnlock(lua_State *L)
{
    LeaveCriticalSection(&g_lock.lockSct);
}
#endif

#ifdef LUA_USE_POSIX
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

static luaLock(lua_State *L)
{
    pthread_mutex_lock(&g_lock);
}

static luaUnlock(lua_State *L)
{
    pthread_mutex_unlock(&g_lock);
}
#endif
