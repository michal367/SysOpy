#include "functions.h"


int file_exist(const char* filename){
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if(exist == 0) // file exist
        return 1;
    else
        return 0;
}


bool file_time_correct(unsigned long long file_time, int time_sign, int time_days){
    if(time_days != -1){
        unsigned long long time = sec_before_n_days(time_days);
        
        if(time_sign == 1){
            return (file_time < time);
        }
        else if(time_sign == 0){
            unsigned long long time2 = sec_before_n_days(time_days+1);
            return (file_time <= time && file_time >= time2);
        }
        else if(time_sign == -1){
            return (file_time > time);
        }
    }
    return true;
}


void print_files(const char* path, int maxdepth, 
                int atime_sign, int atime_days, 
                int mtime_sign, int mtime_days){
    if(maxdepth == -1 || maxdepth > 0){
        if(maxdepth > 0)
            maxdepth--;

        DIR * dir;
        struct dirent * de;

        char full_path[PATH_MAX];

        if((dir = opendir(path))){
            while((de = readdir(dir))){
                if(strcmp(de->d_name, "..") == 0 || 
                    strcmp(de->d_name, ".") == 0)
                    continue;
                
                strcpy(full_path, path);
                if(full_path[strlen(full_path)-1] != '/')
                    strcat(full_path, "/");
                strcat(full_path, de->d_name);

                struct stat st;
                stat(full_path, &st);

                if(file_time_correct(st.st_atime, atime_sign, atime_days) &&
                    file_time_correct(st.st_mtime, mtime_sign, mtime_days)){
                    
                    print_file_full_info(full_path, &st);
                }
                
                if(S_ISDIR(st.st_mode)){
                    print_files(full_path, maxdepth, atime_sign, atime_days, mtime_sign, mtime_days);
                }
            }
            closedir(dir);
        }
    }
}

void print_time(unsigned long long seconds)
{
    time_t time = (time_t)(seconds);
    struct tm* tm_info = localtime(&time);

    if(tm_info){
        char buffer[30];
        strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", tm_info);
        printf(buffer);
    }
}

unsigned long long sec_before_n_days(unsigned int n){
    unsigned long long sec = n * 24 * 60 * 60;
    sec = (unsigned long long)time(NULL) - sec;
    return sec;
}


void print_file_full_info(char* path, const struct stat* st){
    printf("%s \n", path);

    char* real_path = realpath(path, NULL);
    printf("\t%s\n", real_path);
    free(real_path);
    
    print_file_info(st);
}

void print_file_info(const struct stat* st){
    printf("\t");
    printf("Links: %ld", st->st_nlink);

    printf("      Type: ");
    if(S_ISBLK(st->st_mode))
        printf("block device");
    else if(S_ISCHR(st->st_mode))
        printf("character device");
    else if(S_ISDIR(st->st_mode))
        printf("directory");
    else if(S_ISFIFO(st->st_mode))
        printf("FIFO/pipe");
    else if(S_ISLNK(st->st_mode))
        printf("symlink");
    else if(S_ISREG(st->st_mode))
        printf("regular file");
    else if(S_ISSOCK(st->st_mode))
        printf("socket");
    else
        printf("unknown");

    printf("      Size(B): %ld", st->st_size);

    printf("\n\t");

    printf("atime: ");
    print_time(st->st_atime);

    printf("      mtime: ");
    print_time(st->st_mtime);

    printf("\n");
}


void print_stat(const struct stat* st){
    printf("dev \t %ld \n", st->st_dev);
    printf("ino \t %ld \n", st->st_ino);
    printf("mode \t %d \n", st->st_mode);
    printf("nlink \t %ld \n", st->st_nlink);
    printf("uid \t %d \n", st->st_uid);
    printf("gid \t %d \n", st->st_gid);
    printf("rdev \t %ld \n", st->st_rdev);
    printf("size \t %ld \n", st->st_size);
    printf("blksize\t %ld \n", st->st_blksize);
    printf("blocks \t %ld \n", st->st_blocks);
    printf("atime \t %ld \n", st->st_atime);
    printf("mtime \t %ld \n", st->st_mtime);
    printf("ctime \t %ld \n", st->st_ctime);
}

void print_dirent(const struct dirent* de){
    printf("ino \t %ld \n", de->d_ino);
    printf("name \t %s \n", de->d_name);
    printf("off \t %ld \n", de->d_off);
    printf("reclen \t %d \n", de->d_reclen);
    printf("type \t %d \n", de->d_type);
}


void help(){
    printf("find [path...] [parameters] \n\n");

    printf("Parameters: \n");
    printf("-atime \n");
    printf("Returns true if a file was last accessed n * 24 hours ago. When find figures out how many 24-hour periods ago the file was last accessed, any fractional part is ignored, so to match -atime +1, a file has to have been accessed at least two days ago.");
    printf("\n");
    printf("+n\tFor greater than n. \n");
    printf("-n\tFor less than n. \n");
    printf("n\tFor exactly n. \n");
    printf("\n");

    printf("-mtime \n");
    printf("Returns true if a file's data was last modified n * 24 hours ago. See the -atime option to understand how rounding affects the interpretation of file modification times.");
    printf("\n\n");

    printf("-maxdepth \n");
    printf("Descend at most levels (a non-negative integer) levels of directories below the command line arguments. -maxdepth 0 means only apply the tests and actions to the paths specified on the command line, and do not descend into subdirectories at all.");
    printf("\n\n");
}

