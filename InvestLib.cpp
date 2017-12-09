#include <Windows.h> //Подключает все необходимое
#include <thread> //Подключает возможность работы с потоками
#include "stdafx.h"


//=== Необходимые для Lua константы ============================================================================//
#define LUA_LIB
#define LUA_BUILD_AS_DLL

//=== Заголовочные файлы LUA ===================================================================================//
extern "C" {
#include "Lua\lauxlib.h"
#include "Lua\lua.h"
}

//=== Стандартная точка входа для DLL ==========================================================================//
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

//Функция вызывает функцию из QLua с периодичностью в 1 секунду
static int MyCallback(lua_State *L)
{
	lua_getglobal(L, "MyLuaCallback"); //Находит в стеке Lua функцию по ее названию и помещает ее наверх стека
	int callbackRef = luaL_ref(L, LUA_REGISTRYINDEX); //Получает индекс функции в специальной внутренней таблице(реестре) Lua	

	for (int i = 0; i < 10; i++)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef); //Достает функцию из реестра Lua по индексу и помещает в стек
		lua_pushstring(L, "Привет из DLL"); //Добавляет в стек параметр, который будет передан функции
		lua_call(L, 1, 1); //Вызывает выбранную функцию в скрипте QLua, передавая в нее 1 параметр (L - стек, 1 - количество передаваемых параметров, 1 - количество возвращаемых значений(нужно, чтобы обновлять стек))
		Sleep(1000); //Пауза в 1 секунду
	}




	return 0; //Выход из функции
}

//=== Реализация функций, вызываемых из LUA ====================================================================//
static int forLua_StartCallback(lua_State *L)
{
	//	std::thread thr(MyCallback, L); //Запускает выполнение функции MyCallback() в отдельном потоке
	// thr.detach(); //Отсоединяет созданный поток от основного, делая его "фоновым"

	MyCallback(L);
	return (0); //Завершает работу функции forLua_StartCallback, при этом функция MyCallback продолжает работать в отдельном потоке
}

//=== Регистрация реализованных в dll функций, чтобы они стали "видимы" для Lua ================================//
static struct luaL_reg ls_lib[] = {
	{ "StartCallback", forLua_StartCallback }
};

//=== Регистрация названия библиотеки, видимого в скрипте Lua ==================================================//
extern "C" LUALIB_API int luaopen_InvestLib(lua_State *L) {
	luaL_openlib(L, "InvestLib", ls_lib, 0);
	return 0;
}