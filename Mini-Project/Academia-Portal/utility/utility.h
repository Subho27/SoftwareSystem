#ifndef UTILITY_FUNCTIONS
#define UTILITY_FUNCTIONS

#include <stdio.h>     // Import for `printf` & `perror`
#include <unistd.h>    // Import for `read`, `write & `lseek`
#include <string.h>    // Import for string functions
#include <stdbool.h>   // Import for `bool` data type
#include <sys/types.h> // Import for `open`, `lseek`
#include <sys/stat.h>  // Import for `open`
#include <fcntl.h>     // Import for `open`
#include <stdlib.h>    // Import for `atoi`
#include <errno.h>     // Import for `errno`

#include "../roles/student.h"
#include "../roles/admin.h"
#include "../roles/faculty.h"
// #include "./admin-credentials.h"
#include "./server-constants.h"

// Function Prototypes =================================

bool login_handler(bool isAdmin, bool isStudent, bool isFaculty, int connFD, struct Student *ptrToStudentID, struct Faculty *ptrToFacultyID);
// bool get_account_details(int connFD, struct Account *customerAccount);
bool get_student_details(int connFD, int studentID);
bool get_faculty_details(int connFD, int facultyID);
// bool get_transaction_details(int connFD, int accountNumber);

// =====================================================

// Function Definition =================================

bool login_handler(bool isAdmin, bool isStudent, bool isFaculty, int connFD, struct Student *ptrToStudentID, struct Faculty *ptrToFacultyID)
{
    ssize_t readBytes, writeBytes;            // Number of bytes written to / read from the socket
    char readBuffer[1000], writeBuffer[1000]; // Buffer for reading from / writing to the client
    char tempBuffer[1000];
    struct Student student;
    struct Faculty faculty;

    int ID;

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    // Get login message for respective user type
    if (isAdmin)
        strcpy(writeBuffer, ADMIN_LOGIN_WELCOME);
    else if(isFaculty)
        strcpy(writeBuffer, FACULTY_LOGIN_WELCOME);
    else
        strcpy(writeBuffer, STUDENT_LOGIN_WELCOME);

    // Append the request for LOGIN ID message
    strcat(writeBuffer, "\n");
    strcat(writeBuffer, LOGIN_ID);

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing WELCOME & LOGIN_ID message to the client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading login ID from client!");
        return false;
    }

    bool userFound = false;

    if (isAdmin)
    {
        if (strcmp(readBuffer, ADMIN_LOGIN_ID) == 0)
            userFound = true;
    }
    else if(isFaculty)
    {
        bzero(tempBuffer, sizeof(tempBuffer));
        strcpy(tempBuffer, readBuffer);
        //strtok(tempBuffer, "-");
        //ID = atoi(strtok(NULL, "-"));
        ID = atoi(tempBuffer);

        int customerFileFD = open(FACULTY_FILE, O_RDONLY);
        if (customerFileFD == -1)
        {
            perror("Error opening faculty file in read mode!");
            return false;
        }

        off_t offset = lseek(customerFileFD, ID * sizeof(struct Faculty), SEEK_SET);
        if (offset >= 0)
        {
            struct flock lock = {F_RDLCK, SEEK_SET, ID * sizeof(struct Faculty), sizeof(struct Faculty), getpid()};

            int lockingStatus = fcntl(customerFileFD, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining read lock on faculty record!");
                return false;
            }

            readBytes = read(customerFileFD, &faculty, sizeof(struct Faculty));
            if (readBytes == -1)
            {
                ;
                perror("Error reading faculty record from file!");
            }

            lock.l_type = F_UNLCK;
            fcntl(customerFileFD, F_SETLK, &lock);

            // if (strcmp(faculty.fid, readBuffer) == 0)
            //     userFound = true;
            if (faculty.fid == ID)
                userFound = true;

            close(customerFileFD);
        }
        else
        {
            writeBytes = write(connFD, FACULTY_LOGIN_ID_DOESNT_EXIT, strlen(FACULTY_LOGIN_ID_DOESNT_EXIT));
        }
    }
    else{
        bzero(tempBuffer, sizeof(tempBuffer));
        strcpy(tempBuffer, readBuffer);
        //strtok(tempBuffer, "-");
        //ID = atoi(strtok(NULL, "-"));
        ID = atoi(tempBuffer);

        int customerFileFD = open(STUDENT_FILE, O_RDONLY);
        if (customerFileFD == -1)
        {
            perror("Error opening student file in read mode!");
            return false;
        }

        off_t offset = lseek(customerFileFD, ID * sizeof(struct Student), SEEK_SET);
        if (offset >= 0)
        {
            struct flock lock = {F_RDLCK, SEEK_SET, ID * sizeof(struct Student), sizeof(struct Student), getpid()};

            int lockingStatus = fcntl(customerFileFD, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining read lock on student record!");
                return false;
            }

            readBytes = read(customerFileFD, &student, sizeof(struct Student));
            if (readBytes == -1)
            {
                ;
                perror("Error reading student record from file!");
            }

            lock.l_type = F_UNLCK;
            fcntl(customerFileFD, F_SETLK, &lock);

            // if (strcmp(student.sid, readBuffer) == 0)
            //     userFound = true;
            if (student.sid == ID){
                if(student.active[0] == '0'){
                    writeBytes = write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
                    if (writeBytes == -1)
                    {
                        perror("Error writing ACCOUNT_DEACTIVATED message to client!");
                        return false;
                    }
                    writeBytes = write(connFD, STUDENT_LOGOUT, strlen(STUDENT_LOGOUT));
                    return false;
                }
                userFound = true;
            }
            close(customerFileFD);
        }
        else
        {
            writeBytes = write(connFD, STUDENT_LOGIN_ID_DOESNT_EXIT, strlen(STUDENT_LOGIN_ID_DOESNT_EXIT));
        }
    }

    if (userFound)
    {
        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, PASSWORD, strlen(PASSWORD));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD message to client!");
            return false;
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == 1)
        {
            perror("Error reading password from the client!");
            return false;
        }

        //char hashedPassword[1000];
        //strcpy(hashedPassword, crypt(readBuffer, SALT_BAE));

        if (isAdmin)
        {
            // if (strcmp(hashedPassword, ADMIN_PASSWORD) == 0)
            //     return true;
            if (strcmp(readBuffer, ADMIN_PASSWORD) == 0)
                return true;
        }
        else if(isFaculty)
        {
            // if (strcmp(hashedPassword, customer.password) == 0)
            // {
            //     *ptrToCustomerID = customer;
            //     return true;
            // }
            if (strcmp(readBuffer, faculty.password) == 0)
            {
                *ptrToFacultyID = faculty;
                return true;
            }
        }
        else
        {
            // if (strcmp(hashedPassword, customer.password) == 0)
            // {
            //     *ptrToCustomerID = customer;
            //     return true;
            // }
            if (strcmp(readBuffer, student.password) == 0)
            {
                *ptrToStudentID = student;
                return true;
            }
        }

        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, INVALID_PASSWORD, strlen(INVALID_PASSWORD));
    }
    else
    {
        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, INVALID_LOGIN, strlen(INVALID_LOGIN));
    }

    return false;
}

