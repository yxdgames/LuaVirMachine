#pragma once

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// Base on lua version5.3.4
class LuaVirMachine
{
public:
    LuaVirMachine(void)
        : m_pLuaState(::luaL_newstate())
    {
        m_Flag.data = 0x01;
        if (m_pLuaState) luaL_openlibs(m_pLuaState);
    }
    LuaVirMachine(lua_State *ls, bool own = false)
        : m_pLuaState(ls)
    {
        m_Flag.data = !own ? 0x00 : 0x01;
    }
    ~LuaVirMachine(void)
    {
        if (m_pLuaState && m_Flag.owner)
            ::lua_close(m_pLuaState);
    }
public:
    /*********************/
    /* base manipulation */
    /*********************/
    /* base stack manipulation */
    int absindex(int idx) { return lua_absindex(m_pLuaState, idx); }
    int gettop(void) { return lua_gettop(m_pLuaState); }
    void settop(int idx) { lua_settop(m_pLuaState, idx); }
    void pushvalue(int idx) { lua_pushvalue(m_pLuaState, idx); }
    void rotate(int idx, int n) { lua_rotate(m_pLuaState, idx, n); }
    void copy(int fromidx, int toidx) { lua_copy(m_pLuaState, fromidx, toidx); }
    int checkstack(int n) { return lua_checkstack(m_pLuaState, n); }
    void xmove(lua_State *to, int n) { lua_xmove(m_pLuaState, to, n); }

    /* -> stack */
    void push_nil(void) { lua_pushnil(m_pLuaState); }
    int push_thread(void) { return lua_pushthread(m_pLuaState); }
    void push(bool value) { lua_pushboolean(m_pLuaState, value ? 1 : 0); }
    void push(lua_Number value) { lua_pushnumber(m_pLuaState, value); }
    void push(lua_Integer value) { lua_pushinteger(m_pLuaState, value); }
    void push(void *value) { lua_pushlightuserdata(m_pLuaState, value); }
    void push(lua_CFunction fn, int n = 0) { lua_pushcclosure(m_pLuaState, fn, n); }
    const char *push(const char *s, size_t len) { return lua_pushlstring(m_pLuaState, s, len); }
    const char *push(const char *s) { return lua_pushstring(m_pLuaState, s); }
    const char *push_fmt(const char *fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        const char *ret = lua_pushvfstring(m_pLuaState, fmt, ap);
        va_end(ap);
        return ret;
    }
    /* get stack value */
    lua_Number tonumber(int idx, bool *isnum = nullptr)
    {
        int is_num;
        lua_Number ret = lua_tonumberx(m_pLuaState, idx, &is_num);
        if (isnum) *isnum = is_num != 0;
        return ret;
    }
    lua_Integer tointeger(int idx, bool *isnum = nullptr)
    {
        int is_num;
        lua_Integer ret = lua_tointegerx(m_pLuaState, idx, &is_num);
        if (isnum) *isnum = is_num != 0;
        return ret;
    }
    bool toboolean(int idx) { return lua_toboolean(m_pLuaState, idx) != 0 ? true : false; }
    const char *tostring(int idx, size_t *len = nullptr) { return lua_tolstring(m_pLuaState, idx, len); }
    lua_CFunction tocfunction(int idx) { return lua_tocfunction(m_pLuaState, idx); }
    void *touserdata(int idx) { return lua_touserdata(m_pLuaState, idx); }
    lua_State *tothread(int idx) { return lua_tothread(m_pLuaState, idx); }
    const void *topointer(int idx) { return lua_topointer(m_pLuaState, idx); }
    size_t rawlen(int idx) { return lua_rawlen(m_pLuaState, idx); }

    /* Lua -> stack */
    int getglobal(const char *name) { return lua_getglobal(m_pLuaState, name); }
    int gettable(int idx) { return lua_gettable(m_pLuaState, idx); }
    int getfield(int idx, const char *k) { return lua_getfield(m_pLuaState, idx, k); }
    int geti(int idx, lua_Integer n) { return lua_geti(m_pLuaState, idx, n); }
    int rawget(int idx) { return lua_rawget(m_pLuaState, idx); }
    int rawgeti(int idx, lua_Integer n) { return lua_rawgeti(m_pLuaState, idx, n); }
    int rawgetp(int idx, const void *p) { return lua_rawgetp(m_pLuaState, idx, p); }
    void createtable(int narr, int nrec) { lua_createtable(m_pLuaState, narr, nrec); }
    void *newuserdata(size_t sz) { return lua_newuserdata(m_pLuaState, sz); }
    int getmetatable(int objindex) { return lua_getmetatable(m_pLuaState, objindex); }
    int getuservalue(int idx) { return lua_getuservalue(m_pLuaState, idx); }
    /* stack -> Lua */
    void setglobal(const char *name) { lua_setglobal(m_pLuaState, name); }
    void settable(int idx) { lua_settable(m_pLuaState, idx); }
    void setfield(int idx, const char *k) { lua_setfield(m_pLuaState, idx, k); }
    void seti(int idx, lua_Integer n) { lua_seti(m_pLuaState, idx, n); }
    void rawset(int idx) { lua_rawset(m_pLuaState, idx); }
    void rawseti(int idx, lua_Integer n) { lua_rawseti(m_pLuaState, idx, n); }
    void rawsetp(int idx, const void *p) { lua_rawsetp(m_pLuaState, idx, p); }
    int setmetatable(int objindex) { return lua_setmetatable(m_pLuaState, objindex); }
    void setuservalue(int idx) { lua_setuservalue(m_pLuaState, idx); }

