#ifndef FACULTY_FUNCTIONS
#define FACULTY_FUNCTIONS

// Semaphores are necessary joint account due the design choice I've made
#include <sys/ipc.h>
#include <sys/sem.h>

struct Faculty loggedInFaculty;
int semIdentifier;

// Function Prototypes =================================

bool faculty_op_handler(int connFD);
bool get_offering_courses(int connFD);
int add_course(int connFD);
bool remove_course(int connFD);
bool modify_course_info(int connFD);
bool change_password(int connFD);

// =====================================================

// Function Definition =================================

bool faculty_op_handler(int connFD)
{
    if (login_handler(false, false, true, connFD, NULL, &loggedInFaculty))
    {
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client

        // Get a semaphore for the user
        key_t semKey = ftok(FACULTY_FILE, loggedInFaculty.fid); // Generate a key based on the account number hence, different customers will have different semaphores

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
        strcpy(writeBuffer, FACULTY_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, FACULTY_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing FACULTY_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for FACULTY_MENU");
                return false;
            }
            
            // printf("READ BUFFER : %s\n", readBuffer);
            int choice = readBuffer[0] - '0';
            // char tbu[10];
            // bzero(tbu, sizeof(tbu));
            // sprintf(tbu, "%d", choice);
            // write(connFD, tbu, sizeof(tbu));
            switch (choice)
            {
            case 1:
                get_offering_courses(connFD);
                break;
            case 2:
                add_course(connFD);
                break;
            case 3:
                remove_course(connFD);
                break;
            case 4:
                write(connFD, "In the Switch case", sizeof("In the Switch case"));
                modify_course_info(connFD);
                break;
            case 5:
                change_password(connFD);
                break;
            default:
                writeBytes = write(connFD, FACULTY_LOGOUT, strlen(FACULTY_LOGOUT));
                return false;
            }
        }
    }
    else
    {
        // FACULTY LOGIN FAILED
        return false;
    }
    return true;
}

