
#include "stdafx.h"
#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>
#include <vector>
#include <cstdint>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

void PrintLog(const char *text)
{
#if 0
	static HANDLE file = INVALID_HANDLE_VALUE;
	if (file == INVALID_HANDLE_VALUE)
		file = CreateFile(L"C:\\Development\\dinput.txt", GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	DWORD written = 0;
	WriteFile(file, text, strlen(text), &written, nullptr);
#endif
}

class MouseMovement
{
public:
	MouseMovement()
	{
		WNDCLASSEX class_desc = { 0 };
		class_desc.cbSize = sizeof(WNDCLASSEX);
		class_desc.style = 0;
		class_desc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		class_desc.lpfnWndProc = &MouseMovement::static_window_proc;
		class_desc.lpszClassName = L"MouseMovement";
		ATOM class_atom = RegisterClassEx(&class_desc);

		DWORD ex_style = 0;
		DWORD style = WS_POPUPWINDOW;
		window_handle = CreateWindowEx(ex_style, L"MouseMovement", L"MouseMovement", style, CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, 0, 0, (HINSTANCE)GetModuleHandle(0), this);
		if (window_handle == 0)
			return;

		// See http://msdn.microsoft.com/en-us/library/ms789918.aspx

		#ifndef HID_USAGE_PAGE_GENERIC
		#define HID_USAGE_PAGE_GENERIC		((USHORT) 0x01)
		#endif

		#ifndef HID_USAGE_GENERIC_MOUSE
		#define HID_USAGE_GENERIC_MOUSE	((USHORT) 0x02)
		#endif

		#ifndef HID_USAGE_GENERIC_JOYSTICK
		#define HID_USAGE_GENERIC_JOYSTICK	((USHORT) 0x04)
		#endif

		#ifndef HID_USAGE_GENERIC_GAMEPAD
		#define HID_USAGE_GENERIC_GAMEPAD	((USHORT) 0x05)
		#endif

		#ifndef RIDEV_INPUTSINK
		#define RIDEV_INPUTSINK	(0x100)
		#endif

		RAWINPUTDEVICE rid;
		rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid.usUsage = HID_USAGE_GENERIC_MOUSE;
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = window_handle;
		BOOL result = RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
		if (result == FALSE)
		{
			DestroyWindow(window_handle);
			window_handle = 0;
		}

	}

	~MouseMovement()
	{
		if (window_handle)
			DestroyWindow(window_handle);
	}

	int x = 0;
	int y = 0;

private:
	LRESULT window_proc(HWND window_handle, UINT message_id, WPARAM wparam, LPARAM lparam)
	{
		if (message_id == WM_INPUT)
		{
			HRAWINPUT handle = (HRAWINPUT)lparam;
			UINT size = 0;
			UINT result = GetRawInputData(handle, RID_INPUT, 0, &size, sizeof(RAWINPUTHEADER));
			if (result == 0 && size > 0)
			{
				std::vector<uint8_t*> buffer(size);
				result = GetRawInputData(handle, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER));
				if (result >= 0)
				{
					RAWINPUT *rawinput = (RAWINPUT*)buffer.data();
					if (rawinput->header.dwType == RIM_TYPEMOUSE)
					{
						x += rawinput->data.mouse.lLastX;
						y += rawinput->data.mouse.lLastY;
					}
				}
			}
			return DefWindowProc(window_handle, message_id, wparam, lparam);
		}
		else
		{
			return DefWindowProc(window_handle, message_id, wparam, lparam);
		}
	}

	static LRESULT CALLBACK static_window_proc(HWND window_handle, UINT message_id, WPARAM wparam, LPARAM lparam)
	{
		MouseMovement *self = 0;
		if (message_id == WM_CREATE)
		{
			LPCREATESTRUCT create_struct = (LPCREATESTRUCT)lparam;
			self = (MouseMovement *)create_struct->lpCreateParams;
			SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)self);
		}
		else
		{
			self = (MouseMovement *)GetWindowLongPtr(window_handle, GWLP_USERDATA);
		}

		if (self)
			return self->window_proc(window_handle, message_id, wparam, lparam);
		else
			return DefWindowProc(window_handle, message_id, wparam, lparam);
	}

	HWND window_handle;
};

