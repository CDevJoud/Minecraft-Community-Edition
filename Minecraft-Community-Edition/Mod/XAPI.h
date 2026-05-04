///---------------------------------------------------------------------------------\\\
///   MIT License																	\\\
///   																				\\\
///   Copyright(c) 2026 Joud Kandeel												\\\
///   																				\\\
///   Permission is hereby granted, free of charge, to any person obtaining a copy	\\\
///   of this software and associated documentation files(the "Software"), to deal	\\\
///   in the Software without restriction, including without limitation the rights	\\\
///   to use, copy, modify, merge, publish, distribute, sublicense, and /or sell	\\\
///   copies of the Software, and to permit persons to whom the Software is			\\\
///   furnished to do so, subject to the following conditions :						\\\
///   																				\\\
///   The above copyright notice and this permission notice shall be included in all\\\
///   copies or substantial portions of the Software.								\\\
///   																				\\\
///   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR	\\\
///   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,		\\\
///   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE	\\\
///   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER		\\\
///   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	\\\
///   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE	\\\
///   SOFTWARE.																		\\\
///---------------------------------------------------------------------------------\\\	

#pragma once
#ifdef XAPI_USE_V1_0_0
#ifndef XAPI_HEADER
#define XAPI_HEADER

#ifdef _WIN32
#define XAPI_EXPORT __declspec(dllexport)
#define XAPI_LOCAL
#else
#define XAPI_EXPORT __attribute__((visibility("default")))
#define XAPI_LOCAL __attribute__((visibility("hidden")))
#endif

#define XAPI_LOCAL 
#define XAPI_STDCALL __stdcall

#define XAPI_VERSION 0x01000000u
#define XAPI_NULL (Xvoid*)0

#define XE_ERROR 0xDEADBEEF

#define XE_EVENT_TYPE_LOG 0xA2942192B2001D9E //mce.core.event.logger_output

#define XE_EVENT_LOG_DEBUG 0x07

#define Xconst const
#define Xconstptr const*
#define XInterface struct

#ifdef __cplusplus
extern "C" {
#endif

	typedef char Xint8;
	typedef short Xint16;
	typedef int Xint32;
	typedef long long Xint64;

	typedef unsigned char Xuint8;
	typedef unsigned short Xuint16;
	typedef unsigned int Xuint32;
	typedef unsigned long long Xuint64;

	typedef Xconst char Xconstptr Xcstrcp;
	typedef Xconst char* Xcstr;

	typedef void Xvoid;

	typedef XInterface XIDevice XIDevice;
	typedef XInterface XIContext XIContext;
	typedef Xconst Xint32(XAPI_STDCALL* XI_createDeviceAndContextFn)(XIDevice**, XIContext**);
	typedef Xconst Xint32(XAPI_STDCALL* XI_destroyDeviceAndContextFn)(XIDevice**, XIContext**);

	typedef struct {
		Xcstrcp name;
		Xcstrcp author;
		Xint32 version;
		Xcstrcp language;// programming language
		Xcstrcp sdkName;
		Xcstrcp dependencies;
	}XAPIDescriptor;

	typedef struct {
		Xint64 idx;
	}XHQEventBus;

	typedef struct {
		Xcstr name;
		Xuint16 maxSize;
	}XQEventBusDescriptor;

	typedef struct {
		Xcstr channel;
		Xcstr msg;
		Xuint8 severity;
	}XSEventLog;

	typedef struct {
		Xint32(*onShutdown)(Xvoid);
		Xint32(*onUpdate)(Xvoid);
		Xint32(*onInit)(Xvoid);
	}XIExports;

	typedef struct {
		Xvoid(*addRef)(XIDevice* device);
		Xvoid(*release)(XIDevice* device);
		Xint32(*createQEventBus)(XIDevice* device, XHQEventBus* qBus, XQEventBusDescriptor desc);
		Xvoid(*setXIExports)(XIDevice* device, XIExports exp);
	}XIDeviceVTable;

	XInterface XIDevice{
		Xconst XIDeviceVTable Xconstptr vtbl;
	};

	
	typedef struct {
		Xvoid(*addRef)(XIContext* ctx);
		Xvoid(*release)(XIContext* ctx);
		//Xvoid(*setXIExports)(XIExports exp);
		Xuint32(*postEvent)(XHQEventBus* qBus, Xvoid* event, Xconst Xuint64 type);
		Xuint32(*subscribeEvent)(XHQEventBus* qBus, Xconst Xuint64 type, Xconst Xvoid Xconstptr fn);
	}XIContextVTable;

	XInterface XIContext{
		Xconst XIContextVTable Xconstptr vtbl;
	};

#ifdef __cplusplus
}
#endif
#endif
#endif