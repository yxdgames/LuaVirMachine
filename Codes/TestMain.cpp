#include "LuaVirMachine.h"
#include <stdio.h>
#include <string>
#include <iostream>

static int LUA_PrintStack(lua_State *L)
{
    LuaVirMachine vm(L);
    int stack_size = vm.gettop();
    std::cout << "stack size: " << stack_size << std::endl;
    for (size_t i = 1; i <= stack_size; ++i) {
        std::cout << "index: " << i << " type value: " << vm.type(i) << " type name: " << vm.typename_byvalue(vm.type(i)) << std::endl;
    }
    return 0;
}

static int LUA_Test(lua_State *L)
{
    LuaVirMachine vm(L);
    std::cout << "Test..." << std::endl;
    LUA_PrintStack(L);
    vm.push(static_cast<lua_Integer>(100));
    vm.push(static_cast<lua_Integer>(200));
    LUA_PrintStack(L);
    return 2;
}

static int LUA_Func1(lua_State *L)
{
    LuaVirMachine vm(L);
    int stack_size = vm.gettop();
    if (stack_size < 1) {
        lua_pushfstring(L, "Func1 error!");
        return 1;
    }
    int a1 = vm.checkinteger(1);
    int a2 = vm.checkinteger(2);
    std::string ret(std::to_string(a1 + a2));
    vm.push(ret.c_str());
    vm.push("Func1_ret_2");
    LUA_PrintStack(L);
    return 2;
}

int main(int argc, const char **argv)
{
    LuaVirMachine vm;

    int ret = vm.loadfile("lua/1.lua");
    if (0 != ret) {
        printf("load lua file error!");
        return -1;
    }
    int top_idx = vm.gettop();
    std::cout << "top_idx=" << top_idx << std::endl;
    
    std::cout << std::endl << "--set global variable" << std::endl;
    vm.push("I'm a string from C/C++");
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;
    vm.setglobal("str_from_c");
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;

    std::cout << std::endl << "--set global function" << std::endl;
    vm.push(&LUA_PrintStack);
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;
    vm.setglobal("PrintStack");
    // top_idx = vm.gettop();
    // std::cout << "top_idx=" << top_idx << std::endl;
    
    vm.register_func("Func1", &LUA_Func1);

    std::cout << std::endl;
    vm.push(&LUA_Test);
    vm.push("aaa");
    vm.push("bbb");
    vm.push("ccc");
    top_idx = vm.gettop();
    std::cout << "top_idx=" << top_idx << std::endl;

    std::cout << "==begin call lua test function==" << std::endl;
    ret = vm.pcall(3, 2, 0);
    std::cout << "==end call lua test function(" << ret << ")==" << std::endl;
    top_idx = vm.gettop();
    std::cout << "top_idx=" << top_idx << std::endl;
    LUA_PrintStack(vm);
    vm.pop(2);

    std::cout << std::endl << "==begin call lua global function==" << std::endl;
    ret = vm.pcall(0, 1, 0);
    std::cout << "==end call lua global function(" << ret << ")==" << std::endl;
    top_idx = vm.gettop();
    std::cout << "top_idx=" << top_idx << std::endl;

    std::cout << "string changed by lua: " << vm.tostring(top_idx) << std::endl;
    
    vm.pop(1);
    top_idx = vm.gettop();
    std::cout << "top_idx=" << top_idx << std::endl;
    std::cout << std::endl;
    if (0 != top_idx) {
        auto type_value = vm.type(top_idx);
        auto cfunc = vm.tocfunction(top_idx);
        std::string func_desc;
        if (LUA_TFUNCTION == type_value) {
            if (nullptr == cfunc)
                func_desc = "lua_function";
            else func_desc = "c_function";
        }
        else {
            func_desc = " ";
        }
        std::cout << type_value << " - " << vm.typename_byindex(top_idx) << " - " << func_desc << std::endl;
    }

    std::string str("test....");
    vm.push(true);
    vm.push(static_cast<lua_Number>(1.2f));
    vm.push(static_cast<lua_Integer>(15));
    vm.push("hello world!");
    vm.push(str.c_str(), str.size());
    
    for (int i = 0; i < 5; ++i) {
        std::cout << -1 << " : " << vm.type(-1) << " - " << vm.typename_byvalue(vm.type(-1)) << " - " << vm.tostring(-1) << std::endl;
    }
    std::cout << vm.type(1) << " - " << vm.typename_byvalue(vm.type(1)) << " - " << vm.toboolean(1) << std::endl;
    std::cout << vm.type(2) << " - " << vm.typename_byvalue(vm.type(2)) << " - " << vm.tonumber(2, nullptr) << std::endl;
    std::cout << vm.type(3) << " - " << vm.typename_byvalue(vm.type(3)) << " - " << vm.tointeger(3, nullptr) << std::endl;
    std::cout << vm.type(4) << " - " << vm.typename_byvalue(vm.type(4)) << " - " << vm.tostring(4) << std::endl;
    std::cout << vm.type(5) << " - " << vm.typename_byvalue(vm.type(5)) << " - " << vm.tostring(5) << std::endl;

    // printf("Hello world!\n");
    return 0;
}