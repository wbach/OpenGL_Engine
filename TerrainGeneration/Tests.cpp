#include "Tests.h"
#include <iostream>
#include <vector>
#include "Terrain.h"

void NormalizationTest()
{
    struct serr
    {
        int x_;
        float min_;
        float max_;
    };
    std::vector<serr> errors;

    int loops = 100;
    //   x - ?
    int p  = 0;
    int lp = -1;
    for (int x = 1; x < loops; x++)
    {
        p = ((x * 100) / loops);
        if (lp != p)
        {
            std::cout << "\rTesting: " << p << "% Errors: " << errors.size() << std::flush;  //<< std::endl;
            lp = p;
        }

        Terrain terrain(256, 256, 20);
        terrain.Generate(15016 * x);
        // terrain.PrintMinMaxValues();
        serr e;
        e.x_   = x;
        e.min_ = terrain.GetMin();
        e.max_ = terrain.GetMax();

        if (floor(terrain.GetMin()) != 0)
            errors.push_back(e);
        if (floor(terrain.GetMax()) != 1)
            errors.push_back(e);
    }
    std::cout << std::endl;

    if (errors.empty())
        std::cout << "Test is ok." << std::endl;
    else
    {
        std::cout << "Test fail. Err count: " << errors.size() << std::endl;
        for (auto& er : errors)
        {
            std::cout << er.x_ << "," << er.min_ << "," << er.max_ << std::endl;
        }
    }
}