class DirectInputDeviceImplA : public IDirectInputDeviceA
{
public:
	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOINTERFACE;
	}

	STDMETHOD_(ULONG, AddRef)(THIS)
	{
		return ++refcount;
	}

	STDMETHOD_(ULONG, Release)(THIS)
	{
		ULONG result = --refcount;
		if (result == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return result;
		}
	}

	/*** IDirectInputDeviceA methods ***/
	STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOTIMPL;
	}

	STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOTIMPL;
	}

	STDMETHOD(GetProperty)(THIS_ REFGUID, LPDIPROPHEADER)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOTIMPL;
	}

	STDMETHOD(SetProperty)(THIS_ REFGUID guid, LPCDIPROPHEADER header)
	{
		PrintLog(__FUNCTION__ "\r\n");

		if (&guid == &DIPROP_AXISMODE)
		{
			// Sets the axis mode. The value being set (DIPROPAXISMODE_ABS or DIPROPAXISMODE_REL) must be specified in the dwData member of the associated DIPROPDWORD structure.
		}
		else if (&guid == &DIPROP_BUFFERSIZE)
		{
			// Sets the input buffer size. The value being set must be specified in the dwData member of the associated DIPROPDWORD structure.
		}
		else
		{
			if (&guid == &DIPROP_GRANULARITY) PrintLog("DIPROP_GRANULARITY\r\n");
			else if (&guid == &DIPROP_RANGE) PrintLog("DIPROP_RANGE\r\n");
			else if (&guid == &DIPROP_DEADZONE) PrintLog("DIPROP_DEADZONE\r\n");
			else if (&guid == &DIPROP_SATURATION) PrintLog("DIPROP_SATURATION\r\n");
			else if (&guid == &DIPROP_FFGAIN) PrintLog("DIPROP_FFGAIN\r\n");
			else if (&guid == &DIPROP_FFLOAD) PrintLog("DIPROP_FFLOAD\r\n");
			else if (&guid == &DIPROP_AUTOCENTER) PrintLog("DIPROP_AUTOCENTER\r\n");
			else if (&guid == &DIPROP_CALIBRATIONMODE) PrintLog("DIPROP_CALIBRATIONMODE\r\n");
			else if (&guid == &DIPROP_CALIBRATION) PrintLog("DIPROP_CALIBRATION\r\n");
			else if (&guid == &DIPROP_PHYSICALRANGE) PrintLog("DIPROP_PHYSICALRANGE\r\n");
			else if (&guid == &DIPROP_LOGICALRANGE) PrintLog("DIPROP_LOGICALRANGE\r\n");
			else PrintLog("Unknown DIPROP\r\n");
		}
		return DI_OK;
	}

	STDMETHOD(Acquire)(THIS)
	{
		return S_OK;
	}

	STDMETHOD(Unacquire)(THIS)
	{
		return S_OK;
	}

	STDMETHOD(GetDeviceState)(THIS_ DWORD size, LPVOID data)
	{
		DIMOUSESTATE *state = (DIMOUSESTATE*)data;
		state->lX = movement.x;
		state->lY = movement.y;
		state->lZ = 0;
		state->rgbButtons[0] = 0;
		state->rgbButtons[1] = 0;
		state->rgbButtons[2] = 0;
		state->rgbButtons[3] = 0;
		return DI_OK;
	}

	STDMETHOD(GetDeviceData)(THIS_ DWORD cbObjectData, LPDIDEVICEOBJECTDATA objectData, LPDWORD pdwInOut, DWORD dwFlags)
	{
		*pdwInOut = 0;
		return DI_OK;
	}

	STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT format)
	{
		if (format->dwFlags & DIDF_ABSAXIS)
		{
			PrintLog("DIDF_ABSAXIS\r\n");
		}
		else if (format->dwFlags & DIDF_RELAXIS)
		{
			PrintLog("DIDF_RELAXIS\r\n");
		}
		if (format->dwObjSize == c_dfDIMouse.dwObjSize && format->dwDataSize == c_dfDIMouse.dwDataSize && format->dwNumObjs == c_dfDIMouse.dwNumObjs)
		{
			PrintLog("Looks like c_dfDIMouse\r\n");
		}
		if (format->dwObjSize == c_dfDIMouse2.dwObjSize && format->dwDataSize == c_dfDIMouse2.dwDataSize && format->dwNumObjs == c_dfDIMouse2.dwNumObjs)
		{
			PrintLog("Looks like c_dfDIMouse2\r\n");
		}
		return DI_OK;
	}

	STDMETHOD(SetEventNotification)(THIS_ HANDLE)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOTIMPL;
	}

	STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD)
	{
		return S_OK;
	}

	STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOTIMPL;
	}

	STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOTIMPL;
	}

	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOTIMPL;
	}

	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD, REFGUID)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return S_OK;
	}

	int refcount = 1;
	MouseMovement movement;
};