// Get Student Details with Student ID
bool get_student_details(int connFD, int studentId)
{
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];

    struct Student student;
    int customerFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Student), getpid()};

    if (studentId == -1)
    {
        writeBytes = write(connFD, GET_STUDENT_ID, strlen(GET_STUDENT_ID));
        if (writeBytes == -1)
        {
            perror("Error while writing GET_STUDENT_ID message to client!");
            return false;
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error getting student ID from client!");
            ;
            return false;
        }

        studentId = atoi(readBuffer);
    }
    // char testbuffer[100];
    // bzero(testbuffer, sizeof(testbuffer));
    // sprintf(testbuffer, "Eta ami %d",studentId);
    // writeBytes = write(connFD, testbuffer, strlen(testbuffer));

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

    struct stat file_info;

    if(fstat(customerFileDescriptor, &file_info) == -1){
        perror("Failed to get file info.");
        close(customerFileDescriptor);
        return -1;
    }

    // bzero(testbuffer, sizeof(testbuffer));
    // sprintf(testbuffer, "Eta ami %ld, size %ld",studentId * sizeof(struct Student), file_info.st_size);
    // writeBytes = write(connFD, testbuffer, strlen(testbuffer));

    if(studentId * sizeof(struct Student) >= file_info.st_size) {
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

    int offset = lseek(customerFileDescriptor, studentId * sizeof(struct Student), SEEK_SET);
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
    lock.l_start = offset;

    int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Student file!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &student, sizeof(struct Student));
    if (readBytes == -1)
    {
        perror("Error reading student record from file!");
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "Student Details - \n\tID : %d\n\tName : %s\n\tActive : %s\n", 
    student.sid, student.name, student.active);

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

// Get Faculty Details with Faculty ID
bool get_faculty_details(int connFD, int facultyId)
{
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];

    struct Faculty faculty;
    int customerFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Faculty), getpid()};

    if (facultyId == -1)
    {
        writeBytes = write(connFD, GET_FACULTY_ID, strlen(GET_FACULTY_ID));
        if (writeBytes == -1)
        {
            perror("Error while writing GET_FACULTY_ID message to client!");
            return false;
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error getting faculty ID from client!");
            ;
            return false;
        }

        facultyId = atoi(readBuffer);
    }

    customerFileDescriptor = open(FACULTY_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Faculty File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    struct stat file_info;

    if(fstat(customerFileDescriptor, &file_info) == -1){
        perror("Failed to get file info.");
        close(customerFileDescriptor);
        return -1;
    }

    // bzero(testbuffer, sizeof(testbuffer));
    // sprintf(testbuffer, "Eta ami %ld, size %ld",studentId * sizeof(struct Student), file_info.st_size);
    // writeBytes = write(connFD, testbuffer, strlen(testbuffer));

    if(facultyId * sizeof(struct Faculty) >= file_info.st_size) {
        // Student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    int offset = lseek(customerFileDescriptor, facultyId * sizeof(struct Faculty), SEEK_SET);
    if (errno == EINVAL)
    {
        // Faculty record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
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
    lock.l_start = offset;

    int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the Faculty file!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &faculty, sizeof(struct Faculty));
    if (readBytes == -1)
    {
        perror("Error reading faculty record from file!");
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "Faculty Details - \n\tID : %d\n\tName : %s\n", 
    faculty.fid, faculty.name);

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

#endif