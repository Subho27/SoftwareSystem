#ifndef STUDENT_FUNCTIONS
#define STUDENT_FUNCTIONS

// Semaphores are necessary joint account due the design choice I've made
// #include <sys/ipc.h>
// #include <sys/sem.h>
#include "../roles/student.h"

struct Student loggedInStudent;
int semIdentifier;

// Function Prototypes =================================

bool student_op_handler(int connFD);
bool get_course_details(int connFD);
bool enroll_course(int connFD);
bool drop_course(int connFD);
bool change_password_student(int connFD);
bool get_enrolled_courses(int connFD);
// bool unlock_critical_section(struct sembuf *sem_op);
// void write_transaction_to_array(int *transactionArray, int ID);
// int write_transaction_to_file(int accountNumber, long int oldBalance, long int newBalance, bool operation);

// =====================================================

// Function Definition =================================

bool student_op_handler(int connFD)
{
    if (login_handler(false, true, false, connFD, &loggedInStudent, NULL))
    {
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client
        bzero(readBuffer, sizeof(readBuffer));
        bzero(writeBuffer, sizeof(writeBuffer));

        // Get a semaphore for the user
        key_t semKey = ftok(STUDENT_FILE, loggedInStudent.sid); // Generate a key based on the account number hence, different customers will have different semaphores

        union semun
        {
            int val; // Value of the semaphore
        } semSet;

        int semctlStatus;
        semIdentifier = semget(semKey, 1, 0); // Get the semaphore if it exists
        if (semIdentifier == -1)
        {
            semIdentifier = semget(semKey, 1, IPC_CREAT | 0700); // Create a new semaphore
            if (semIdentifier == -1)
            {
                perror("Error while creating semaphore!");
                _exit(1);
            }

            semSet.val = 1; // Set a binary semaphore
            semctlStatus = semctl(semIdentifier, 0, SETVAL, semSet);
            if (semctlStatus == -1)
            {
                perror("Error while initializing a binary sempahore!");
                _exit(1);
            }
        }

        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, STUDENT_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing STUDENT_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for STUDENT_MENU");
                return false;
            }

            // printf("READ BUFFER : %s\n", readBuffer);
            int choice = atoi(readBuffer);
            // printf("CHOICE : %d\n", choice);
            switch (choice)
            {
            case 1:
                get_course_details(connFD);
                break;
            case 2:
                enroll_course(connFD);
                break;
            case 3:
                drop_course(connFD);
                break;
            case 4:
                get_enrolled_courses(connFD);
                break;
            case 5:
                change_password_student(connFD);
                break;
            default:
                writeBytes = write(connFD, STUDENT_LOGOUT, strlen(STUDENT_LOGOUT));
                return false;
            }
        }
    }
    else
    {
        // STUDENT LOGIN FAILED
        return false;
    }
    return true;
}

