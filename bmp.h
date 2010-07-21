#pragma once

struct Color {
	Color(){}
	Color(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r), g(_g), b(_b) {}
	unsigned char r, g, b;
	bool operator==(const Color &col) const {
		return (r == col.r && g == col.g && b == col.b);
	}
	void set(unsigned char _r, unsigned char _g, unsigned char _b) {
		r = _r;
		g = _g;
		b = _b;
	}
};

extern Color White, Black;

// 24-bit or 32-bit
struct Image {
	unsigned int width, height;
	unsigned char *data;
	bool LoadFromBmpFile(char *filename);
	Color &Pixels(unsigned int x, unsigned int y);
	void Clear();

private:
	void ConvertColors();
	unsigned int trueWidth;
	unsigned int stride;
};

