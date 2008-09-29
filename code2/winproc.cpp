#include "main.h"
#include "sidefunctions.h"
#include "Game.h"
//---------------------------------------------------------
LRESULT CALLBACK WindowProcedure(HWND hWindow,UINT uMessage,WPARAM wparam,LPARAM lparam){
	switch (uMessage){
		case WM_PAINT: g_game.Render(); ValidateRect(hWindow, NULL); return 0;
		case WM_MOUSEMOVE: g_game.MouseMove(wparam, LOWORD(lparam), HIWORD(lparam)); return 0;
		case WM_LBUTTONDOWN: g_game.LeftButtonDown(wparam, lparam);	return 0;
		case WM_LBUTTONUP: g_game.LeftButtonUp(wparam, lparam);	return 0;
		case WM_RBUTTONDOWN: g_game.RightButtonDown(wparam, lparam);	return 0;
		case WM_RBUTTONUP: g_game.RightButtonUp(wparam, lparam);	return 0;
		case WM_MBUTTONDOWN: g_game.MidButtonDown(wparam);	return 0;
		case WM_MBUTTONUP: g_game.MidButtonUp(wparam);	return 0;
		case WM_MOUSEWHEEL: g_game.MouseWheel(wparam); return 0;
		case WM_KEYUP: g_game.KeyUp(wparam);	return 0;
		case WM_KEYDOWN: {
			if (wparam == VK_ESCAPE){
				Log("escape key pressed\n");
				DestroyWindow(hWindow);
			} else {
				g_game.KeyDown(wparam);
			}
			return 0;
		}
		case WM_DESTROY: {
			g_game.run = false;
			break;
		}
	}
	return DefWindowProc(hWindow,uMessage,wparam,lparam);
}