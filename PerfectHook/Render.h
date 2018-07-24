
#pragma once

#include "Interfaces.h"
#include "ESP.h"
#include "Vector2D.h"

void Quad();



class Render
{
public:
	void SetupFonts();

	// Normal Drawing functions
	void Clear(int x, int y, int w, int h, Color color);
	void DrawOutlinedRect(int x, int y, int w, int h, Color col);
	void Text2(int x, int y, const char * _Input, int font, Color color);
	bool TransformScreen(const Vector & in, Vector & out);
	void DrawRect(int x1, int y1, int x2, int y2, Color clr);
	void Outline(int x, int y, int w, int h, Color color);
	void DrawString2(DWORD font, int x, int y, Color color, DWORD alignment, const char* msg, ...);
	void OutlineRainbow(int x, int y, int w, int h, float flSpeed, float &flRainbow);
	void Line(int x, int y, int x2, int y2, Color color);
	void PolyLine(int *x, int *y, int count, Color color);
	void Polygon(int count, Vertex_t* Vertexs, Color color);
	void PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine);
	void PolyLine(int count, Vertex_t* Vertexs, Color colorLine);
	void OutlineCircle(int x, int y, int r, int seg, Color color);
	void Pixel(int x, int y, Color col);
	void TexturedPolygon(int n, std::vector<Vertex_t> vertice, Color color);
	// Gradient Functions
	void GradientV(int x, int y, int w, int h, Color c1, Color c2);
	void GradientH(int x, int y, int w, int h, Color c1, Color c2);

	// Text functions

	void DrawFillBox(int x, int y, int w, int h, Color color);
	
	RECT GetTextSize(DWORD font, const char* text);

	Vector2D GetTextSizeWelcome(unsigned int Font, std::string Input);

	// Other rendering functions
	bool WorldToScreen(Vector &in, Vector &out);
	void Text(int x, int y, Color color, DWORD font, const char * text, ...);
	void Text(int x, int y, Color color, DWORD font, const wchar_t * text);
	void Text(int x, int y, DWORD font, const wchar_t * text);
	void Textf(int x, int y, Color color, DWORD font, const char * fmt, ...);
	RECT GetViewport();
	class Fonts
	{
	public:
		DWORD Default;
		DWORD ESP;
		DWORD Defuse;
		DWORD DroppedGuns;
		DWORD LBY;
		DWORD Watermark;
		DWORD Guns;
		DWORD NameFont;
		DWORD Welcome;
	} font;


};

extern Render* g_Render;