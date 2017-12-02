#ifndef LUAUSER_H
#define LUAUSER_H

#define lua_lock(L)    luaLock(L)
#define lua_unlock(L)  luaUnlock(L)

#ifdef LUA_USE_WINDOWS
#  define lua_userstateopen(L)        luaLockInitial(L)
#  define lua_userstatethread(L, L1)  luaLockInitial(L1)
#endif

void luaLock(lua_State * L);
void luaUnlock(lua_State * L);

#ifdef LUA_USE_WINDOWS
void luaLockInitial(lua_State * L);
void luaLockFinal(lua_State * L);
#endif

#endif /* LUAUSER_H */
