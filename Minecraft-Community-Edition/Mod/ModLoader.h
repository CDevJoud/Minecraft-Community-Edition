#pragma once
#define XAPI_USE_V1_0_0
#include "XAPI.h"
#include <Core/QEventBus.hpp>

Xvoid setGlobalQEventBus(mce::core::QEventBus* qBus);

Xint32 mce_createDeviceAndContext(XIDevice** device, XIContext** ctx);
Xint32 mce_destroyDeviceAndContext(XIDevice** device, XIContext** ctx);

Xconst XIExports mce_pullSessionsExports();
