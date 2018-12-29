//
// Created by eli on 18-12-27.
//

#ifndef ERENDER_OBJ_PARSER_H
#define ERENDER_OBJ_PARSER_H
#include "../mesh/polymesh.h"
#include "../utilis/timer.h"

class ObjParser {
public :
    static int load (const char* filename,PolymeshInfo* polymeshInfo);
private:
    static Timer timer;
};


#endif //ERENDER_OBJ_PARSER_H