// ADD COURSE
int add_course(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    struct Course newCourse, previousCourse;
    // struct stat file_info;

    int customerFileDescriptor = open(COURSE_FILE, O_RDONLY);
    // if(fstat(customerFileDescriptor, &file_info) == -1){
    //     perror("Failed to get file info.");
    //     close(customerFileDescriptor);
    //     return -1;
    // }
    if (customerFileDescriptor == -1 && errno == ENOENT)
    {
        // Course file was never created
        newCourse.cid = 1;
    }
    else if (customerFileDescriptor == -1)
    {
        perror("Error while opening course file");
        return -1;
    }
    else
    {
        int offset = lseek(customerFileDescriptor, -sizeof(struct Course), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last Course record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
        int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Course record!");
            return false;
        }

        readBytes = read(customerFileDescriptor, &previousCourse, sizeof(struct Course));
        if (readBytes == -1)
        {
            perror("Error while reading Course record from file!");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLK, &lock);

        close(customerFileDescriptor);

        // char tempb[100];
        // sprintf(tempb, "eta ami %d", previousCourse.cid);
        // writeBytes = write(connFD, tempb, strlen(tempb));

        newCourse.cid = previousCourse.cid + 1;
    }

    // if (isPrimary)
    //     sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_PRIMARY, ADMIN_ADD_CUSTOMER_NAME);
    // else
    //     sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_SECONDARY, ADMIN_ADD_CUSTOMER_NAME);


    // Enter Course name
    writeBytes = write(connFD, FACULTY_ADD_COURSE_NAME, sizeof(FACULTY_ADD_COURSE_NAME));
    if (writeBytes == -1)
    {
        perror("Error writing FACULTY_ADD_COURSE_NAME message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading course name response from client!");
        ;
        return false;
    }
    strcpy(newCourse.name, readBuffer);

    // Enter Course registered count
    newCourse.count = 0;

    // Enter Course registered limit
    newCourse.limit = 150;

    // Enter Course 
    bzero(newCourse.enrolled_student, sizeof(newCourse.enrolled_student));

    // Enter Course offered by
    newCourse.offeredby = loggedInFaculty.fid;
    for(int i=0; i<100; i++){
        if(loggedInFaculty.courses_offered[i].cid == 0){
            loggedInFaculty.courses_offered[i] = newCourse;
            break;
        }
    }
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

    // char tempbuffer[300];
    // bzero(tempbuffer, sizeof(tempbuffer));
    // sprintf(tempbuffer, "ami eta %s, %d, %d, %d, %d", loggedInFaculty.courses_offered[0].name, loggedInFaculty.courses_offered[0].count, loggedInFaculty.courses_offered[0].limit, loggedInFaculty.courses_offered[0].cid, loggedInFaculty.courses_offered[0].offeredby);
    // writeBytes = write(connFD, tempbuffer, strlen(tempbuffer));


    customerFileDescriptor = open(COURSE_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (customerFileDescriptor == -1)
    {
        perror("Error while creating / opening course file!");
        return false;
    }
    writeBytes = write(customerFileDescriptor, &newCourse, sizeof(newCourse));
    if (writeBytes == -1)
    {
        perror("Error while writing course record to file!");
        return false;
    }

    close(customerFileDescriptor);

    customerFileDescriptor = open(FACULTY_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening faculty file");
        return false;
    }

    int offset = lseek(customerFileDescriptor, loggedInFaculty.fid * sizeof(struct Faculty), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required faculty record!");
        return false;
    }
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on faculty record!");
        return false;
    }

    writeBytes = write(customerFileDescriptor, &loggedInFaculty, sizeof(struct Faculty));
    if (writeBytes == -1)
    {
        perror("Error while writing update faculty info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLKW, &lock);

    close(customerFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    //sprintf(writeBuffer, "%s%s-%d\n%s%s", ADMIN_ADD_STUDENT_AUTOGEN_LOGIN, newStudent.name, newStudent.sid, ADMIN_ADD_STUDENT_AUTOGEN_PASSWORD, AUTOGEN_PASSWORD);
    strcat(writeBuffer, "New course has been added.^");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error sending customer loginID and password to the client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return newCourse.cid;
}

// VIEW OFFERING COURSES
bool get_offering_courses(int connFD) {
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];

    struct Faculty faculty;
    int customerFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Faculty), getpid()};

    int facultyId = loggedInFaculty.fid;

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
    strcat(writeBuffer, "Courses offered by you - \n");

    for(int i=0; i<100; i++) {
        if(loggedInFaculty.courses_offered[i].cid == 0){
            break;
        }
        else{
            char temp[200];
            bzero(temp, sizeof(temp));
            sprintf(temp, "\tCourse %d : %s\n", loggedInFaculty.courses_offered[i].cid, loggedInFaculty.courses_offered[i].name);
            strcat(writeBuffer, temp);
        }
    }

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

// REMOVE COURSE
bool remove_course(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    struct Course course;
    struct Faculty faculty;

    int courseId;
    int facultyId = loggedInFaculty.fid;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, FACULTY_DEL_COURSE_ID, strlen(FACULTY_DEL_COURSE_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing FACULTY_DEL_COURSE_ID message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading faculty ID from client!");
        return false;
    }

    courseId = atoi(readBuffer);

    // Faculty read
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

    // Course read
    customerFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Faculty File doesn't exist
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
    
    offset = lseek(customerFileDescriptor, (courseId-1) * sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
        // Faculty record doesn't exist
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

    struct flock lockl = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
    //struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};
    // Lock the record to be read
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lockl);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on course record!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &course, sizeof(struct Course));
    if (readBytes == -1)
    {
        perror("Error while reading course record from the file!");
        return false;
    }

    // Unlock the record
    lockl.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lockl);

    close(customerFileDescriptor);

    writeBytes = write(connFD, FACULTY_DEL_COURSE_CONF, strlen(FACULTY_DEL_COURSE_CONF));
    if (writeBytes == -1)
    {
        perror("Error while writing FACULTY_DEL_COURSE_CONF message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting faculty modification menu choice from client!");
        return false;
    }

    char choice = readBuffer[0];
    // if (choice == 0)
    // { // A non-numeric string was passed to atoi
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

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 'y':
        // writeBytes = write(connFD, ADMIN_MOD_NEW_NAME, strlen(ADMIN_MOD_NEW_NAME));
        // if (writeBytes == -1)
        // {
        //     perror("Error while writing ADMIN_MOD_NEW_NAME message to client!");
        //     return false;
        // }
        // readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        // if (readBytes == -1)
        // {
        //     perror("Error while getting response for student's new name from client!");
        //     return false;
        // }
        // strcpy(faculty.name, readBuffer);
        // break;
        for(int i=0; i<99; i++){
            if(faculty.courses_offered[i].cid == 0){
                break;
            }
            if(faculty.courses_offered[i].cid >= courseId){
                faculty.courses_offered[i] = faculty.courses_offered[i+1];
            }
        }

        // Faculty update
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

        course.count = 0;
        course.offeredby = -1;
        bzero(course.enrolled_student, sizeof(course.enrolled_student));

        // Course update
        customerFileDescriptor = open(COURSE_FILE, O_WRONLY);
        if (customerFileDescriptor == -1)
        {
            perror("Error while opening course file");
            return false;
        }
        offset = lseek(customerFileDescriptor, (courseId-1) * sizeof(struct Course), SEEK_SET);
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
            perror("Error while writing update course info into file");
        }

        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLKW, &lock);

        close(customerFileDescriptor);

        // char mess[100] = "Please login again to see your changes. :)\n";
        // writeBytes = write(connFD, mess, sizeof(mess));
        // if (writeBytes == -1)
        // {
        //     perror("Error while writing ADMIN_MOD_SUCCESS message to client!");
        //     return false;
        // }

        writeBytes = write(connFD, COURSE_DEL_SUCCESS, strlen(COURSE_DEL_SUCCESS));
        if (writeBytes == -1)
        {
            perror("Error while writing COURSE_DEL_SUCCESS message to client!");
            return false;
        }
        break;
    case 'n':
        bzero(writeBuffer, sizeof(writeBuffer));
        strcat(writeBuffer, "\n\nYou'll now be redirected to the main menu...^");

        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing redirecting message to client!");
            return false;
        }
        // readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        // if (readBytes == -1)
        // {
        //     perror("Error while getting response for customer's new age from client!");
        //     return false;
        // }
        // int updatedAge = atoi(readBuffer);
        // if (updatedAge == 0)
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
        // customer.age = updatedAge;
        break;
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

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}

