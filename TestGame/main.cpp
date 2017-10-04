#include "MainScene.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include <fstream>
#include "FreeImage.h"

class Perlin
{
public:
	int repeat = 1;
	//Perlin() {}
	Perlin()
	{
		int permutation[] = {151, 160,137,91,90,15,					// Hash lookup table as defined by Ken Perlin.  This is a randomly
			131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,	// arranged array of all numbers from 0-255 inclusive.
			190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
		};

		for (int x = 0; x < 512; x++)
		{
			p[x] = permutation[x % 256];
		}
	}

	//Perlin(int repeat = -1)
	//{
	//	repeat = repeat;
	//}

	double OctavePerlin(double x, double y, double z, int octaves, double persistence)
	{
		double total = 0;
		double frequency = 4;
		double amplitude = 128;
		double maxValue = 0;			// Used for normalizing result to 0.0 - 1.0
		for (int i = 0; i < octaves; i++)
		{
			total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;

			maxValue += amplitude;

			amplitude *= persistence;
			frequency *= 2;
		}

		return total / maxValue;
	}

	int p[512]; 													// Doubled permutation to avoid overflow


	double perlin(double x, double y, double z)
	{
		if (repeat > 0)
		{									// If we have any repeat on, change the coordinates to their "local" repetitions
			x = fmod(x, repeat);
			y = fmod(y, repeat);
			z = fmod(z, repeat);
		}

		int xi = (int) x & 255;								// Calculate the "unit cube" that the point asked will be located in
		int yi = (int) y & 255;								// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
		int zi = (int) z & 255;								// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
		double xf = x - (int) x;								// We also fade the location to smooth the result.
		double yf = y - (int) y;
		double zf = z - (int) z;
		double u = fade(xf);
		double v = fade(yf);
		double w = fade(zf);

		int aaa, aba, aab, abb, baa, bba, bab, bbb;
		aaa = p[p[p[xi] + yi] + zi];
		aba = p[p[p[xi] + inc(yi)] + zi];
		aab = p[p[p[xi] + yi] + inc(zi)];
		abb = p[p[p[xi] + inc(yi)] + inc(zi)];
		baa = p[p[p[inc(xi)] + yi] + zi];
		bba = p[p[p[inc(xi)] + inc(yi)] + zi];
		bab = p[p[p[inc(xi)] + yi] + inc(zi)];
		bbb = p[p[p[inc(xi)] + inc(yi)] + inc(zi)];

		double x1, x2, y1, y2;
		x1 = lerp(grad(aaa, xf, yf, zf),				// The gradient function calculates the dot product between a pseudorandom
				  grad(baa, xf - 1, yf, zf),				// gradient vector and the vector from the input coordinate to the 8
				  u);										// surrounding points in its unit cube.
		x2 = lerp(grad(aba, xf, yf - 1, zf),				// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
				  grad(bba, xf - 1, yf - 1, zf),				// values we made earlier.
				  u);
		y1 = lerp(x1, x2, v);

		x1 = lerp(grad(aab, xf, yf, zf - 1),
				  grad(bab, xf - 1, yf, zf - 1),
				  u);
		x2 = lerp(grad(abb, xf, yf - 1, zf - 1),
				  grad(bbb, xf - 1, yf - 1, zf - 1),
				  u);
		y2 = lerp(x1, x2, v);

		return (lerp(y1, y2, w) + 1) / 2;						// For convenience we bound it to 0 - 1 (theoretical min/max before is -1 - 1)
	}

	int inc(int num)
	{
		num++;
		if (repeat > 0) num %= repeat;

		return num;
	}

	static double grad(int hash, double x, double y, double z)
	{
		int h = hash & 15;									// Take the hashed value and take the first 4 bits of it (15 == 0b1111)
		double u = h < 8 /* 0b1000 */ ? x : y;				// If the most significant bit (MSB) of the hash is 0 then set u = x.  Otherwise y.

		double v;											// In Ken Perlin's original implementation this was another conditional operator (?:).  I
															// expanded it for readability.

		if (h < 4 /* 0b0100 */)								// If the first and second significant bits are 0 set v = y
			v = y;
		else if (h == 12 /* 0b1100 */ || h == 14 /* 0b1110*/)// If the first and second significant bits are 1 set v = x
			v = x;
		else 												// If the first and second significant bits are not equal (0/1, 1/0) set v = z
			v = z;

		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v); // Use the last 2 bits to decide if u and v are positive or negative.  Then return their addition.
	}

	static double fade(double t)
	{
		// Fade function as defined by Ken Perlin.  This eases coordinate values
		// so that they will "ease" towards integral values.  This ends up smoothing
		// the final output.
		return t * t * t * (t * (t * 6 - 15) + 10);			// 6t^5 - 15t^4 + 10t^3
	}

	static double lerp(double a, double b, double x)
	{
		return a + x * (b - a);
	}
};