class DirectInputImplA : public IDirectInputA
{
public:
	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOINTERFACE;
	}

	STDMETHOD_(ULONG, AddRef)(THIS)
	{
		return ++refcount;
	}

	STDMETHOD_(ULONG, Release)(THIS)
	{
		ULONG result = --refcount;
		if (result == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return result;
		}
	}

	/*** IDirectInputA methods ***/
	STDMETHOD(CreateDevice)(THIS_ REFGUID guid, LPDIRECTINPUTDEVICEA *device, LPUNKNOWN)
	{
		if (guid == GUID_SysMouse)
		{
			*device = new DirectInputDeviceImplA();
			return S_OK;
		}
		else
		{
			return DIERR_DEVICENOTREG;
		}
	}

	STDMETHOD(EnumDevices)(THIS_ DWORD dword0, LPDIENUMDEVICESCALLBACKA callback, LPVOID userdata, DWORD dword1)
	{
		PrintLog(__FUNCTION__ "\r\n");
		DIDEVICEINSTANCEA instance = { };
		instance.dwSize = sizeof(DIDEVICEINSTANCEA);
		instance.guidInstance = GUID_SysMouse;
		instance.guidProduct = GUID_SysMouse;
		instance.dwDevType = DIDEVTYPE_MOUSE;
		strcpy(instance.tszInstanceName, "Mouse");
		strcpy(instance.tszProductName, "Mouse");
		instance.guidFFDriver = GUID_SysMouse;
		instance.wUsagePage = 0;
		instance.wUsage = 0;
		callback(&instance, userdata);
		return S_OK;
	}

	STDMETHOD(GetDeviceStatus)(THIS_ REFGUID guid)
	{
		PrintLog(__FUNCTION__ "\r\n");
		if (guid == GUID_SysMouse) return DI_OK;
		return DIERR_DEVICENOTREG;
	}

	STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return E_NOTIMPL;
	}

	STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD)
	{
		PrintLog(__FUNCTION__ "\r\n");
		return S_OK;
	}

	int refcount = 1;
};

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* lplpDirectInput, LPUNKNOWN punkOuter)
{
	*lplpDirectInput = new DirectInputImplA();
	return S_OK;
}

HRESULT WINAPI DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW* lplpDirectInput, LPUNKNOWN punkOuter)
{
	PrintLog(__FUNCTION__ "\r\n");
	return E_NOTIMPL;
}

HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	PrintLog(__FUNCTION__ "\r\n");
	return E_NOTIMPL;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
	PrintLog(__FUNCTION__ "\r\n");
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow()
{
	return S_FALSE;
}