// VIEW ALL COURSES
bool get_course_details(int connFD)
{
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];
    struct Course coursesBuffer[1000];

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(tempBuffer, sizeof(tempBuffer));

    struct Faculty faculty;
    int customerFileDescriptor;

    // if (facultyId == -1)
    // {
    //     writeBytes = write(connFD, GET_FACULTY_ID, strlen(GET_FACULTY_ID));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing GET_FACULTY_ID message to client!");
    //         return false;
    //     }

    //     bzero(readBuffer, sizeof(readBuffer));
    //     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    //     if (readBytes == -1)
    //     {
    //         perror("Error getting faculty ID from client!");
    //         ;
    //         return false;
    //     }

    //     facultyId = atoi(readBuffer);
    // }

    customerFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Faculty File doesn't exist
        // bzero(writeBuffer, sizeof(writeBuffer));
        // strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        // strcat(writeBuffer, "^");
        // writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        // if (writeBytes == -1)
        // {
        //     perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
        //     return false;
        // }
        // readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // return false;
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "Could not open course file in read only mode.");
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FAILED_TO_OPEN_FILE message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    struct stat file_info;

    if (fstat(customerFileDescriptor, &file_info) == -1)
    {
        perror("Failed to get file info.");
        close(customerFileDescriptor);
        return -1;
    }

    // bzero(testbuffer, sizeof(testbuffer));
    // sprintf(testbuffer, "Eta ami %ld, size %ld",studentId * sizeof(struct Student), file_info.st_size);
    // writeBytes = write(connFD, testbuffer, strlen(testbuffer));

    // if(facultyId * sizeof(struct Faculty) >= file_info.st_size) {
    //     // Student record doesn't exist
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
    //     strcat(writeBuffer, "^");
    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    //     return false;
    // }

    int offset = lseek(customerFileDescriptor, 0, SEEK_SET);
    if (errno == EINVAL)
    {
        // Faculty record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "Could not set seek pointer.");
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing POINTER_SEEK_PROBLEM message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required faculty record!");
        return false;
    }
    struct flock lock = {F_RDLCK, SEEK_SET, offset, file_info.st_size, getpid()};
    lock.l_start = offset;

    int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Faculty file!");
        return false;
    }

    int i = 0;
    do
    {
        struct Course course;
        readBytes = read(customerFileDescriptor, &course, sizeof(struct Course));
        if (readBytes == -1)
        {
            perror("Error reading faculty record from file!");
            return false;
        }
        coursesBuffer[i] = course;
        i++;
    } while (readBytes > 0);

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    bzero(writeBuffer, sizeof(writeBuffer));
    strcat(writeBuffer, "Courses available for you to pick - \n");

    for (int i = 0; i < 1000; i++)
    {
        if (coursesBuffer[i].cid >= 1 && coursesBuffer[i].cid == coursesBuffer[i - 1].cid)
        {
            break;
        }
        else if (coursesBuffer[i].cid == 0)
        {
            break;
        }
        else
        {
            if (coursesBuffer[i].offeredby != -1)
            {
                char temp[200];
                bzero(temp, sizeof(temp));
                sprintf(temp, "\tCourse %d : %s\n", coursesBuffer[i].cid, coursesBuffer[i].name);
                strcat(writeBuffer, temp);
            }
        }
    }

    // bzero(writeBuffer, sizeof(writeBuffer));
    // sprintf(writeBuffer, "%s, %s, %s, %s, %s, %s, %s, %s, %s, %s",
    // coursesBuffer[0].name, coursesBuffer[1].name, coursesBuffer[2].name,
    // coursesBuffer[3].name, coursesBuffer[4].name, coursesBuffer[5].name,
    // coursesBuffer[6].name, coursesBuffer[7].name, coursesBuffer[8].name,
    // coursesBuffer[9].name);

    strcat(writeBuffer, "\n\nYou'll now be redirected to the main menu...^");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing faculty info to client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}

// CHANGE PASSWORD
bool change_password_student(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    char newPassword[1000];

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(newPassword, sizeof(newPassword));
    // Lock the critical section
    // struct sembuf semOp = {0, -1, SEM_UNDO};
    // int semopStatus = semop(semIdentifier, &semOp, 1);
    // if (semopStatus == -1)
    // {
    //     perror("Error while locking critical section");
    //     return false;
    // }

    writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS));
    if (writeBytes == -1)
    {
        perror("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
        // unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading old password response from client");
        // unlock_critical_section(&semOp);
        return false;
    }

    if (strcmp(readBuffer, loggedInStudent.password) == 0)
    {
        // Password matches with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
            // unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password response from client");
            // unlock_critical_section(&semOp);
            return false;
        }

        strcpy(newPassword, readBuffer);

        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
            // unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password reenter response from client");
            // unlock_critical_section(&semOp);
            return false;
        }

        if (strcmp(readBuffer, newPassword) == 0)
        {
            // New & reentered passwords match

            strcpy(loggedInStudent.password, newPassword);

            int customerFileDescriptor = open(STUDENT_FILE, O_WRONLY);
            if (customerFileDescriptor == -1)
            {
                perror("Error opening customer file!");
                // unlock_critical_section(&semOp);
                return false;
            }

            off_t offset = lseek(customerFileDescriptor, loggedInStudent.sid * sizeof(struct Faculty), SEEK_SET);
            if (offset == -1)
            {
                perror("Error seeking to the customer record!");
                // unlock_critical_section(&semOp);
                return false;
            }

            struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};
            int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining write lock on customer record!");
                // unlock_critical_section(&semOp);
                return false;
            }

            writeBytes = write(customerFileDescriptor, &loggedInStudent, sizeof(struct Student));
            if (writeBytes == -1)
            {
                perror("Error storing updated customer password into customer record!");
                // unlock_critical_section(&semOp);
                return false;
            }

            lock.l_type = F_UNLCK;
            lockingStatus = fcntl(customerFileDescriptor, F_SETLK, &lock);

            close(customerFileDescriptor);

            writeBytes = write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

            // unlock_critical_section(&semOp);

            return true;
        }
        else
        {
            // New & reentered passwords don't match
            writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
            return false;
        }
    }
    else
    {
        // Password doesn't match with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // unlock_critical_section(&semOp);

    return false;
}

