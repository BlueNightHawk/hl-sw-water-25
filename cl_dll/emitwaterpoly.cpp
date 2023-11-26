#include "hooks.h"

#include "studio.h"
#include "com_model.h"
#include "r_studioint.h"

#include "r_turbsin.h"
#include "r_ripples.h"

extern cvar_s* r_ripples;
extern engine_studio_api_s IEngineStudio;

#define TURBSCALE (256.0f / (M_PI))
#define SUBDIVIDE_SIZE 64

void (*pEmitWaterPolys)(struct msurface_s* fa, int direction);
void HOOKED_EmitWaterPolys(struct msurface_s* fa, int direction);

static struct
{
	uint r,g,b,fog;
} g_fogcolor;

bool ApplyFogParams()
{
	float fogColor[4];
	GLfloat forward;

	if (gHUD.refparams.waterlevel > 2 && gHUD.refparams.onlyClientDraw == false)
	{
		fogColor[3] = 1.0;
		fogColor[0] = (long double)g_fogcolor.r / 255.0;
		fogColor[1] = (long double)g_fogcolor.g / 255.0;
		fogColor[2] = (long double)g_fogcolor.b / 255.0;
		glFogfv(GL_FOG_COLOR, fogColor);
		forward = (float)(1536 - 4 * g_fogcolor.fog);
		pglFogf(GL_FOG_END, forward);
		return true;
	}
	return false;
}

void CL_EmitWaterPolys(msurface_t* fa, int direction)
{
	cl_entity_t* currententity = IEngineStudio.GetCurrentEntity();
	float time = gEngfuncs.GetClientTime();

	float texturescale;
	float *v, nv, waveHeight;
	float s, t, os, ot;
	glpoly_t* p;
	int i;

	texturescale = R_GetRippleTextureScale();

	// set the current waveheight
	if (fa->polys->verts[0][2] >= gHUD.refparams.vieworg[2])
		waveHeight = -currententity->curstate.scale;
	else
		waveHeight = currententity->curstate.scale;

	for (p = fa->polys; p; p = p->next)
	{
		if (direction > 0)
			v = p->verts[0] + (p->numverts - 1) * VERTEXSIZE;
		else
			v = p->verts[0];

		glBegin(GL_POLYGON);

		for (i = 0; i < p->numverts; i++)
		{
			if (waveHeight > 0.0f)
			{
				nv = r_turbsin[(int)(time * 160.0f + v[1] + v[0]) & 255] + 8.0f;
				nv = (r_turbsin[(int)(v[0] * 5.0f + time * 171.0f - v[1]) & 255] + 8.0f) * 0.8f + nv;
				nv = nv * waveHeight + v[2];
			}
			else
				nv = v[2];

			os = v[3];
			ot = v[4];

			if (r_ripples->value <= 0)
			{
				s = os + r_turbsin[(int)((ot * 0.125f + time) * TURBSCALE) & 255];
				t = ot + r_turbsin[(int)((os * 0.125f + time) * TURBSCALE) & 255];
			}
			else
			{
				s = os / texturescale;
				t = ot / texturescale;
			}

			s *= (1.0f / SUBDIVIDE_SIZE);
			t *= (1.0f / SUBDIVIDE_SIZE);

			glTexCoord2f(s, t);
			glVertex3f(v[0], v[1], nv);

			if (direction != 0)
				v -= VERTEXSIZE;
			else
				v += VERTEXSIZE;
		}

		glEnd();
	}
}

void SetFogParams(int r, int g, int b, int fog)
{
	g_fogcolor.r = r;
	g_fogcolor.g = g;
	g_fogcolor.b = b;
	g_fogcolor.fog = fog;
}

void HOOKED_EmitWaterPolys(struct msurface_s* fa, int direction)
{
	byte* pal;

	if (!fa->polys)
		return;

	R_UploadRipples(fa->texinfo->texture);

	pal = fa->texinfo->texture->pPal;

	SetFogParams(pal[9], pal[10], pal[11], pal[12]);
	CL_EmitWaterPolys(fa, direction);
}