// CHANGE PASSWORD
bool change_password(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    char newPassword[1000];

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
        //unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading old password response from client");
        //unlock_critical_section(&semOp);
        return false;
    }

    if (strcmp(readBuffer ,loggedInFaculty.password) == 0)
    {
        // Password matches with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
            //unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password response from client");
            //unlock_critical_section(&semOp);
            return false;
        }

        strcpy(newPassword, readBuffer);

        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
            //unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password reenter response from client");
            //unlock_critical_section(&semOp);
            return false;
        }

        if (strcmp(readBuffer, newPassword) == 0)
        {
            // New & reentered passwords match

            strcpy(loggedInFaculty.password, newPassword);

            int customerFileDescriptor = open(FACULTY_FILE, O_WRONLY);
            if (customerFileDescriptor == -1)
            {
                perror("Error opening customer file!");
                //unlock_critical_section(&semOp);
                return false;
            }

            off_t offset = lseek(customerFileDescriptor, loggedInFaculty.fid * sizeof(struct Faculty), SEEK_SET);
            if (offset == -1)
            {
                perror("Error seeking to the customer record!");
                //unlock_critical_section(&semOp);
                return false;
            }

            struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};
            int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining write lock on customer record!");
                //unlock_critical_section(&semOp);
                return false;
            }

            writeBytes = write(customerFileDescriptor, &loggedInFaculty, sizeof(struct Faculty));
            if (writeBytes == -1)
            {
                perror("Error storing updated customer password into customer record!");
                //unlock_critical_section(&semOp);
                return false;
            }

            lock.l_type = F_UNLCK;
            lockingStatus = fcntl(customerFileDescriptor, F_SETLK, &lock);

            close(customerFileDescriptor);

            writeBytes = write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

            //unlock_critical_section(&semOp);

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

    //unlock_critical_section(&semOp);
    return false;
}