// ENROLL COURSE
bool enroll_course(int connFD)
{
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(tempBuffer, sizeof(tempBuffer));
    // char t[100];
    // bzero(t, sizeof(t));
    // sprintf(t, "check : %d, %s", loggedInStudent.sid, loggedInStudent.name);
    // write(connFD, t, sizeof(t));

    struct Student student;
    struct Course course;
    int customerFileDescriptor;

    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Course), getpid()};
    int courseId = -1;
    writeBytes = write(connFD, GET_COURSE_ID, strlen(GET_COURSE_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing GET_COURSE_ID message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error getting course ID from client!");
        ;
        return false;
    }

    courseId = atoi(readBuffer);

    customerFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, COURSE_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing COURSE_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    int offset = lseek(customerFileDescriptor, (courseId - 1) * sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
        // Student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, COURSE_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing COURSE_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return false;
    }
    lock.l_start = offset;

    int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Course file!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &course, sizeof(struct Course));
    if (readBytes == -1)
    {
        perror("Error reading student record from file!");
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);
    //struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Course), getpid()};
    // int courseId = -1;
    // writeBytes = write(connFD, GET_COURSE_ID, strlen(GET_COURSE_ID));
    // if (writeBytes == -1)
    // {
    //     perror("Error while writing GET_COURSE_ID message to client!");
    //     return false;
    // }

    // bzero(readBuffer, sizeof(readBuffer));
    // readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    // if (readBytes == -1)
    // {
    //     perror("Error getting course ID from client!");
    //     ;
    //     return false;
    // }

    // courseId = atoi(readBuffer);


    // Get Student Details
    struct flock lockst = {F_RDLCK, SEEK_SET, 0, sizeof(struct Student), getpid()};
    customerFileDescriptor = open(STUDENT_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    offset = lseek(customerFileDescriptor, loggedInStudent.sid * sizeof(struct Student), SEEK_SET);
    if (errno == EINVAL)
    {
        // Student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required student record!");
        return false;
    }
    lockst.l_start = offset;

    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lockst);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Course file!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &student, sizeof(struct Student));
    if (readBytes == -1)
    {
        perror("Error reading student record from file!");
        return false;
    }

    lockst.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lockst);

    // if(student.active[0] == active) {
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcat(writeBuffer, student.name);
    //     if(active == '0')
    //         strcat(writeBuffer, " is already deactivated.\n");
    //     else
    //         strcat(writeBuffer, " is already activated.\n");

    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error writing student info to client!");
    //         return false;
    //     }
    // }
    // else{
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcat(writeBuffer, student.name);
    //     offset = lseek(customerFileDescriptor, -sizeof(struct Student), SEEK_CUR);
    //     if (offset == -1)
    //     {
    //         perror("Error seeking to last Customer record!");
    //         return false;
    //     }
    //     student.active[0] = active;
    //     writeBytes = write(customerFileDescriptor, &student, sizeof(student));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing Faculty record to file!");
    //         return false;
    //     }
    //     if(active == '0')
    //         strcat(writeBuffer, " is now deactivated.\n");
    //     else
    //         strcat(writeBuffer, " is now activated.\n");

    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error writing student info to client!");
    //         return false;
    //     }
    // }


    // Assign course to student
    for (int i = 0; i < 100; i++)
    {
        if (student.courses_taken[i].cid == 0)
        {
            student.courses_taken[i] = course;
            break;
        }
        else{
            if(student.courses_taken[i].cid == course.cid) {
                writeBytes = write(connFD, "Student is already enrolled to this course",
                 strlen("Student is already enrolled to this course"));
                if (writeBytes == -1)
                {
                    perror("Error writing student info to client!");
                    return false;
                }
                return false;
            }
        }
    }

    for (int i = 0; i < 100; i++)
    {
        if (course.enrolled_student[i] == 0)
        {
            course.enrolled_student[i] = student.sid;
            break;
        }
        else{
            if(course.enrolled_student[i] == student.sid) {
                writeBytes = write(connFD, "Student is already enrolled to this course",
                 strlen("Student is already enrolled to this course"));
                if (writeBytes == -1)
                {
                    perror("Error writing student info to client!");
                    return false;
                }
                return false;
            }
        }
    }

    customerFileDescriptor = open(STUDENT_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }
    offset = lseek(customerFileDescriptor, student.sid * sizeof(struct Student), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required student record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on student record!");
        return false;
    }

    writeBytes = write(customerFileDescriptor, &student, sizeof(struct Student));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLKW, &lock);

    close(customerFileDescriptor);

    // Update course info
    customerFileDescriptor = open(COURSE_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening course file");
        return false;
    }
    offset = lseek(customerFileDescriptor, (course.cid-1) * sizeof(struct Course), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on course record!");
        return false;
    }

    writeBytes = write(customerFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLKW, &lock);

    close(customerFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    strcat(writeBuffer, "You are now enrolled to the course.");
    // sprintf(writeBuffer, "Student Details - \n\tID : %d\n\tName : %s\n\tActive : %s\n\tCourses: %s\n",
    // student.sid, student.name, student.active, student.courses);

    strcat(writeBuffer, "\n\nYou'll now be redirected to the main menu...^");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing student info to client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}

