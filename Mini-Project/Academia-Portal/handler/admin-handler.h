#ifndef ADMIN_FUNCTIONS
#define ADMIN_FUNCTIONS

#include "../utility/utility.h"

// Function Prototypes =================================

// bool admin_op_handler(int connFD);
int add_student(int connFD);
// bool get_student_details(connFD, NULL);
//                 break;
//             case 3: 
int add_faculty(int connFD);
//                 break;
//             case 4:
//                 get_faculty_details(connFD, NULL);
//                 break;
//             case 5:
bool activate_student(int connFD, int studentID, char active);
//                 break;
//             case 6:
//                 block_student(connFD);
//                 break;
//             case 7:
bool modify_student_info(int connFD);
//                 break;
//             case 8:
bool modify_faculty_info(int connFD);
//                 break;

// =====================================================

// Function Definition =================================

// =====================================================

bool admin_op_handler(int connFD)
{

    if (login_handler(true, false, false, connFD, NULL, NULL))
    {
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ADMIN_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, ADMIN_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ADMIN_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }

            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                add_student(connFD);
                break;
            case 2:
                get_student_details(connFD, -1);
                break;
            case 3: 
                add_faculty(connFD);
                break;
            case 4:
                get_faculty_details(connFD, -1);
                break;
            case 5:
                activate_student(connFD, -1, '1');
                break;
            case 6:
                activate_student(connFD, -1, '0');
                break;
            case 7:
                modify_student_info(connFD);
                break;
            case 8:
                modify_faculty_info(connFD);
                break;
            default:
                writeBytes = write(connFD, ADMIN_LOGOUT, strlen(ADMIN_LOGOUT));
                return false;
            }
        }
    }
    else
    {
        // ADMIN LOGIN FAILED
        return false;
    }
    return true;
}

// ADD STUDENT
int add_student(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    struct Student newStudent, previousStudent;
    // struct stat file_info;

    int customerFileDescriptor = open(STUDENT_FILE, O_RDONLY);
    // if(fstat(customerFileDescriptor, &file_info) == -1){
    //     perror("Failed to get file info.");
    //     close(customerFileDescriptor);
    //     return -1;
    // }
    if (customerFileDescriptor == -1 && errno == ENOENT)
    {
        // Student file was never created
        newStudent.sid = 0;
    }
    else if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return -1;
    }
    else
    {
        int offset = lseek(customerFileDescriptor, -sizeof(struct Student), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last Customer record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};
        int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Customer record!");
            return false;
        }

        readBytes = read(customerFileDescriptor, &previousStudent, sizeof(struct Student));
        if (readBytes == -1)
        {
            perror("Error while reading Customer record from file!");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLK, &lock);

        close(customerFileDescriptor);

        newStudent.sid = previousStudent.sid + 1;
    }

    // if (isPrimary)
    //     sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_PRIMARY, ADMIN_ADD_CUSTOMER_NAME);
    // else
    //     sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_SECONDARY, ADMIN_ADD_CUSTOMER_NAME);


    // Enter Student name
    writeBytes = write(connFD, ADMIN_ADD_STUDENT_NAME, sizeof(ADMIN_ADD_STUDENT_NAME));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADD_STUDENT_NAME message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading student name response from client!");
        ;
        return false;
    }
    strcpy(newStudent.name, readBuffer);

    //Enter Student Activeness
    strcpy(newStudent.active, "1");

    //Enter Student Password
    char hashedPassword[1000];
    strcpy(hashedPassword, AUTOGEN_PASSWORD);
    strcpy(newStudent.password, hashedPassword);

    // Enter Student Courses
    bzero(newStudent.courses_taken, sizeof(newStudent.courses_taken));

    // writeBytes = write(connFD, "1", strlen("1"));
    // if (writeBytes == -1)
    // {
    //     perror("Error writing ADMIN_ADD_CUSTOMER_GENDER message to client!");
    //     return false;
    // }

    // bzero(readBuffer, sizeof(readBuffer));
    // readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    // if (readBytes == -1)
    // {
    //     perror("Error reading customer gender response from client!");
    //     return false;
    // }

    // if (readBuffer[0] == 'M' || readBuffer[0] == 'F' || readBuffer[0] == 'O')
    //     newCustomer.gender = readBuffer[0];
    // else
    // {
    //     writeBytes = write(connFD, ADMIN_ADD_CUSTOMER_WRONG_GENDER, strlen(ADMIN_ADD_CUSTOMER_WRONG_GENDER));
    //     readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    //     return false;
    // }

    // bzero(writeBuffer, sizeof(writeBuffer));
    // strcpy(writeBuffer, ADMIN_ADD_CUSTOMER_AGE);
    // writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    // if (writeBytes == -1)
    // {
    //     perror("Error writing ADMIN_ADD_CUSTOMER_AGE message to client!");
    //     return false;
    // }

    // bzero(readBuffer, sizeof(readBuffer));
    // readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    // if (readBytes == -1)
    // {
    //     perror("Error reading customer age response from client!");
    //     return false;
    // }

    // int customerAge = atoi(readBuffer);
    // if (customerAge == 0)
    // {
    //     // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    //     return false;
    // }
    // newCustomer.age = customerAge;


    // newCustomer.account = newAccountNumber;

    // strcpy(newCustomer.login, newCustomer.name);
    // strcat(newCustomer.login, "-");
    // sprintf(writeBuffer, "%d", newCustomer.id);
    // strcat(newCustomer.login, writeBuffer);

    // char hashedPassword[1000];
    // strcpy(hashedPassword, crypt(AUTOGEN_PASSWORD, SALT_BAE));
    // strcpy(newCustomer.password, hashedPassword);

    customerFileDescriptor = open(STUDENT_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (customerFileDescriptor == -1)
    {
        perror("Error while creating / opening student file!");
        return false;
    }
    writeBytes = write(customerFileDescriptor, &newStudent, sizeof(newStudent));
    if (writeBytes == -1)
    {
        perror("Error while writing Customer record to file!");
        return false;
    }

    close(customerFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "%s%s-%d\n%s%s", ADMIN_ADD_STUDENT_AUTOGEN_LOGIN, newStudent.name, newStudent.sid, ADMIN_ADD_STUDENT_AUTOGEN_PASSWORD, AUTOGEN_PASSWORD);
    strcat(writeBuffer, "^");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error sending customer loginID and password to the client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return newStudent.sid;
}

