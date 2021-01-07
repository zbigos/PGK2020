#include <iostream>
#include <fstream>
#include <dirent.h>
#define NATIVE_MAP_RESOLUTION 1201
#define SQ(x) ((x) * (x))

using std::vector;
using std::string;

vector<string> gettargets() {
    vector<string> targets;

    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir ("test/maps")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                string mname = ent->d_name;
                targets.push_back("test/maps/" + mname);
            }
            printf ("%s\n", ent->d_name);
        }

        closedir (dir);
    } else {
        std::cout << "isn't a valid directory" << std::endl << "terminating" << std::endl;
        exit(0);
    }

    std::cout << "located " << targets.size() << " maps..." << std::endl;

    return targets;
}

void readfile(string filename, unsigned short int *mapdump) {
    std::ifstream mapdata(filename, std::ios::out | std::ios::binary);

    if (!mapdata) {
        std::cout << "failed to open file " << filename << std::endl << "terminating...";
        exit(1);
    }

    unsigned char *recvdata = (unsigned char *)malloc(2 * SQ(NATIVE_MAP_RESOLUTION) * sizeof(unsigned char));
    mapdata.read((char *)recvdata, 2 * SQ(NATIVE_MAP_RESOLUTION));
    
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

unsigned short int * readfile_and_downsample(string filename, int downsample) {
    unsigned short int *originalmap = (unsigned short int *)malloc(SQ(NATIVE_MAP_RESOLUTION) * sizeof(unsigned short int));
    readfile(filename, originalmap);
    return originalmap;
}