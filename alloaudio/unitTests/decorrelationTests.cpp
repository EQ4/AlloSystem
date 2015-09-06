
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <cassert>
#include <iostream>

#include "alloaudio/al_Decorrelation.hpp"
#include "allocore/system/al_Time.hpp"


void ut_class_test(void)
{
	al::Decorrelation dec(32, 1, 1, 1000);
	assert(dec.getCurrentSeed() == 1000);
	assert(dec.getSize() == 32);

	float *ir = dec.getIR(0);
	double expected[] = {0.65027274, -0.16738815,  0.1617437 ,  0.18901241,  0.01768662,
						 -0.0802799 , -0.12612745,  0.09564361,  0.00803435,  0.07643685,
						 -0.030273  ,  0.26991193, -0.03412993, -0.05709789,  0.05474607,
						 -0.12850219,  0.03040506, -0.05887395,  0.05779415,  0.12589107,
						 0.0778308 , -0.19303948,  0.16970104, -0.34332016, -0.14030879,
						 0.02862106,  0.18978155,  0.02629568, -0.09265464, -0.04808504,
						 0.00549774,  0.26477413};
	for (int i = 0; i < 32; i++) {
//		std::cout << ir[i] << "..." << expected[i];
		assert(ir[i] - expected[i] < 0.000001);
	}
	std::cout << std::endl;

	al::Decorrelation dec2(1024, 1, 32, 1001);
	assert(dec2.getCurrentSeed() == 1001);
	assert(dec2.getSize() == 1024);


	al::Decorrelation dec3(10, 1, 8, 1001);
	assert(dec3.getCurrentSeed() == 1001);
	assert(dec3.getSize() == 0); // Size of 10 is too small

	al::Decorrelation dec4(32, 1, 0);
	assert(dec4.getSize() == 0); // Num Outs is 0
}

void ut_decorrelation_test(void)
{
	al::AudioIO io(64, 44100, 0, 0, 2, 2, al::AudioIO::DUMMY); // Dummy Audio Backend
	io.channelsBus(1);
	al::Decorrelation dec(64, 0, 1, 1000);
	dec.configure(io, 0, true);
	io.append(dec);
	float *input = io.busBuffer(0);
	for (int i = 0; i < io.framesPerBuffer(); i++) { // Zero out input bus
		input[i] = 0.0;
	}
	input[1] = 1.0;
	io.processAudio();
	float *outbuf = io.outBuffer(0);
	double expected[] = {0.0, 0.68639828, -0.21015081,  0.04274105, -0.00369917, -0.06308476,
						 0.24883819,  0.09921908, -0.02740205,  0.03255728, -0.00742716,
						-0.00136285, -0.11266077, -0.0909083 ,  0.04217425,  0.07128946,
						-0.01452214, -0.0008219 ,  0.03799216,  0.073492  , -0.04003114,
						-0.02366538,  0.07602104,  0.15514681,  0.06790056, -0.0044905 ,
						-0.10180065,  0.03126825, -0.0241807 ,  0.07766891, -0.11034507,
						 0.02519892, -0.06023501, -0.03090125,  0.07787655, -0.10905136,
						 0.09593274, -0.10025149,  0.12081278,  0.08383462,  0.03523137,
						 0.04325256, -0.0628779 , -0.05428473, -0.03601444,  0.06532053,
						 0.02946899, -0.16636388, -0.20115566, -0.12191195,  0.08616827,
						 0.00697796,  0.00775061,  0.06617171,  0.14810011,  0.0442153 ,
						-0.1437734 , -0.02805416,  0.03769239, -0.00884531, -0.1745563 ,
						 0.13952994,  0.06541837,  0.05971518}; //,  0.153454};
	for (int i = 0; i < io.framesPerBuffer(); i++) { // Zero out input bus
//		std::cout << outbuf[i] << " ... "<< expected[i] << std::endl;
		assert(fabs(expected[i] - outbuf[i]) < 0.000001);
	}

	input = io.busBuffer(0);
	for (int i = 0; i < io.framesPerBuffer(); i++) { // Zero out input bus
		input[i] = 0.0;
	}
	input[6] = 0.5;
	io.processAudio();
	outbuf = io.outBuffer(0);
	double expected2[] = {0.153454, 0.0, 0.0, 0.0, 0.0, 0.0, 0.34319914, -0.1050754 ,  0.02137052, -0.00184959, -0.03154238,
						  0.12441909,  0.04960954, -0.01370102,  0.01627864, -0.00371358,
						 -0.00068143, -0.05633038, -0.04545415,  0.02108713,  0.03564473,
						 -0.00726107, -0.00041095,  0.01899608,  0.036746  , -0.02001557,
						 -0.01183269,  0.03801052,  0.07757341,  0.03395028, -0.00224525,
						 -0.05090033,  0.01563412, -0.01209035,  0.03883445, -0.05517253,
						  0.01259946, -0.03011751, -0.01545062,  0.03893828, -0.05452568,
						  0.04796637, -0.05012575,  0.06040639,  0.04191731,  0.01761568,
						  0.02162628, -0.03143895, -0.02714236, -0.01800722,  0.03266027,
						  0.01473449, -0.08318194, -0.10057783, -0.06095598,  0.04308413,
						  0.00348898,  0.0038753 ,  0.03308586,  0.07405005,  0.02210765,
						 -0.0718867 , -0.01402708,  0.0188462 , -0.00442266, -0.08727815,
						  0.06976497,  0.03270919,  0.02985759,  0.076727};
	for (int i = 0; i < io.framesPerBuffer(); i++) { // Zero out input bus
//		std::cout << outbuf[i] << " ... "<< expected2[i] << std::endl;
		assert(fabs(expected2[i] - outbuf[i]) < 0.000001);
	}

}


#define RUNTEST(Name)\
	printf("%s ", #Name);\
	ut_##Name();\
	for(size_t i=0; i<32-strlen(#Name); ++i) printf(".");\
	printf(" pass\n")

int main()
{
	RUNTEST(class_test);
	RUNTEST(decorrelation_test);

	return 0;
}