//#define IYMAX 512
//#define IXMAX 512 
//
//// Function to linearly interpolate between a0 and a1
//// Weight w should be in the range [0.0, 1.0]
//float lerp(float a0, float a1, float w)
//{
//	return (1.0 - w)*a0 + w*a1;
//}
//
//// Computes the dot product of the distance and gradient vectors.
//float dotGridGradient(int ix, int iy, float x, float y)
//{
//
//	// Precomputed (or otherwise) gradient vectors at each grid node
//	float Gradient[IYMAX][IXMAX][2];
//
//	// Compute the distance vector
//	float dx = x - (float) ix;
//	float dy = y - (float) iy;
//
//	// Compute the dot-product
//	return (dx*Gradient[iy][ix][0] + dy*Gradient[iy][ix][1]);
//}
//
//// Compute Perlin noise at coordinates x, y
//float perlin(float x, float y)
//{
//
//	// Determine grid cell coordinates
//	int x0 = floor(x);
//	int x1 = x0 + 1;
//	int y0 = floor(y);
//	int y1 = y0 + 1;
//
//	// Determine interpolation weights
//	// Could also use higher order polynomial/s-curve here
//	float sx = x - (float) x0;
//	float sy = y - (float) y0;
//
//	// Interpolate between grid point gradients
//	float n0, n1, ix0, ix1, value;
//	n0 = dotGridGradient(x0, y0, x, y);
//	n1 = dotGridGradient(x1, y0, x, y);
//	ix0 = lerp(n0, n1, sx);
//	n0 = dotGridGradient(x0, y1, x, y);
//	n1 = dotGridGradient(x1, y1, x, y);
//	ix1 = lerp(n0, n1, sx);
//	value = lerp(ix0, ix1, sy);
//
//	return value;
//}
//
void SaveTextureToFile(const std::string& file_name, const std::vector<int>& data, int width, int height)
{
	//FIBITMAP* Image = FreeImage_ConvertFromRawBits(data, width, height, 4, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
	
	Perlin perlin;

	FIBITMAP* bitmap = FreeImage_Allocate(width, height, 32, 8, 8, 8);

	width = FreeImage_GetWidth(bitmap);

	height = FreeImage_GetHeight(bitmap);

	int index = 0;
	for (int y = height - 1; y >= 0; y--)
	{
		BYTE *bits = FreeImage_GetScanLine(bitmap, height - 1 - y);

		for (int x = 0; x < width; x++)
		{
			/*bits[0] = data[(y*width + x) * 4 + 2];
			bits[1] = data[(y*width + x) * 4 + 1];
			bits[2] = data[(y*width + x) * 4 + 0];
			bits[3] = data[(y*width + x) * 4 + 3];*/
			//perlin.repeat = 1;
			auto xx = static_cast<double>(x) / static_cast<double>(width);
			auto yy = static_cast<double>(y) / static_cast<double>(height);
			//auto v = perlin.OctavePerlin(xx,yy , 0, 128, 0.5);
			auto v = perlin.perlin(xx, yy * 100, 0);
			std::cout <<v << ' ' << yy<<std::endl;
			auto value = static_cast<char>(v * 256);
			bits[0] = value;
			bits[1] = value;
			bits[2] = value;
			bits[3] = 255;
			bits += 4;
		}
	}

	FreeImage_FlipVertical(bitmap);
	FreeImage_Save(FIF_PNG, bitmap, file_name.c_str(), PNG_DEFAULT);
	FreeImage_Unload(bitmap);
}

std::vector<int> ReadFile(const std::string& file)
{
	std::vector<int> out;

	auto data = Utils::ReadFileBinary(file);

	int max_h = -10;

	for (const auto& c : data)
	{
		auto value = static_cast<int>(c);
		out.push_back(value);

		if (max_h < value)
			max_h = value;

		//std::cout << " " <<  value << " ";
		
		//<< std::endl;
	}


	float d = 255.f / static_cast<float>(max_h);

	for (auto& v : out)
	{
		v *= d;
	}
		

	std::cout << "Max value: " << max_h << "\nSize: " << out.size();
	return out;
}

void TestGameStart()
{
    CEngine engine;
    return;
	engine.Init();
    engine.scene = std::make_unique<MainScene>(engine);
	engine.PreperaScene();
	engine.GameLoop();
}



int main(int argc, char* argv[])
{	
	//MemoryTest();
	//auto data = ReadFile(EngineConf_GetFullDataPath("Textures/Terrain/HeightMaps/terrain.raw"));
	//SaveTextureToFile("D:\\_Projects\\OpenGL_Engine\\Data\\Textures\\Terrain\\HeightMaps\\heightmap.png", data, 513, 513);
	//return 0;
	CLogger::Instance().EnableLogs();
	TestGameStart();
	return 0;
}