// DROP COURSE
bool drop_course(int connFD)
{
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(tempBuffer, sizeof(tempBuffer));

    struct Student student;
    struct Course course;
    int customerFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Course), getpid()};

    int courseId = -1;
    writeBytes = write(connFD, GET_COURSE_ID_DROP, strlen(GET_COURSE_ID_DROP));
    if (writeBytes == -1)
    {
        perror("Error while writing GET_COURSE_ID_DROP message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error getting course ID from client!");
        ;
        return false;
    }

    courseId = atoi(readBuffer);

    // Get course details
    customerFileDescriptor = open(COURSE_FILE, O_RDWR);
    if (customerFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, COURSE_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing COURSE_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    int offset = lseek(customerFileDescriptor, (courseId - 1) * sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
        // Student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, COURSE_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing COURSE_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return false;
    }
    lock.l_start = offset;

    int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Course file!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &course, sizeof(struct Course));
    if (readBytes == -1)
    {
        perror("Error reading student record from file!");
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    // Get Student Details
    struct flock lockst = {F_RDLCK, SEEK_SET, 0, sizeof(struct Student), getpid()};
    customerFileDescriptor = open(STUDENT_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    offset = lseek(customerFileDescriptor, loggedInStudent.sid * sizeof(struct Student), SEEK_SET);
    if (errno == EINVAL)
    {
        // Student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required student record!");
        return false;
    }
    lockst.l_start = offset;

    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lockst);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Course file!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &student, sizeof(struct Student));
    if (readBytes == -1)
    {
        perror("Error reading student record from file!");
        return false;
    }

    lockst.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lockst);
    // if(student.active[0] == active) {
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcat(writeBuffer, student.name);
    //     if(active == '0')
    //         strcat(writeBuffer, " is already deactivated.\n");
    //     else
    //         strcat(writeBuffer, " is already activated.\n");

    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error writing student info to client!");
    //         return false;
    //     }
    // }
    // else{
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcat(writeBuffer, student.name);
    //     offset = lseek(customerFileDescriptor, -sizeof(struct Student), SEEK_CUR);
    //     if (offset == -1)
    //     {
    //         perror("Error seeking to last Customer record!");
    //         return false;
    //     }
    //     student.active[0] = active;
    //     writeBytes = write(customerFileDescriptor, &student, sizeof(student));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing Faculty record to file!");
    //         return false;
    //     }
    //     if(active == '0')
    //         strcat(writeBuffer, " is now deactivated.\n");
    //     else
    //         strcat(writeBuffer, " is now activated.\n");

    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error writing student info to client!");
    //         return false;
    //     }
    // }

    char tempbuf[1000];
    // bzero(tempbuf, sizeof(tempbuf));
    // for(int i=0; i<10; i++){
    //     char fh[100];
    //     bzero(fh, sizeof(fh));
    //     sprintf(fh, "Course %d : %d, %s\n", i, student.courses_taken[i].cid, student.courses_taken[i].name);
    //     strcat(tempbuf, fh);
    // }
    // writeBytes = write(connFD, tempbuf, sizeof(tempbuf));
    // drop course for student
    int flag = 0;
    for (int i = 0; i < 99; i++)
    {
        if (student.courses_taken[i].cid == 0){
            break;
        }
        else if (flag == 1){
            student.courses_taken[i] = student.courses_taken[i+1];
        }
        else if (student.courses_taken[i].cid == courseId)
        {
            student.courses_taken[i] = student.courses_taken[i+1];
            flag = 1;
        }
    }

    //char tempbuf[1000];
    // bzero(tempbuf, sizeof(tempbuf));
    // for(int i=0; i<10; i++){
    //     char fh[100];
    //     bzero(fh, sizeof(fh));
    //     sprintf(fh, "Course %d : %d, %s\n", i, student.courses_taken[i].cid, student.courses_taken[i].name);
    //     strcat(tempbuf, fh);
    // }
    // writeBytes = write(connFD, tempbuf, sizeof(tempbuf));

    //char tempbuf[1000];
    // bzero(tempbuf, sizeof(tempbuf));
    // for(int i=0; i<10; i++){
    //     char fh[100];
    //     bzero(fh, sizeof(fh));
    //     sprintf(fh, "Course %d : %d\n", i, course.enrolled_student[i]);
    //     strcat(tempbuf, fh);
    // }
    // writeBytes = write(connFD, tempbuf, sizeof(tempbuf));

    flag = 0;
    for (int i = 0; i < 99; i++)
    {
        if (course.enrolled_student[i] == 0){
            break;
        }
        else if (flag == 1){
            course.enrolled_student[i] = course.enrolled_student[i+1];
        }
        else if (course.enrolled_student[i] == student.sid)
        {
            course.enrolled_student[i] = course.enrolled_student[i+1];
            flag = 1;
        }
    }

    // bzero(tempbuf, sizeof(tempbuf));
    // for(int i=0; i<10; i++){
    //     char fh[100];
    //     bzero(fh, sizeof(fh));
    //     sprintf(fh, "Course %d : %d\n", i, course.enrolled_student[i]);
    //     strcat(tempbuf, fh);
    // }
    // writeBytes = write(connFD, tempbuf, sizeof(tempbuf));


    customerFileDescriptor = open(STUDENT_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }
    offset = lseek(customerFileDescriptor, student.sid * sizeof(struct Student), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required student record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on student record!");
        return false;
    }

    writeBytes = write(customerFileDescriptor, &student, sizeof(struct Student));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLKW, &lock);

    close(customerFileDescriptor);

    // Update course info
    customerFileDescriptor = open(COURSE_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening course file");
        return false;
    }
    offset = lseek(customerFileDescriptor, (course.cid-1) * sizeof(struct Course), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on course record!");
        return false;
    }

    writeBytes = write(customerFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLKW, &lock);

    close(customerFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    strcat(writeBuffer, "You are now dropped from the course.");
    // sprintf(writeBuffer, "Student Details - \n\tID : %d\n\tName : %s\n\tActive : %s\n\tCourses: %s\n",
    // student.sid, student.name, student.active, student.courses);

    strcat(writeBuffer, "\n\nYou'll now be redirected to the main menu...^");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing student info to client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}