// ADD FACULTY
int add_faculty(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    struct Faculty newFaculty, previousFaculty;
    // struct stat file_info;

    int customerFileDescriptor = open(FACULTY_FILE, O_RDONLY);
    // if(fstat(customerFileDescriptor, &file_info) == -1){
    //     perror("Failed to get file info.");
    //     close(customerFileDescriptor);
    //     return -1;
    // }
    if (customerFileDescriptor == -1 && errno == ENOENT)
    {
        // Student file was never created
        newFaculty.fid = 0;
    }
    else if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return -1;
    }
    else
    {
        int offset = lseek(customerFileDescriptor, -sizeof(struct Faculty), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last Customer record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};
        int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Faculty record!");
            return false;
        }

        readBytes = read(customerFileDescriptor, &previousFaculty, sizeof(struct Faculty));
        if (readBytes == -1)
        {
            perror("Error while reading Faculty record from file!");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLK, &lock);

        close(customerFileDescriptor);

        newFaculty.fid = previousFaculty.fid + 1;
    }

    // if (isPrimary)
    //     sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_PRIMARY, ADMIN_ADD_CUSTOMER_NAME);
    // else
    //     sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_SECONDARY, ADMIN_ADD_CUSTOMER_NAME);


    // Enter Student name
    writeBytes = write(connFD, ADMIN_ADD_FACULTY_NAME, sizeof(ADMIN_ADD_FACULTY_NAME));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADD_FACULTY_NAME message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading faculty name response from client!");
        ;
        return false;
    }
    strcpy(newFaculty.name, readBuffer);

    //Enter Student Activeness
    // strcpy(newStudent.active, "1");

    //Enter Student Password
    char hashedPassword[1000];
    strcpy(hashedPassword, AUTOGEN_PASSWORD);
    strcpy(newFaculty.password, hashedPassword);

    // Enter Student Courses
    bzero(newFaculty.courses_offered, sizeof(newFaculty.courses_offered));
    
    // writeBytes = write(connFD, "1", strlen("1"));
    // if (writeBytes == -1)
    // {
    //     perror("Error writing ADMIN_ADD_CUSTOMER_GENDER message to client!");
    //     return false;
    // }

    // bzero(readBuffer, sizeof(readBuffer));
    // readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    // if (readBytes == -1)
    // {
    //     perror("Error reading customer gender response from client!");
    //     return false;
    // }

    // if (readBuffer[0] == 'M' || readBuffer[0] == 'F' || readBuffer[0] == 'O')
    //     newCustomer.gender = readBuffer[0];
    // else
    // {
    //     writeBytes = write(connFD, ADMIN_ADD_CUSTOMER_WRONG_GENDER, strlen(ADMIN_ADD_CUSTOMER_WRONG_GENDER));
    //     readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    //     return false;
    // }

    // bzero(writeBuffer, sizeof(writeBuffer));
    // strcpy(writeBuffer, ADMIN_ADD_CUSTOMER_AGE);
    // writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    // if (writeBytes == -1)
    // {
    //     perror("Error writing ADMIN_ADD_CUSTOMER_AGE message to client!");
    //     return false;
    // }

    // bzero(readBuffer, sizeof(readBuffer));
    // readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    // if (readBytes == -1)
    // {
    //     perror("Error reading customer age response from client!");
    //     return false;
    // }

    // int customerAge = atoi(readBuffer);
    // if (customerAge == 0)
    // {
    //     // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    //     return false;
    // }
    // newCustomer.age = customerAge;


    // newCustomer.account = newAccountNumber;

    // strcpy(newCustomer.login, newCustomer.name);
    // strcat(newCustomer.login, "-");
    // sprintf(writeBuffer, "%d", newCustomer.id);
    // strcat(newCustomer.login, writeBuffer);

    // char hashedPassword[1000];
    // strcpy(hashedPassword, crypt(AUTOGEN_PASSWORD, SALT_BAE));
    // strcpy(newCustomer.password, hashedPassword);

    customerFileDescriptor = open(FACULTY_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (customerFileDescriptor == -1)
    {
        perror("Error while creating / opening faculty file!");
        return false;
    }
    writeBytes = write(customerFileDescriptor, &newFaculty, sizeof(newFaculty));
    if (writeBytes == -1)
    {
        perror("Error while writing Faculty record to file!");
        return false;
    }

    close(customerFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "%s%s-%d\n%s%s", ADMIN_ADD_FACULTY_AUTOGEN_LOGIN, newFaculty.name, newFaculty.fid, ADMIN_ADD_FACULTY_AUTOGEN_PASSWORD, AUTOGEN_PASSWORD);
    strcat(writeBuffer, "^");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error sending faculty loginID and password to the client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return newFaculty.fid;
}

