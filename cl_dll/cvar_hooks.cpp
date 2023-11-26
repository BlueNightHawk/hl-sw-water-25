#include "hooks.h"
#include "r_ripples.h"

void (*pgl_texturemode_hook_callback)(cvar_t* cvar);
void HOOKED_gl_texturemode_hook_callback(cvar_t* cvar);

void HOOKED_gl_texturemode_hook_callback(cvar_t* cvar)
{
	if (cvar->name && !stricmp(cvar->name, "gl_texturemode"))
		R_UpdateRippleTexParams();
	pgl_texturemode_hook_callback(cvar);
}