// UPDATE COURSE
bool modify_course_info(int connFD) {
    write(connFD, "In The modify_course_info", strlen("In The modify_course_info"));
    char readBuffer[1000], writeBuffer[1000];
    // // bzero(readBuffer, sizeof(readBuffer));
    // // bzero(writeBuffer, sizeof(writeBuffer));
    // // write(connFD, "ami", strlen("ami"));
    ssize_t readBytes, writeBytes;
    

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    int dropped_student[150];
    struct Student students[150];
    struct Course course;
    struct Faculty faculty;
    int courseId;
    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, "What", strlen("What"));
    if (writeBytes == -1)
    {
        perror("Error while writing FACULTY_MOD_COURSE_ID message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading faculty ID from client!");
        return false;
    }

    courseId = atoi(readBuffer);
    int facultyId = loggedInFaculty.fid;
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

    int flag = 0;
    for(int i=0; i<100; i++){
        if(faculty.courses_offered[i].cid == courseId){
            course = faculty.courses_offered[i];
            flag = 1;
            break;
        }
    }
    if(flag == 0){
        writeBytes = write(connFD, FACULTY_MOD_COURSE_NOT, strlen(FACULTY_MOD_COURSE_NOT));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_MOD_COURSE_NOT message to client!");
            return false;
        }
    }

    writeBytes = write(connFD, FACULTY_MOD_COURSE_MENU, strlen(FACULTY_MOD_COURSE_MENU));
    if (writeBytes == -1)
    {
        perror("Error while writing FACULTY_MOD_COURSE_MENU message to client!");
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
    case 1: // Modify Course name
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
        strcpy(course.name, readBuffer);
        break;
    case 2: // Modify Course Limit
        writeBytes = write(connFD, ADMIN_MOD_LIMIT, strlen(ADMIN_MOD_LIMIT));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_LIMIT message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new name from client!");
            return false;
        }
        int limit = atoi(readBuffer);
        if(limit > 150){
            writeBytes = write(connFD, "Course student limit can not be greater than 150."
            , strlen("Course student limit can not be greater than 150."));
            if (writeBytes == -1)
            {
                perror("Error while writing FACULTY_MOD_COURSE_NOT message to client!");
                return false;
            }
        }

        int count = 0;
        if(limit < course.limit) {
            for(int i=limit; i<150; i++){
                if(course.enrolled_student[i] == 0){
                    break;
                }
                dropped_student[i-limit] = course.enrolled_student[i];
                count++;
            }
        }

        int customerFileDescriptor;
        customerFileDescriptor = open(STUDENT_FILE, O_RDWR);
        if (customerFileDescriptor == -1)
        {
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, "Could not open student file in read only mode.");
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
            perror("Error while seeking to required student record!");
            return false;
        }
        struct flock lock = {F_WRLCK, SEEK_SET, offset, file_info.st_size, getpid()};
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
            struct Student student;
            readBytes = read(customerFileDescriptor, &student, sizeof(struct Student));
            if (readBytes == -1)
            {
                perror("Error reading faculty record from file!");
                return false;
            }
            int k = 0;
            for(int j=0; j<count; j++){
                if(dropped_student[j] == student.sid){
                    students[k++] = student;
                }
            }
            i++;
        } while (readBytes > 0);

        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLK, &lock);
        close(customerFileDescriptor);
        
        for(int l=0; l<count; l++){
            int flag = 0;
            for (int m = 0; m < 99; m++)
            {
                if (students[l].courses_taken[m].cid == 0){
                    break;
                }
                else if (flag == 1){
                    students[l].courses_taken[m] = students[l].courses_taken[m+1];
                }
                else if (students[l].courses_taken[m].cid == courseId)
                {
                    students[l].courses_taken[m] = students[l].courses_taken[m+1];
                    flag = 1;
                }
            }
            customerFileDescriptor = open(STUDENT_FILE, O_WRONLY);
            if (customerFileDescriptor == -1)
            {
                perror("Error while opening customer file");
                return false;
            }
            offset = lseek(customerFileDescriptor, students[l].sid * sizeof(struct Student), SEEK_SET);
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

            writeBytes = write(customerFileDescriptor, &students[l], sizeof(struct Student));
            if (writeBytes == -1)
            {
                perror("Error while writing update student info into file");
            }

            lock.l_type = F_UNLCK;
            fcntl(customerFileDescriptor, F_SETLKW, &lock);

            close(customerFileDescriptor);
        }
        // strcpy(course.name, readBuffer);
        break;
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

    for(int n=0; n<100; n++){
        if(faculty.courses_offered[n].cid == course.cid){
            faculty.courses_offered[n] = course;
            break;
        }
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