#pragma once
#include <Windows.h>

#define DLL_INTERFACE __declspec(dllexport)

typedef void(_stdcall*FNCallbackDeCarga)(float);

extern "C"
{
	DLL_INTERFACE void _stdcall DLL_CreateRenderer(HWND handle, int qual);
	DLL_INTERFACE void _stdcall DLL_ResizeRenderer(int qual, int w, int h);
	DLL_INTERFACE void _stdcall DLL_LoadVolume(const char* pathToFileList);
	DLL_INTERFACE void _stdcall DLL_SetCallbackDeCarga(FNCallbackDeCarga cbk);
	DLL_INTERFACE void _stdcall DLL_SetThickness(int t);
	DLL_INTERFACE void _stdcall DLL_SetFuncao(int idFuncao);
	DLL_INTERFACE void _stdcall DLL_Reset();

	DLL_INTERFACE int _stdcall DLL_MouseMove(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema);
	DLL_INTERFACE int _stdcall DLL_LMouseDown(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema);
	DLL_INTERFACE int _stdcall DLL_LMouseUp(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema);
	DLL_INTERFACE int _stdcall DLL_MMouseDown(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema);
	DLL_INTERFACE int _stdcall DLL_MMouseUp(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema);
	DLL_INTERFACE int _stdcall DLL_RMouseDown(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema);
	DLL_INTERFACE int _stdcall DLL_RMouseUp(HWND wnd, UINT nFlags, int X, int Y, int qualPanel, long handleDoSubsistema);
}