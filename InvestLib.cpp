#include <Windows.h> //���������� ��� �����������
#include <thread> //���������� ����������� ������ � ��������
#include "stdafx.h"


//=== ����������� ��� Lua ��������� ============================================================================//
#define LUA_LIB
#define LUA_BUILD_AS_DLL

//=== ������������ ����� LUA ===================================================================================//
extern "C" {
#include "Lua\lauxlib.h"
#include "Lua\lua.h"
}

//=== ����������� ����� ����� ��� DLL ==========================================================================//
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

//������� �������� ������� �� QLua � �������������� � 1 �������
static int MyCallback(lua_State *L)
{
	lua_getglobal(L, "MyLuaCallback"); //������� � ����� Lua ������� �� �� �������� � �������� �� ������ �����
	int callbackRef = luaL_ref(L, LUA_REGISTRYINDEX); //�������� ������ ������� � ����������� ���������� �������(�������) Lua	

	for (int i = 0; i < 10; i++)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef); //������� ������� �� ������� Lua �� ������� � �������� � ����
		lua_pushstring(L, "������ �� DLL"); //��������� � ���� ��������, ������� ����� ������� �������
		lua_call(L, 1, 1); //�������� ��������� ������� � ������� QLua, ��������� � ��� 1 �������� (L - ����, 1 - ���������� ������������ ����������, 1 - ���������� ������������ ��������(�����, ����� ��������� ����))
		Sleep(1000); //����� � 1 �������
	}




	return 0; //����� �� �������
}

//=== ���������� �������, ���������� �� LUA ====================================================================//
static int forLua_StartCallback(lua_State *L)
{
	//	std::thread thr(MyCallback, L); //��������� ���������� ������� MyCallback() � ��������� ������
	// thr.detach(); //����������� ��������� ����� �� ���������, ����� ��� "�������"

	MyCallback(L);
	return (0); //��������� ������ ������� forLua_StartCallback, ��� ���� ������� MyCallback ���������� �������� � ��������� ������
}

//=== ����������� ������������� � dll �������, ����� ��� ����� "������" ��� Lua ================================//
static struct luaL_reg ls_lib[] = {
	{ "StartCallback", forLua_StartCallback }
};

//=== ����������� �������� ����������, �������� � ������� Lua ==================================================//
extern "C" LUALIB_API int luaopen_InvestLib(lua_State *L) {
	luaL_openlib(L, "InvestLib", ls_lib, 0);
	return 0;
}