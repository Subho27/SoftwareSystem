#ifndef COURSE_RECORD
#define COURSE_RECORD

struct Course
{
    int cid;
    char name[50];
    int offeredby;
    int count;
    int limit;
    int enrolled_student[200];
};

#endif