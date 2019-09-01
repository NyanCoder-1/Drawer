void Fill(unsigned char r, unsigned char g, unsigned char b);
void DrawLine(int from_x, int from_y, int to_x, int to_y);
void MoveTo(int x, int y);
void LineTo(int x, int y);
void DrawRect(int x1, int y1, int x2, int y2);
void DrawEllipse(int x1, int y1, int x2, int y2);
void SetPenColor(unsigned char r, unsigned char g, unsigned char b);
void SetBrushColor(unsigned char r, unsigned char g, unsigned char b);
void SetPenTransparent();
void SetBrushTransparent();

#include <chrono>
#define _2PI 6.28318530718

double norm(double val)
{
	return val > 1 ? 1 : val < 0 ? 0 : val;
}

int hsv_rgb(float hue, float saturation, float value)
{
	struct {
		unsigned char b, g, r, a;
	} color;
	float r, g, b;
	if      (hue < 1. / 6) { r =           1; g = hue * 6 - 0; b =           0; }
	else if (hue < 2. / 6) { r = 2 - hue * 6; g =           1; b =           0; }
	else if (hue < 3. / 6) { r =           0; g =           1; b = hue * 6 - 2; }
	else if (hue < 4. / 6) { r =           0; g = 4 - hue * 6; b =           1; }
	else if (hue < 5. / 6) { r = hue * 6 - 4; g =           0; b =           1; }
	else                   { r =           1; g =           0; b = 6 - hue * 6; }
	color.r = norm((r * (saturation) + 1 - saturation) * value) * 255;
	color.g = norm((g * (saturation) + 1 - saturation) * value) * 255;
	color.b = norm((b * (saturation) + 1 - saturation) * value) * 255;
	color.a = 0;
	return *reinterpret_cast<int*>(&color);
}

long long time0;
void Init()
{
	time0 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
void Draw()
{
	SetBrushColor(255, 100, 50);

	long long time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	long long dtime = time - time0;
	float f = (dtime % 10000) / 9999.;
	int _color = hsv_rgb(f, 1, 1);
	typedef struct {
		unsigned char b, g, r, a;
	} color;
	auto c = *reinterpret_cast<color*>(&_color);
	SetPenColor(c.r, c.g, c.b);

	DrawLine(0.1f * 640, -0.02f * 480, -0.02f * 640, 0.1f * 480);
	DrawLine(0.9f * 640, -0.02f * 480, 1.02f * 640, 0.1f * 480);
	DrawLine(0.1f * 640, 1.02f * 480, -0.02f * 640, 0.9f * 480);
	DrawLine(0.9f * 640, 1.02f * 480, 1.02f * 640, 0.9f * 480);

	DrawLine(100 + f * 300, 300, 100 + f * 300, 350);

	DrawRect(600, 10, 400, 100);

	SetBrushColor(255, 200, 50);
	DrawEllipse(400, 10, 200, 200);
	SetPenTransparent();
	DrawEllipse(600, 10, 400, 200);
	SetPenColor(10, 150, 255);
	SetBrushTransparent();
	DrawEllipse(500, 10, 300, 200);
}

// 640, 480;

// 0.0, 0.0; 1.0, 0.0;
// 0.0, 1.0; 1.0, 1.0;
// ( 0.1 , -0.02), (-0.02,  0.1 ); ( 0.9 , -0.02), ( 1.02,  0.1 );
// ( 0.1 ,  1.02), (-0.02,  0.9 ); ( 0.9 ,  1.02), ( 1.02,  0.9 );