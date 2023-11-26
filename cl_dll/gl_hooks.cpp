#include "hooks.h"

void(APIENTRY* pglFogf)(GLenum pname, GLfloat param);
void APIENTRY HOOKED_glFogf(GLenum pname, GLfloat param);

void APIENTRY HOOKED_glFogf(GLenum pname, GLfloat param)
{
	if (!ApplyFogParams())
		pglFogf(pname, param);
}