#ifndef TEST_FILE_HPP
#define TEST_FILE_HPP

#include "gtest/gtest.h"
#include "dump_png.hpp"
#include "parse.hpp"

#include "directories.hpp"

void test_file(std::string case_name) {
    int width=0;
    int height=0;
    Render_World world;
    
    // Parse test scene file
    Parse(world,width,height, (getCasesDir() + case_name + ".txt").c_str());

    // Render the image
    world.Render();

    // Save the rendered image to disk
    Dump_png(world.camera.colors,width,height, (case_name + ".png").c_str());

    int width2 = 0, height2 = 0;
    Pixel* data_sol = 0;

    // Read solution from disk
    Read_png(data_sol,width2,height2,(getTestSolutionsDir() + case_name + ".png").c_str());
    assert(width==width2);
    assert(height==height2);

    // For each pixel, check to see if it matches solution
    double error = 0, total = 0;
    for(int i=0; i<height*width; i++)
    {
        vec3 a=From_Pixel(world.camera.colors[i]);
        vec3 b=From_Pixel(data_sol[i]);
        for(int c=0; c<3; c++)
        {
            double e = fabs(a[c]-b[c]);
            error += e;
            total++;
            b[c] = e;
        }
        data_sol[i]=Pixel_Color(b);
    }

    double diff = error/total*100;
    EXPECT_NEAR(0.00, diff, 1e-3);

    delete [] data_sol;
}


TEST(simple, test_0) {
    test_file("00");
}
TEST(simple, test_1) {
    test_file("01");
}
TEST(simple, test_2) {
    test_file("02");
}
TEST(simple, test_3) {
    test_file("03");
}
TEST(simple, test_4) {
    test_file("04");
}
TEST(simple, test_5) {
    test_file("05");
}
TEST(simple, test_6) {
    test_file("06");
}
TEST(simple, test_7) {
    test_file("07");
}
TEST(simple, test_8) {
    test_file("08");
}
TEST(simple, test_9) {
    test_file("09");
}
TEST(simple, test_10) {
    test_file("10");
}
TEST(simple, test_11) {
    test_file("11");
}
TEST(simple, test_12) {
    test_file("12");
}
TEST(simple, test_13) {
    test_file("13");
}
TEST(simple, test_14) {
    test_file("14");
}
TEST(simple, test_15) {
    test_file("15");
}
TEST(simple, test_16) {
    test_file("16");
}
TEST(simple, test_17) {
    test_file("17");
}
TEST(simple, test_18) {
    test_file("18");
}
TEST(simple, test_19) {
    test_file("19");
}
TEST(simple, test_20) {
    test_file("20");
}
TEST(simple, test_21) {
    test_file("21");
}
TEST(simple, test_22) {
    test_file("22");
}
TEST(simple, test_23) {
    test_file("23");
}
TEST(simple, test_24) {
    test_file("24");
}
TEST(simple, test_25) {
    test_file("25");
}
TEST(simple, test_26) {
    test_file("26");
}
TEST(simple, test_27) {
    test_file("27");
}
TEST(simple, test_28) {
    test_file("28");
}
TEST(simple, test_29) {
    test_file("29");
}


#endif // TEST_FILE_HPP
