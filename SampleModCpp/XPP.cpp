#include "XPP.hpp"

IXPP* XPP::_instance;
XI_createDeviceAndContextFn XI_createDeviceAndContext;
XI_destroyDeviceAndContextFn XI_destroyDeviceAndContext;
XHQEventBus qBus;
XIDevice* device;
XIContext* ctx;
XIExports exports;

XPP& XPP::getSingleton() {
	static XPP xpp;
	return xpp;
}

void XPP::initXPP(IXPP* instance) {
	_instance = instance;
}

void XPP::logDebug(const std::string& msg) {
	XSEventLog log;
	log.channel = "default";
	log.msg = msg.c_str();
	log.severity = XE_EVENT_LOG_DEBUG;
	XPP::postEvent(nullptr, &log, XE_EVENT_TYPE_LOG);
}

IXPP* XPP::getModInstance() {
	return _instance;
}
XHQEventBus CXAPI_createQEventBus(Xcstr name) {
	XHQEventBus qBus;
	if (device->vtbl->createQEventBus != XAPI_NULL) {
		XQEventBusDescriptor qBusdesc;
		qBusdesc.name = name;
		qBusdesc.maxSize = 0xFFFF;
		device->vtbl->createQEventBus(device, &qBus, qBusdesc);
	}
	else {
		qBus.idx = 0;
	}
	return qBus;
}
XHQEventBus XPP::createQEventBus(const std::string& name) {
	XHQEventBus qBus;
	if (device->vtbl->createQEventBus != XAPI_NULL) {
		XQEventBusDescriptor qBusDesc;
		qBusDesc.name = name.c_str();
		qBusDesc.maxSize = 0xFFFF;
		device->vtbl->createQEventBus(device, &qBus, qBusDesc);
	}
	else {
		qBus.idx = 0;
	}
	return qBus;
}

int XPP::postEvent(XHQEventBus* _qBus, Xvoid* event, Xconst Xuint64 type) {
	XHQEventBus thisQbus;
	if (_qBus == XAPI_NULL) {
		thisQbus.idx = qBus.idx;
	}
	else {
		thisQbus.idx = _qBus->idx;
	}
	return (ctx->vtbl->postEvent != XAPI_NULL) ? ctx->vtbl->postEvent(&thisQbus, event, type) : XE_ERROR;
}

extern "C" XAPI_EXPORT XAPIDescriptor XI_query(Xvoid) {
	auto& instance = XPP::getSingleton();
	auto desc = instance.getModInstance()->query();
	desc.sdkName = "XPP";
	return desc;
}

extern "C" XAPI_EXPORT Xint32 XI_main(Xvoid* pParam) {
	XI_createDeviceAndContext = (XI_createDeviceAndContextFn)pParam;
	Xconst Xint32 ret = XI_createDeviceAndContext(&device, &ctx);

	if (ret != XE_ERROR) {

		if (device != XAPI_NULL && ctx != XAPI_NULL) {
			if (device->vtbl != XAPI_NULL && ctx->vtbl != XAPI_NULL) {
				if (device->vtbl->createQEventBus != XAPI_NULL) {
					XQEventBusDescriptor desc{};
					desc.maxSize = 0xFFFF;
					desc.name = "APP";
					device->vtbl->createQEventBus(device, &qBus, desc);
				}
				else {
					return XE_ERROR;
				}

				exports.onInit = []() -> Xint32 {return XPP::getSingleton().getModInstance()->onInit(); };
				exports.onUpdate = []() -> Xint32 {return XPP::getSingleton().getModInstance()->onUpdate(); };
				exports.onShutdown = []() -> Xint32 {return XPP::getSingleton().getModInstance()->onShutdown(); };

				if (device->vtbl->setXIExports != XAPI_NULL) {
					device->vtbl->setXIExports(device, exports);
				}
				else {
					return XE_ERROR;
				}
			}
			else {
				return XE_ERROR;
			}
		}
		else {
			return XE_ERROR;
		}
	}
	XPP::getSingleton().logDebug("XPP loaded up successfully! XAPI version v1.0.0");
	return ret;
}


extern "C" XAPI_EXPORT Xint32 XI_terminate(Xvoid* pParam) {
	XI_destroyDeviceAndContext = (XI_destroyDeviceAndContextFn)pParam;

	auto ret = XI_destroyDeviceAndContext(&device, &ctx);
	
	return ret;
}
