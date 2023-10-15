#ifndef STUDENT_RECORD
#define STUDENT_RECORD

#include "./courses.h"

struct Student
{
    int sid;
    char password[20];
    char name[50];
    struct Course courses_taken[100];
    char active[1];
};

#endif