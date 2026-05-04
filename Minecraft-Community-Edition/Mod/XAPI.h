#pragma once
#ifdef XAPI_USE_V1_0_0
#ifndef XAPI_HEADER
#define XAPI_HEADER

#define XAPI_EXPORT __declspec(dllexport)
#define XAPI_LOCAL 
#define XAPI_STDCALL __stdcall*

#define XAPI_VERSION 1
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
	typedef Xconst Xint32(XAPI_STDCALL XI_createDeviceAndContextFn)(XIDevice**, XIContext**);
	typedef Xconst Xint32(XAPI_STDCALL XI_destroyDeviceAndContextFn)(XIDevice**, XIContext**);

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
		Xvoid(*addRef)(XIDevice* device);
		Xvoid(*release)(XIDevice* device);
		Xint32(*createQEventBus)(XIDevice* device, XHQEventBus* qBus, XQEventBusDescriptor desc);
	}XIDeviceVTable;

	XInterface XIDevice{
		Xconst XIDeviceVTable Xconstptr vtbl;
	};

	typedef struct {
		Xint32(*onShutdown)(Xvoid);
		Xint32(*onUpdate)(Xvoid);
		Xint32(*onInit)(Xvoid);
	}XIExports;

	typedef struct {
		Xvoid(*addRef)(XIContext* ctx);
		Xvoid(*release)(XIContext* ctx);
		Xvoid(*setXIExports)(XIExports* exp);
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