//Activate & Deactivate Student
bool activate_student(int connFD, int studentId, char active)
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

    if(student.active[0] == active) {
        bzero(writeBuffer, sizeof(writeBuffer));
        strcat(writeBuffer, student.name);
        if(active == '0')
            strcat(writeBuffer, " is already deactivated.\n");
        else
            strcat(writeBuffer, " is already activated.\n");

        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error writing student info to client!");
            return false;
        }
    }
    else{
        bzero(writeBuffer, sizeof(writeBuffer));
        strcat(writeBuffer, student.name);
        offset = lseek(customerFileDescriptor, -sizeof(struct Student), SEEK_CUR);
        if (offset == -1)
        {
            perror("Error seeking to last Customer record!");
            return false;
        }
        student.active[0] = active;
        writeBytes = write(customerFileDescriptor, &student, sizeof(student));
        if (writeBytes == -1)
        {
            perror("Error while writing Faculty record to file!");
            return false;
        }
        if(active == '0')
            strcat(writeBuffer, " is now deactivated.\n");
        else
            strcat(writeBuffer, " is now activated.\n");

        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error writing student info to client!");
            return false;
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    bzero(writeBuffer, sizeof(writeBuffer));
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

// Modify Student info
bool modify_student_info(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Student student;

    int studentID;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, ADMIN_MOD_STUDENT_ID, strlen(ADMIN_MOD_STUDENT_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_STUDENT_ID message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading student ID from client!");
        return false;
    }

    studentID = atoi(readBuffer);

    int customerFileDescriptor = open(STUDENT_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Student File doesn't exist
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
    
    offset = lseek(customerFileDescriptor, studentID * sizeof(struct Student), SEEK_SET);
    if (errno == EINVAL)
    {
        // Customer record doesn't exist
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

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on student record!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &student, sizeof(struct Student));
    if (readBytes == -1)
    {
        perror("Error while reading student record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    close(customerFileDescriptor);

    writeBytes = write(connFD, ADMIN_MOD_STUDENT_MENU, strlen(ADMIN_MOD_STUDENT_MENU));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_STUDENT_MENU message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting student modification menu choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 1:
        writeBytes = write(connFD, ADMIN_MOD_NEW_NAME, strlen(ADMIN_MOD_NEW_NAME));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_NEW_NAME message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new name from client!");
            return false;
        }
        strcpy(student.name, readBuffer);
        break;
    // case 2:
    //     writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_AGE, strlen(ADMIN_MOD_CUSTOMER_NEW_AGE));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_AGE message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    //     if (readBytes == -1)
    //     {
    //         perror("Error while getting response for customer's new age from client!");
    //         return false;
    //     }
    //     int updatedAge = atoi(readBuffer);
    //     if (updatedAge == 0)
    //     {
    //         // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
    //         bzero(writeBuffer, sizeof(writeBuffer));
    //         strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
    //         writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //         if (writeBytes == -1)
    //         {
    //             perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
    //             return false;
    //         }
    //         readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    //         return false;
    //     }
    //     customer.age = updatedAge;
    //     break;
    // case 3:
    //     writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_GENDER, strlen(ADMIN_MOD_CUSTOMER_NEW_GENDER));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_GENDER message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    //     if (readBytes == -1)
    //     {
    //         perror("Error while getting response for customer's new gender from client!");
    //         return false;
    //     }
    //     customer.gender = readBuffer[0];
    //     break;
    default:
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, INVALID_MENU_CHOICE);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing INVALID_MENU_CHOICE message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    customerFileDescriptor = open(STUDENT_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }
    offset = lseek(customerFileDescriptor, studentID * sizeof(struct Student), SEEK_SET);
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

    writeBytes = write(connFD, ADMIN_MOD_SUCCESS, strlen(ADMIN_MOD_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}

// Modify Faculty info
bool modify_faculty_info(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Faculty faculty;

    int facultyId;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, ADMIN_MOD_FACULTY_ID, strlen(ADMIN_MOD_FACULTY_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_FACULTY_ID message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading faculty ID from client!");
        return false;
    }

    facultyId = atoi(readBuffer);

    int customerFileDescriptor = open(FACULTY_FILE, O_RDONLY);
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
    
    offset = lseek(customerFileDescriptor, facultyId * sizeof(struct Faculty), SEEK_SET);
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

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on faculty record!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &faculty, sizeof(struct Faculty));
    if (readBytes == -1)
    {
        perror("Error while reading faculty record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    close(customerFileDescriptor);

    writeBytes = write(connFD, ADMIN_MOD_FACULTY_MENU, strlen(ADMIN_MOD_FACULTY_MENU));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_FACULTY_MENU message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting faculty modification menu choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 1:
        writeBytes = write(connFD, ADMIN_MOD_NEW_NAME, strlen(ADMIN_MOD_NEW_NAME));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_NEW_NAME message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new name from client!");
            return false;
        }
        strcpy(faculty.name, readBuffer);
        break;
    // case 2:
    //     writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_AGE, strlen(ADMIN_MOD_CUSTOMER_NEW_AGE));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_AGE message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    //     if (readBytes == -1)
    //     {
    //         perror("Error while getting response for customer's new age from client!");
    //         return false;
    //     }
    //     int updatedAge = atoi(readBuffer);
    //     if (updatedAge == 0)
    //     {
    //         // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
    //         bzero(writeBuffer, sizeof(writeBuffer));
    //         strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
    //         writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //         if (writeBytes == -1)
    //         {
    //             perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
    //             return false;
    //         }
    //         readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    //         return false;
    //     }
    //     customer.age = updatedAge;
    //     break;
    // case 3:
    //     writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_GENDER, strlen(ADMIN_MOD_CUSTOMER_NEW_GENDER));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_GENDER message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    //     if (readBytes == -1)
    //     {
    //         perror("Error while getting response for customer's new gender from client!");
    //         return false;
    //     }
    //     customer.gender = readBuffer[0];
    //     break;
    default:
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, INVALID_MENU_CHOICE);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing INVALID_MENU_CHOICE message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    customerFileDescriptor = open(FACULTY_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening faculty file");
        return false;
    }
    offset = lseek(customerFileDescriptor, facultyId * sizeof(struct Faculty), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required faculty record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on faculty record!");
        return false;
    }

    writeBytes = write(customerFileDescriptor, &faculty, sizeof(struct Faculty));
    if (writeBytes == -1)
    {
        perror("Error while writing update faculty info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLKW, &lock);

    close(customerFileDescriptor);

    writeBytes = write(connFD, ADMIN_MOD_SUCCESS, strlen(ADMIN_MOD_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}

#endif