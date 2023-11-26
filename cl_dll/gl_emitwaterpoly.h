#pragma once

bool ApplyFogParams();

extern void (*pEmitWaterPolys)(struct msurface_s* fa, int direction);
extern void HOOKED_EmitWaterPolys(struct msurface_s* fa, int direction);