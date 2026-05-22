using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Text;

#pragma warning disable CS8500

// ==========================
// BASIC TYPES
// ==========================
using Xint8 = System.SByte;
using Xint16 = System.Int16;
using Xint32 = System.Int32;
using Xint64 = System.Int64;

using Xuint8 = System.Byte;
using Xuint16 = System.UInt16;
using Xuint32 = System.UInt32;
using Xuint64 = System.UInt64;

// ==========================
// CONSTANTS
// ==========================
public static class XE {
	public const Xuint32 ERROR = 0xDEADBEEF;
	public const Xuint64 EVENT_TYPE_LOG = 0xA2942192B2001D9E;
	public const Xuint8 EVENT_LOG_DEBUG = 0x07;
}

// ==========================
// STRUCTS
// ==========================
[StructLayout(LayoutKind.Sequential)]
public unsafe struct XHQEventBus {
	public Xint64 idx;
}

[StructLayout(LayoutKind.Sequential)]
public unsafe struct XQEventBusDescriptor {
	public Xint8* name;
	public Xuint16 maxSize;
}

[StructLayout(LayoutKind.Sequential)]
public unsafe struct XSEventLog {
	public Xint8* channel;
	public Xint8* msg;
	public Xuint8 severity;
}

[StructLayout(LayoutKind.Sequential)]
public unsafe struct XAPIDescriptor {
	public Xint8* name;
	public Xint8* author;
	public Xint32 version;
	public Xint8* language;
	public Xint8* sdkName;
	public Xint8* dependencies;
}

// ==========================
// FORWARD TYPES
// ==========================
public unsafe struct XIDevice {
	public XIDeviceVTable* vtbl;
}

public unsafe struct XIContext {
	public XIContextVTable* vtbl;
}

// ==========================
// VTABLES
// ==========================
public unsafe struct XIDeviceVTable {
	public delegate* unmanaged[Stdcall]<XIDevice*, void> addRef;
	public delegate* unmanaged[Stdcall]<XIDevice*, void> release;
	public delegate* unmanaged[Stdcall]<XIDevice*, XHQEventBus*, XQEventBusDescriptor, Xint32> createQEventBus;
	public delegate* unmanaged[Stdcall]<XIDevice*, XIExports, void> setXIExports;

}

public unsafe struct XIContextVTable {
	public delegate* unmanaged[Stdcall]<XIContext*, void> addRef;
	public delegate* unmanaged[Stdcall]<XIContext*, void> release;

	//public delegate* unmanaged[Stdcall]<XIExports, void> setXIExports;

	public delegate* unmanaged[Stdcall]<XHQEventBus*, void*, Xuint64, Xuint32> postEvent;

	public delegate* unmanaged[Stdcall]<XHQEventBus*, Xuint64, void*, Xuint32> subscribeEvent;
}

// ==========================
// EXPORT CALLBACKS
// ==========================
public unsafe struct XIExports {
	public delegate* unmanaged[Stdcall]<int> onShutdown;
	public delegate* unmanaged[Stdcall]<int> onUpdate;
	public delegate* unmanaged[Stdcall]<int> onInit;
}

// ==========================
// FUNCTION POINTER TYPES
// ==========================
public unsafe class XAPIFunctions {
	public delegate* unmanaged[Stdcall]<XIDevice**, XIContext**, Xint32> CreateDeviceAndContext;

	public delegate* unmanaged[Stdcall]<XIDevice**, XIContext**, Xint32> DestroyDeviceAndContext;
}

// ==========================
// GLOBAL STATE 
// ==========================
public unsafe static class CSX {
	public static XIDevice* device;
	public static XIContext* ctx;
	public static XHQEventBus qBus;

	// ======================
	// INIT
	// ======================
	public static void Init(void* fn) {
		var create = (delegate* unmanaged[Stdcall]<XIDevice**, XIContext**, int>)fn;

		XIDevice* dev = null;
		XIContext* context = null;

		int ret = create(&dev, &context);
		if (ret == XE.ERROR)
			return;

		// create default event bus
		XQEventBusDescriptor desc;
		desc.name = CString("APP");
		desc.maxSize = 0xFFFF;

		device = dev;
		ctx = context;

		XHQEventBus hQEventBus;

		if (device != null) {
			if (device->vtbl != null) {
				if (device->vtbl->createQEventBus != null) {
					device->vtbl->createQEventBus(device, &hQEventBus, desc);
					qBus = hQEventBus;
				} else {
					return;
				}
			}
		}
	}

	// ======================
	// LOGGING
	// ======================
	public static void LogDebug(string msg) {
		XSEventLog log;

		log.channel = CString("default");
		log.msg = CString(msg);
		log.severity = XE.EVENT_LOG_DEBUG;

		PostEvent(null, &log, XE.EVENT_TYPE_LOG);
	}

	// ======================
	// EVENTS
	// ======================
	public static Xuint32 PostEvent(XHQEventBus* bus, void* ev, Xuint64 type) {
		XHQEventBus target;

		if (bus == null)
			target = qBus;
		else
			target = *bus;

		return ctx->vtbl->postEvent(&target, ev, type);
	}

	// ======================
	// STRING HELPER
	// ======================
	public static sbyte* CString(string s) {
		byte[] bytes = Encoding.ASCII.GetBytes(s + "\0");
		fixed (byte* p = bytes)
			return (sbyte*)p;
	}
}

// ==========================
// EXAMPLE MOD IMPLEMENTATION
// ==========================
public unsafe static class MyMod {
	// callbacks
	[UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvStdcall) })]
	public static int OnInit() {
		CSX.LogDebug("Hello from C# XAPI mod!");
		return 1;
	}

	[UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvStdcall) })]
	public static int OnUpdate() {
		//CSX.LogDebug("This is Update");
		return 1;
	}

	[UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvStdcall) })]
	public static int OnShutdown() {
		return 1;
	}
	static XIExports exp;
	// ======================
	// EXPORTS
	// ======================
	[UnmanagedCallersOnly(EntryPoint = "XI_main")]
	public static int XI_main(void* p) {
		CSX.Init(p);

		exp.onInit = &OnInit;
		exp.onUpdate = &OnUpdate;
		exp.onShutdown = &OnShutdown;

		CSX.device->vtbl->setXIExports(CSX.device, exp);
		return 1;
	}

	[UnmanagedCallersOnly(EntryPoint = "XI_query", CallConvs = new[] { typeof(CallConvStdcall) })]
	public static XAPIDescriptor XI_query() {
		XAPIDescriptor desc;

		desc.name = CSX.CString("C# Mod");
		desc.author = CSX.CString("CDevJoud");
		desc.version = 1;
		desc.language = CSX.CString("C#");
		desc.sdkName = CSX.CString("XAPI-CS");
		desc.dependencies = CSX.CString("mce.core.*");


		return desc;
	}

	[UnmanagedCallersOnly(EntryPoint = "XI_terminate")]
	public static void XI_terminate(void* p) {
		var destroy = (delegate* unmanaged[Stdcall]<XIDevice**, XIContext**, int>)p;
		XIDevice* dev = null;
		XIContext* context = null;
		destroy(&dev, &context);

		CSX.device = dev;
		CSX.ctx = context;
	}
}