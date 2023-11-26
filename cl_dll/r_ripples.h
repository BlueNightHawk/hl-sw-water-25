#pragma once

void R_ResetRipples(void);
void R_InitRipples(void);
void R_AnimateRipples(void);
void R_UpdateRippleTexParams(void);
float R_GetRippleTextureScale();
void R_UploadRipples(struct texture_s* image);