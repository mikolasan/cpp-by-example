#include <iostream>

// #include "lua.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace {
    int l_my_print(lua_State* L) {
        int nargs = lua_gettop(L);
        std::cout << "[LUA] ";
        for (int i = 1; i <= nargs; ++i) {
        	std::cout << lua_tostring(L, i) << " ";
        }
        std::cout << std::endl;
        return 0;
    };

    const struct luaL_Reg printlib [] = {
        {"print", l_my_print},
        {NULL, NULL} /* end of array */
    };
} //empty namespace

lua_State* prepare_lua() {
    lua_State* L = luaL_newstate();
    if (!L) return NULL;
    luaL_openlibs(L);
    // wrap Lua prints
    lua_getglobal(L, "_G");
    luaL_setfuncs(L, printlib, 0);
    lua_pop(L, 1);
    return L;
}

const char VARNAME_PARASITE[] = "_parasite_tables";

void get_parasite_field(lua_State* L, int key, const char* name)
{
    lua_getglobal(L, VARNAME_PARASITE); // 1
    int base = lua_gettop(L);
    if(lua_istable(L, -1))
    {
        lua_pushinteger(L, key); // 2
        lua_gettable(L, -2); // 2
        if(lua_istable(L, -1))
        {
            lua_pushstring(L, name); // 3
            lua_gettable(L, -2); // 3
            if(!lua_isnil(L, -1))
            {
                lua_insert(L, base);
                lua_pop(L, 2);
                return;
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    lua_pushnil(L);
}

void set_parasite_field(lua_State* L, int key, const char* name)
{
    // printf("set_parasite_field(top: %d) BEGIN\n", lua_gettop(L));
    lua_getglobal(L, VARNAME_PARASITE);
    if(!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, VARNAME_PARASITE);
        lua_getglobal(L, VARNAME_PARASITE);
    }
    lua_pushinteger(L, key);
    lua_gettable(L, -2);
    if(lua_isnil(L, -1))
    {
        lua_pop(L, 1);
        lua_pushinteger(L, key);
        lua_newtable(L);
        lua_settable(L, -3);
        lua_pushinteger(L, key);
        lua_gettable(L, -2);
    }
    lua_pushstring(L, name);
    lua_pushvalue(L, -4);
    lua_settable(L, -3);

    lua_pop(L, 2); // Pop parasite table and its list.
}

void get_parasite_field_new(lua_State* L, int key, const char* name)
{
    // lua: _parasite_tables
    lua_getglobal(L, VARNAME_PARASITE);
    int table_index = lua_gettop(L);
    if(lua_istable(L, table_index))
    {
        // lua: _parasite_tables[key]
        lua_pushinteger(L, key);
        lua_gettable(L, table_index);
        int table_2_index = lua_gettop(L);
        if(lua_istable(L, table_2_index))
        {
            // lua: _parasite_tables[key][name]
            lua_pushstring(L, name);
            lua_gettable(L, table_2_index);
            if(!lua_isnil(L, -1))
            {
                lua_insert(L, table_index); // move down to preserve the value
                lua_pop(L, 2); // _parasite_tables[key], _parasite_tables
                return;
            }
            lua_pop(L, 1); // _parasite_tables[key][name]
        }
        lua_pop(L, 1); // _parasite_tables[key]
    }
    lua_pop(L, 1); // _parasite_tables
    lua_pushnil(L);
}

void set_parasite_field_new(lua_State* L, int key, const char* name)
{
    int value_index = lua_gettop(L); // value to set is on the stack already
    // lua: _parasite_tables
    lua_getglobal(L, VARNAME_PARASITE);
    if(!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, VARNAME_PARASITE);
        lua_getglobal(L, VARNAME_PARASITE);
    }
    // lua: _parasite_tables[key]
    int table_index = lua_gettop(L);
    lua_pushinteger(L, key);
    lua_gettable(L, table_index);
    if(lua_isnil(L, -1))
    {
        lua_pop(L, 1);
        // lua: _parasite_tables[key] = {}
        lua_pushinteger(L, key);
        lua_newtable(L);
        lua_settable(L, table_index); // pops 2 values
        // lua: _parasite_tables[key]
        lua_pushinteger(L, key);
        lua_gettable(L, table_index); 
    }
    int table_2_index = lua_gettop(L);
    // lua: _parasite_tables[key][name] = value
    lua_pushstring(L, name);
    lua_pushvalue(L, value_index);
    lua_settable(L, table_2_index); // pops 2 values

    lua_pop(L, 2); // Pop parasite table and its list.
}

int main(int argc, char const *argv[])
{
    lua_State* L = prepare_lua();
    
    lua_pushinteger(L, 42);
    set_parasite_field_new(L, 13, "test");

    get_parasite_field_new(L, 13, "test");

    const char* lua_code = R"lua(
        local t = _G['_parasite_tables']
        if t then
            print("_G['_parasite_tables']")
            local t2 = t[13]
            if t2 then
                print("    [13]")
                local v = t2['test']
                if v then
                    print("        ['test']")
                    print("            ", v)
                end
            end
        end
        --for k, v in pairs(t) do
        --    print(k)
        --    print(type(v))
        --    for _k, _v in pairs(v) do
        --        print("   ", _k, _v, type(_v))
        --    end
        --end
    )lua";

    if (luaL_dostring(L, lua_code))
    {
        std::cerr << lua_tostring(L, -1) << std::endl;
    }
    // int index = lua_gettop(L);
    // std::cout << index << std::endl;
    // lua_pushinteger(L, 1);
    // lua_pushinteger(L, 2);
    // lua_pushinteger(L, 3);
    // lua_pushinteger(L, 4);
    // lua_pushinteger(L, 5);
    // lua_pushinteger(L, 6);
    // // lua_insert(L, -6);
    // lua_pushvalue(L, index);

    int a = 0;
    a = lua_tointeger(L, -1);
    lua_pop(L, 1);
    std::cout << a << std::endl;

    // a = lua_tointeger(L, -1);
    // lua_pop(L, 1);
    // std::cout << a << std::endl;

    // a = lua_tointeger(L, -1);
    // lua_pop(L, 1);
    // std::cout << a << std::endl;

    // a = lua_tointeger(L, -1);
    // lua_pop(L, 1);
    // std::cout << a << std::endl;

    // a = lua_tointeger(L, -1);
    // lua_pop(L, 1);
    // std::cout << a << std::endl;

    // a = lua_tointeger(L, -1);
    // lua_pop(L, 1);
    // std::cout << a << std::endl;

    // a = lua_tointeger(L, -1);
    // lua_pop(L, 1);
    // std::cout << a << std::endl;

    lua_close(L);
}

