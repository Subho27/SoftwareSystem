#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main()
{
    while (1)
    {
	//O_CREAT creates a file if it does not exist
        creat("./test-directory/file_1", O_CREAT);
        creat("./test-directory/file_2", O_CREAT);
        creat("./test-directory/file_3", O_CREAT);
        creat("./test-directory/file_4", O_CREAT);
        creat("./test-directory/file_5", O_CREAT);
    }
    return 0;
}
