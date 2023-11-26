#pragma once

extern void(APIENTRY* pglFogf)(GLenum pname, GLfloat param);
extern void APIENTRY HOOKED_glFogf(GLenum pname, GLfloat param);