// GET ENROLLED COURSES
bool get_enrolled_courses(int connFD) {
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];

    struct Student student = loggedInStudent;
    struct Course course;
    int customerFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Student), getpid()};

    // int courseId = -1;
    // writeBytes = write(connFD, GET_COURSE_ID_DROP, strlen(GET_COURSE_ID_DROP));
    // if (writeBytes == -1)
    // {
    //     perror("Error while writing GET_COURSE_ID_DROP message to client!");
    //     return false;
    // }

    // bzero(readBuffer, sizeof(readBuffer));
    // readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    // if (readBytes == -1)
    // {
    //     perror("Error getting course ID from client!");
    //     ;
    //     return false;
    // }

    // courseId = atoi(readBuffer);

    customerFileDescriptor = open(STUDENT_FILE, O_RDWR);
    if (customerFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    int offset = lseek(customerFileDescriptor, student.sid * sizeof(struct Student), SEEK_SET);
    if (errno == EINVAL)
    {
        // Student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required course record!");
        return false;
    }
    lock.l_start = offset;

    int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Course file!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &student, sizeof(struct Student));
    if (readBytes == -1)
    {
        perror("Error reading student record from file!");
        return false;
    }

    // if(student.active[0] == active) {
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcat(writeBuffer, student.name);
    //     if(active == '0')
    //         strcat(writeBuffer, " is already deactivated.\n");
    //     else
    //         strcat(writeBuffer, " is already activated.\n");

    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error writing student info to client!");
    //         return false;
    //     }
    // }
    // else{
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcat(writeBuffer, student.name);
    //     offset = lseek(customerFileDescriptor, -sizeof(struct Student), SEEK_CUR);
    //     if (offset == -1)
    //     {
    //         perror("Error seeking to last Customer record!");
    //         return false;
    //     }
    //     student.active[0] = active;
    //     writeBytes = write(customerFileDescriptor, &student, sizeof(student));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing Faculty record to file!");
    //         return false;
    //     }
    //     if(active == '0')
    //         strcat(writeBuffer, " is now deactivated.\n");
    //     else
    //         strcat(writeBuffer, " is now activated.\n");

    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error writing student info to client!");
    //         return false;
    //     }
    // }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    // // drop course for student
    // int flag = 0;
    // for (int i = 0; i < 99; i++)
    // {
    //     if (flag == 1){
    //         student.courses_taken[i] = student.courses_taken[i+1];
    //     }
    //     else if (student.courses_taken[i].cid == courseId)
    //     {
    //         student.courses_taken[i] = student.courses_taken[i+1];
    //         flag = 1;
    //     }
    //     else if (student.courses_taken[i].cid == 0){
    //         break;
    //     }
    // }

    // flag = 0;
    // for (int i = 0; i < 99; i++)
    // {
    //     if (flag == 1){
    //         course.enrolled_student[i] = course.enrolled_student[i+1];
    //     }
    //     else if (course.enrolled_student[i] == student.sid)
    //     {
    //         course.enrolled_student[i] = course.enrolled_student[i+1];
    //         flag = 1;
    //     }
    //     else if (course.enrolled_student[i] == 0){
    //         break;
    //     }
    // }

    // customerFileDescriptor = open(STUDENT_FILE, O_WRONLY);
    // if (customerFileDescriptor == -1)
    // {
    //     perror("Error while opening customer file");
    //     return false;
    // }
    // offset = lseek(customerFileDescriptor, student.sid * sizeof(struct Student), SEEK_SET);
    // if (offset == -1)
    // {
    //     perror("Error while seeking to required student record!");
    //     return false;
    // }

    // lock.l_type = F_WRLCK;
    // lock.l_start = offset;
    // lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    // if (lockingStatus == -1)
    // {
    //     perror("Error while obtaining write lock on student record!");
    //     return false;
    // }

    // writeBytes = write(customerFileDescriptor, &student, sizeof(struct Student));
    // if (writeBytes == -1)
    // {
    //     perror("Error while writing update student info into file");
    // }

    // lock.l_type = F_UNLCK;
    // fcntl(customerFileDescriptor, F_SETLKW, &lock);

    // close(customerFileDescriptor);

    // // Update course info
    // customerFileDescriptor = open(COURSE_FILE, O_WRONLY);
    // if (customerFileDescriptor == -1)
    // {
    //     perror("Error while opening course file");
    //     return false;
    // }
    // offset = lseek(customerFileDescriptor, (course.cid-1) * sizeof(struct Course), SEEK_SET);
    // if (offset == -1)
    // {
    //     perror("Error while seeking to required course record!");
    //     return false;
    // }

    // lock.l_type = F_WRLCK;
    // lock.l_start = offset;
    // lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    // if (lockingStatus == -1)
    // {
    //     perror("Error while obtaining write lock on course record!");
    //     return false;
    // }

    // writeBytes = write(customerFileDescriptor, &course, sizeof(struct Course));
    // if (writeBytes == -1)
    // {
    //     perror("Error while writing update student info into file");
    // }

    // lock.l_type = F_UNLCK;
    // fcntl(customerFileDescriptor, F_SETLKW, &lock);

    // close(customerFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    // sprintf(writeBuffer, "%d : %s, %d : %s, %d : %s", 
    // student.courses_taken[0].cid, student.courses_taken[0].name,
    // student.courses_taken[1].cid, student.courses_taken[1].name,
    // student.courses_taken[2].cid, student.courses_taken[2].name);
    strcat(writeBuffer, "Courses you have enrolled for - \n");

    for (int i = 0; i < 100; i++)
    {
        if (student.courses_taken[i].cid >= 1 && student.courses_taken[i].cid == student.courses_taken[i-1].cid)
        {
            break;
        }
        else if (student.courses_taken[i].cid == 0)
        {
            break;
        }
        else
        {
            char temp[200];
            bzero(temp, sizeof(temp));
            sprintf(temp, "\tCourse %d : %s\n", student.courses_taken[i].cid, student.courses_taken[i].name);
            strcat(writeBuffer, temp);
        }
    }
    // sprintf(writeBuffer, "Student Details - \n\tID : %d\n\tName : %s\n\tActive : %s\n\tCourses: %s\n",
    // student.sid, student.name, student.active, student.courses);

    strcat(writeBuffer, "\n\nYou'll now be redirected to the main menu...^");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing student info to client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}
#endif