#include "ModLoader.h"

static XIDevice* g_device;
static XIContext* g_ctx;
static XIExports* g_exp;
eastl::unordered_map<Xuint64, Xvoid*> registeries;
Xuint64 hash_str(Xcstr str) {
	Xuint64 h = 0xcbf29ce484222325ULL; // FNV offset basis

	while (*str) {
		h ^= (Xuint8)(*str++);
		h *= 0x100000001b3ULL; // FNV prime
	}

	// Final mixing (improves avalanche)
	h ^= h >> 33;
	h *= 0xff51afd7ed558ccdULL;
	h ^= h >> 33;
	h *= 0xc4ceb9fe1a85ec53ULL;
	h ^= h >> 33;

	return h;
}
typedef struct {
	XIDevice iface;
	Xint32 ref;
	Xint32 handle;
}DeviceImpl;

typedef struct {
	XIContext iface;
	Xint32 ref;
}ContextImpl;

Xvoid mce_device_addref(XIDevice* device) {
	((DeviceImpl*)device)->ref++;
}

Xvoid mce_device_release(XIDevice* device) {
	DeviceImpl* self = (DeviceImpl*)device;
	if (--self->ref == 0) {
		//printf("[Engine] Device destroyed\n");
	}
}

Xint32 mce_device_createQEventBus(XIDevice* device, XHQEventBus* qBus, XQEventBusDescriptor desc) {
	
	Xuint64 hash = hash_str(desc.name);
	auto it = registeries.find(hash);
	if (it == registeries.end()) {
		registeries[hash] = new mce::core::QEventBus(desc.name);
		qBus->idx = hash;
		return 1;
	}
	else {
		qBus->idx = it->first;
	}
	return 1;
}

Xvoid mce_device_setXIExports(XIDevice* device, XIExports exp) {
	g_exp = exp;
}

Xvoid mce_ctx_addRef(XIContext* ctx) {
	((ContextImpl*)ctx)->ref++;
}

Xvoid mce_ctx_release(XIContext* device) {
	ContextImpl* self = (ContextImpl*)device;
	if (--self->ref == 0) {
		//printf("[Engine] Context destroyed\n");
	}
}

Xvoid mce_ctx_setXIExports(XIExports* exp) {
	g_exp = exp;
}

Xuint32 mce_ctx_postEvent(XHQEventBus* qBus, Xvoid* event, Xconst Xuint64 type) {
	auto it = registeries.find(qBus->idx);
	if (it != registeries.end()) {
		mce::core::QEventBus* _qBus = reinterpret_cast<mce::core::QEventBus*>(it->second);
		
		if (type == XE_EVENT_TYPE_LOG) {
			XSEventLog* XAPI_eventLog = reinterpret_cast<XSEventLog*>(event);
			mce::event::Log log;
			log.channel = XAPI_eventLog->channel;
			log.msg = XAPI_eventLog->msg;
			log.severity = static_cast<mce::event::Log::Severity>(XAPI_eventLog->severity);
			_qBus->post(log);
			return 1;
		}

	}

	return XE_ERROR;
}

void zMem(void*& mem, size_t size) {
	char* _mem = (char*)mem;

	for (size_t i = 0; i < size; i++) {
		_mem[i] = 0;
	}
}

Xvoid setGlobalQEventBus(mce::core::QEventBus* qBus) {
	//auto num = hash_str("mce.core.event.logger_output");
	Xuint64 hash = hash_str(qBus->getNamespace().data());
	auto it = registeries.find(hash);

	if (it == registeries.end()) {
		registeries[hash] = qBus;
	}
}

Xint32 mce_createDeviceAndContext(XIDevice** device, XIContext** ctx) {
	if ((*device) != nullptr && (*ctx) != nullptr) {
		return XE_ERROR;
	}

	*device = new XIDevice();
	*ctx = new XIContext();
	
	XIDeviceVTable* device_vtbl = new XIDeviceVTable();
	XIContextVTable* ctx_vtbl = new XIContextVTable();

	//ZeroMemory(device_vtbl, sizeof(XIDeviceVTable));

	//memset(device_vtbl, 0, sizeof(XIDeviceVTable));

	if (!device_vtbl && !ctx_vtbl) {
		return XE_ERROR;
	}
	const_cast<XIDeviceVTable*>(device_vtbl)->addRef = mce_device_addref;
	const_cast<XIDeviceVTable*>(device_vtbl)->addRef = mce_device_release;
	const_cast<XIDeviceVTable*>(device_vtbl)->createQEventBus = mce_device_createQEventBus;
	const_cast<XIDeviceVTable*>(device_vtbl)->setXIExports = mce_device_setXIExports;

	const_cast<XIContextVTable*>(ctx_vtbl)->addRef = mce_ctx_addRef;
	const_cast<XIContextVTable*>(ctx_vtbl)->release = mce_ctx_release;
	const_cast<XIContextVTable*>(ctx_vtbl)->setXIExports = mce_ctx_setXIExports;
	const_cast<XIContextVTable*>(ctx_vtbl)->postEvent = mce_ctx_postEvent;

	(*device)->vtbl = device_vtbl;
	(*ctx)->vtbl = ctx_vtbl;

	g_device = (*device);
	g_ctx = (*ctx);

	return 1;
}

Xint32 mce_destroyDeviceAndContext(XIDevice** device, XIContext** ctx) {
	if ((*device) == nullptr || (*ctx) == nullptr) {
		return XE_ERROR;
	}

	delete (*device)->vtbl;
	delete (*ctx)->vtbl;

	delete* device;
	delete* ctx;
	return 1;
}

Xconst XIExports* mce_pullSessionsExports() {
	return g_exp;
}
