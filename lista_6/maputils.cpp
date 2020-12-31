#include <iostream>
#include <fstream>

#define NATIVE_MAP_RESOLUTION 1201
#define SQ(x) ((x) * (x))

using std::vector;
using std::string;

void readfile(string filename, short int *mapdump) {
    std::ifstream mapdata(filename, std::ios::out | std::ios::binary);

    if (!mapdata) {
        std::cout << "failed to open file " << filename << std::endl << "terminating...";
        exit(1);
    }

    char *recvdata = (char *)malloc(2 * SQ(NATIVE_MAP_RESOLUTION) * sizeof(char));
    mapdata.read(recvdata, 2 * SQ(NATIVE_MAP_RESOLUTION));
    
    for(int i = 0 ; i < NATIVE_MAP_RESOLUTION; i++)
        for(int j = 0 ; j < NATIVE_MAP_RESOLUTION; j++) {
            int xcoord = i;
            int ycoord = (NATIVE_MAP_RESOLUTION + (j-i))%NATIVE_MAP_RESOLUTION;
            int index = NATIVE_MAP_RESOLUTION * xcoord + ycoord;
            int findex = NATIVE_MAP_RESOLUTION * i + j;
            mapdump[index] = recvdata[2 * findex + 0] << 8 | recvdata[2 * findex + 1];
        }

    free(recvdata);
}

void downsample(short int *map_input, short int *map_output, int input_map_edge, int compression_edge_ratio) {

}

short int * readfile_and_downsample(string filename, int downsample) {
    short int *originalmap = (short int *)malloc(SQ(NATIVE_MAP_RESOLUTION) * sizeof(short int));
    readfile(filename, originalmap);
    return originalmap;
}