#ifndef FACULTY_RECORD
#define FACULTY_RECORD

#include "./courses.h"

struct Faculty
{
    int fid;
    char password[20];
    char name[50];
    struct Course courses_offered[100];
};

#endif