    /* check */
    bool isnumber(int idx) { return lua_isnumber(m_pLuaState, idx) != 0 ? true : false; }
    bool isstring(int idx) { return lua_isstring(m_pLuaState, idx) != 0 ? true : false; }
    bool iscfunction(int idx) { return lua_iscfunction(m_pLuaState, idx) != 0 ? true : false; }
    bool isinteger(int idx) { return lua_isinteger(m_pLuaState, idx) != 0 ? true : false; }
    bool isuserdata(int idx) { return lua_isuserdata(m_pLuaState, idx) != 0 ? true : false; }
    int type(int idx) { return lua_type(m_pLuaState, idx); }
    const char *typename_byindex(int idx) { return lua_typename(m_pLuaState, lua_type(m_pLuaState, idx)); }
    const char *typename_byvalue(int type_value) { return lua_typename(m_pLuaState, type_value); }
    bool rawequal(int idx1, int idx2) { return lua_rawequal(m_pLuaState, idx1, idx2) != 0 ? true : false; }
    bool compare(int idx1, int idx2, int op) { return lua_compare(m_pLuaState, idx1, idx2, op) != 0 ? true : false; }
    
    /* load and call functions (load and run Lua code / success: ret == 0) */
    void call(int nargs, int nresults, lua_KContext ctx = 0, lua_KFunction k = nullptr) { lua_callk(m_pLuaState, nargs, nresults, ctx, k); }
    int pcall(int nargs, int nresults, int errfunc, lua_KContext ctx = 0, lua_KFunction k = nullptr) { return lua_pcallk(m_pLuaState, nargs, nresults, errfunc, ctx, k); }
    int load(lua_Reader reader, void *dt, const char *chunkname, const char *mode) { return lua_load(m_pLuaState, reader, dt, chunkname, mode); }
    int dump(lua_Writer writer, void *data, int strip) { return lua_dump(m_pLuaState, writer, data, strip); }

    /* extension */
    //
    void *getextraspace(void) { return lua_getextraspace(m_pLuaState); }
    //
    void pop(int n) { lua_pop(m_pLuaState, n); }
    //
    void newtable(void) { lua_newtable(m_pLuaState); }
    //
    void register_func(const char *name, lua_CFunction fn) { lua_register(m_pLuaState, name, fn); }
    //
    // const char *pushliteral("...") { return lua_pushliteral(m_pLuaState, s); }
    void pushglobaltable(void) { lua_pushglobaltable(m_pLuaState); }
#if defined(LUA_COMPAT_APIINTCASTS)
    void pushunsigned(lua_Unsigned value) { lua_pushunsigned(m_pLuaState, value); }
    //
    lua_Unsigned tounsigned(int idx, bool *isnum = nullptr)
    {
        int is_num;
        lua_Unsigned ret = lua_tounsignedx(m_pLuaState, idx, &is_num);
        if (isnum) *isnum = is_num != 0;
        return ret;
    }
#endif //LUA_COMPAT_APIINTCASTS
    //
    void insert(int idx) { lua_insert(m_pLuaState, idx); }
    void remove(int idx) { lua_remove(m_pLuaState, idx); }
    void replace(int idx) { lua_replace(m_pLuaState, idx); }
    // check
    bool isfunction(int n) { return lua_isfunction(m_pLuaState, n); }
    bool istable(int n) { return lua_istable(m_pLuaState, n); }
    bool islightuserdata(int n) { return lua_islightuserdata(m_pLuaState, n); }
    bool isnil(int n) { return lua_isnil(m_pLuaState, n); }
    bool isboolean(int n) { return lua_isboolean(m_pLuaState, n); }
    bool isthread(int n) { return lua_isthread(m_pLuaState, n); }
    bool isnone(int n) { return lua_isnone(m_pLuaState, n); }
    bool isnoneornil(int n) { return lua_isnoneornil(m_pLuaState, n); }
public:
    /********************/
    /* lib manipulation */
    /********************/
    // arguments for lua_CFunction(if error, lua will be halted)
    void checkstack(int sz, const char *msg) { luaL_checkstack(m_pLuaState, sz, msg); }
    void checktype(int arg, int t) { luaL_checktype(m_pLuaState, arg, t); }
    void checkany(int arg) { luaL_checkany(m_pLuaState, arg); }
    const char *checkstring(int arg, size_t *l = nullptr) { return luaL_checklstring(m_pLuaState, arg, l); }
    const char *optlstring(int arg, const char *def, size_t *l) { return luaL_optlstring(m_pLuaState, arg, def, l); }
    lua_Number checknumber(int arg) { return luaL_checknumber(m_pLuaState, arg); }
    lua_Number optnumber(int arg, lua_Number def) { return luaL_optnumber(m_pLuaState, arg, def); }
    lua_Integer checkinteger(int arg) { return luaL_checkinteger(m_pLuaState, arg); }
    lua_Integer optinteger(int arg, lua_Integer def) { return luaL_optinteger(m_pLuaState, arg, def); }

    int newmetatable(const char *tname) { return luaL_newmetatable(m_pLuaState, tname); }
    void setmetatable(const char *tname) { luaL_setmetatable(m_pLuaState, tname); }
    void *testudata(int ud, const char *tname) { return luaL_testudata(m_pLuaState, ud, tname); }
    void *checkudata(int ud, const char *tname) { return luaL_checkudata(m_pLuaState, ud, tname); }

    // success: ret == 0
    int loadfile(const char *filename, const char *mode = nullptr) { return luaL_loadfilex(m_pLuaState, filename, mode); }
    void reg_funcs(const luaL_Reg *l, int nup) { luaL_setfuncs(m_pLuaState, l, nup); }
public:
    operator lua_State*(void) { return this->m_pLuaState; }
private:
    union {
        struct {
            unsigned char owner : 1;
            unsigned char reserve: 7;
        };
        unsigned char data;
    } m_Flag;
private:
    lua_State   *m_pLuaState;
};