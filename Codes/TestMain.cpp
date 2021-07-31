#include "LuaVirMachine.h"
#include <stdio.h>
#include <string>
#include <iostream>

static int KFunction(lua_State *L, int status, lua_KContext ctx)
{
    printf("status=%d, ctx=%d\n", status, ctx);
    return 0;
}

static std::string GetValueInStack(LuaVirMachine &vm, int idx)
{
    switch (vm.type(idx))
    {
    case LUA_TNIL:
        return "nil";
    case LUA_TBOOLEAN:
        return vm.toboolean(idx) ? "true" : "false";
    case LUA_TLIGHTUSERDATA:
        {
            char str[128];
            snprintf(str, sizeof(str), "%p", vm.touserdata(idx));
            return str;
        }
    case LUA_TNUMBER:
        return std::to_string(vm.tonumber(idx));
    case LUA_TSTRING:
        return vm.tostring(idx);
    case LUA_TTABLE:
        return "_table_";
    case LUA_TFUNCTION:
        return "_function_";
    case LUA_TUSERDATA:
        {
            char str[128];
            snprintf(str, sizeof(str), "%p", vm.touserdata(idx));
            return str;
        }
    case LUA_TTHREAD:
        return "_thread_";
    default:
        return "_unknown_";
    }
}

static int LUA_PrintStack(lua_State *L)
{
    LuaVirMachine vm(L);
    std::cout << "printf stack begin..." << std::endl;
    int stack_size = vm.gettop();
    std::cout << "stack size: " << stack_size << std::endl;
    for (size_t i = 1; i <= stack_size; ++i) {
        std::cout << "index " << i << ": " << GetValueInStack(vm, i)
            << " [type value: " << vm.type(i) << " type name: " << vm.typename_byvalue(vm.type(i)) << "]" << std::endl;
    }
    std::cout << "printf stack end." << std::endl;
    return 0;
}

static int LUA_Test(lua_State *L)
{
    LuaVirMachine vm(L);
    std::cout << "Test Begin..." << std::endl;
    LUA_PrintStack(L);
    vm.push(static_cast<lua_Integer>(100));
    vm.push(static_cast<lua_Integer>(200));
    LUA_PrintStack(L);
    std::cout << "Test End." << std::endl;
    return 2;
}

static int LUA_Func1(lua_State *L)
{
    LuaVirMachine vm(L);
    std::cout << "Func1 Begin..." << std::endl;
    int stack_size = vm.gettop();
    if (stack_size < 1) {
        lua_pushfstring(L, "Func1 error!");
        return 1;
    }
    int a1 = vm.checkinteger(1);
    int a2 = vm.checkinteger(2);
    std::string ret("Func1_ret_1: " + std::to_string(a1 + a2));
    vm.push(ret.c_str());
    vm.push("Func1_ret_2");
    LUA_PrintStack(L);
    std::cout << "Func1 End." << std::endl;
    return 2;
}

static int LUA_CYield(lua_State *L)
{
    LuaVirMachine vm(L);

    return vm.yield(0);
}

static int LUA_CResult(lua_State *L)
{
    LuaVirMachine vm(L);

    (void)vm.resume(vm, 0);
    return 0;
}

int main(int argc, const char **argv)
{
    LuaVirMachine vm;

    /* load lua script */
    int ret = vm.loadfile("lua/1.lua");
    if (0 != ret) {
        printf("load lua file error!");
        return -1;
    }
    int top_idx = vm.gettop();
    std::cout << "[after loading 1.lua] top_idx=" << top_idx << std::endl;
    
    /* ===set global variable=== */
    std::cout << "--set global variable" << std::endl;
    vm.push("I'm a string from C/C++");
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;
    vm.setglobal("str_from_c");
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;

    /* ===set global function=== */
    std::cout << "--set global function" << std::endl;
    vm.push(&LUA_PrintStack);
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;
    vm.setglobal("PrintStack");
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;
    vm.register_func("Func1", &LUA_Func1);
    vm.register_func("c_yield", &LUA_CYield);
    vm.register_func("c_result", &LUA_CResult);
    std::cout << std::endl;

    /* ===c call function in stack=== */
    vm.push(&LUA_Test);
    vm.push("aaa");
    vm.push("bbb");
    vm.push("ccc");
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;
    std::cout << "==begin call Test function==" << std::endl;
    ret = vm.pcall(3, 2, 0);
    std::cout << "==end call Test function(" << ret << ")==" << std::endl;
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;
    LUA_PrintStack(vm);
    vm.pop(2);
    std::cout << std::endl;

    std::cout << "==begin call lua main function==" << std::endl << "********************************" << std::endl;
    ret = vm.pcall(0, 1, 0);
    std::cout << "*********************************" << std::endl << "==end call lua main function(" << ret << ")==" << std::endl;
    top_idx = vm.gettop();
    std::cout << "[after calling main function in 1.lua] top_idx=" << top_idx << std::endl;
    std::cout << "string changed by lua: " << vm.tostring(top_idx) << std::endl;
    vm.pop(1);
    top_idx = vm.gettop();
    std::cout << "[after popping main function return value] top_idx=" << top_idx << std::endl << std::endl;

    std::cout << "stack test..." << std::endl;
    std::string str("test....");
    vm.push(true);
    vm.push(static_cast<lua_Number>(1.2f));
    vm.push(static_cast<lua_Integer>(15));
    vm.push("hello world!");
    vm.push(str.c_str(), str.size());
    LUA_PrintStack(vm);

    // printf("Hello world!\n");
    return 0;
}