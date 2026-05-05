#include "CXAPI.h"

XAPIDescriptor desc;
XI_createDeviceAndContextFn XI_createDeviceAndContext;
XI_destroyDeviceAndContextFn XI_destroyDeviceAndContext;
XIDevice* device;
XIContext* ctx;
XIExports exports, cxapiExports;
XHQEventBus qBus;

XAPI_LOCAL Xint32 CXAPI_onUpdate(Xvoid) {
	if (cxapiExports.onUpdate != XAPI_NULL) {
		cxapiExports.onUpdate();
	}
}

XAPI_LOCAL Xint32 CXAPI_onInit(Xvoid) {
	if (cxapiExports.onInit != XAPI_NULL) {
		cxapiExports.onInit();
	}
}

XAPI_LOCAL Xint32 CXAPI_onShutdown(Xvoid) {
	if (cxapiExports.onShutdown != XAPI_NULL) {
		cxapiExports.onShutdown();
	}
}

XAPI_EXPORT Xconst XAPIDescriptor XI_query(Xvoid) {
	desc = queryCXAPIDescriptor();
	desc.sdkName = "CXAPI";
	return desc;
}

XAPI_EXPORT Xint32 XI_main(Xvoid* pParam){
	XI_createDeviceAndContext = (XI_createDeviceAndContextFn)pParam;
	
	Xconst Xint32 ret = XI_createDeviceAndContext(&device, &ctx);
	
	if (ret != XE_ERROR) {
		initCXAPI();
		// request for the APP QEventBus
		if (device->vtbl->createQEventBus != XAPI_NULL) {
			XQEventBusDescriptor qBusdesc;
			qBusdesc.name = "APP";
			Xint32 res = device->vtbl->createQEventBus(device, &qBus, qBusdesc);
		}
		else {
			return XE_ERROR; //Fatal Error!
		}
		
		CXAPI_logDebug("CXAPI running XAPI on version: 1.0.0, instantiating the mod loading phase!");

		exports.onInit = CXAPI_onInit;
		exports.onShutdown = CXAPI_onShutdown;
		exports.onUpdate = CXAPI_onUpdate;

		if (device->vtbl->setXIExports != XAPI_NULL) {
			device->vtbl->setXIExports(&device, exports);
		}
		else {
			return XE_ERROR; //Fatal Error!
		}
	}

	CXAPI_logDebug("CXAPI has been instantiated!");
	return ret != XE_ERROR;
}

XAPI_EXPORT Xint32 XI_terminate(Xvoid* pParam) {
	XI_destroyDeviceAndContext = (XI_destroyDeviceAndContextFn)pParam;
	return XI_destroyDeviceAndContext(&device, &ctx) != XE_ERROR;
}

XAPI_LOCAL Xvoid CXAPI_setOnShutdownCB(onShutdownCB cb) {
	cxapiExports.onShutdown = cb;
}

XAPI_LOCAL Xvoid CXAPI_setOnInitCB(onInitCB cb) {
	cxapiExports.onInit = cb;
}

XAPI_LOCAL Xvoid CXAPI_setOnUpdateCB(onUpdateCB cb) {
	cxapiExports.onUpdate = cb;
}

XAPI_LOCAL Xvoid CXAPI_logDebug(Xcstr msg) {
	XSEventLog log;
	log.channel = "default";
	log.msg = msg;
	log.severity = XE_EVENT_LOG_DEBUG;
	CXAPI_postEvent(XAPI_NULL, &log, XE_EVENT_TYPE_LOG);
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

Xuint32 CXAPI_postEvent(XHQEventBus* _qBus, Xvoid* event, Xconst Xuint64 type) {
	XHQEventBus thisQbus;
	if (_qBus == XAPI_NULL) {
		thisQbus.idx = qBus.idx;
	}
	else {
		thisQbus.idx = _qBus->idx;
	}
	return (ctx->vtbl->postEvent != XAPI_NULL) ? ctx->vtbl->postEvent(&thisQbus, event, type) : XE_ERROR;
}


