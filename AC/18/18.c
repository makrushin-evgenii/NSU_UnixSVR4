#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <string.h>


#include <unistd.h>

/// struct stat {
///     dev_t         st_dev;      /* устройство */
///     ino_t         st_ino;      /* inode */
///     mode_t        st_mode;     /* режим доступа */
///     nlink_t       st_nlink;    /* количество жестких ссылок */
///     uid_t         st_uid;      /* идентификатор пользователя-владельца */
///     gid_t         st_gid;      /* идентификатор группы-владельца */
///     dev_t         st_rdev;     /* тип устройства */
///                                /* (если это устройство) */
///     off_t         st_size;     /* общий размер в байтах */
///     blksize_t     st_blksize;  /* размер блока ввода-вывода */
///                                /* в файловой системе */
///     blkcnt_t      st_blocks;   /* количество выделенных блоков */
///     time_t        st_atime;    /* время последнего доступа */
///     time_t        st_mtime;    /* время последней модификации */
///     time_t        st_ctime;    /* время последнего изменения */
/// };

#define ERROR_CODE -1
#define MAX_ERR_MSG_SIZE 300
#define RIGHTS_TYPES_NUM 9
#define RIGHTS_SYMBS_NUM 3
#define CUR_DIR "."


struct stat get_file_status(char *filename);

void print_file_info(char *filename);

char *get_file_access_rights(struct stat *status);


char *get_file_owner(struct stat *status);

char *get_file_group(struct stat *status);

char *get_file_timestamp(struct stat *status);

char *get_file_name(char *filepath);

char get_file_type(struct stat *status);

int get_file_links_num(struct stat *status);

int get_file_size(struct stat *status);


int main(int argc, char **argv) {
    if (argc < 2) {
        print_file_info(CUR_DIR);
        return EXIT_SUCCESS;
    }

    int filename_index;

    for (filename_index = 1; filename_index < argc; filename_index++) {
            print_file_info(argv[filename_index]);
    }

    return EXIT_SUCCESS;
}

void print_file_info(char *filename) {
    struct stat status;
    //  = get_file_status(filename);

    if (stat(filename, &status) == -1) {
        perror(filename);
        return;
    }

    char *access_rights = get_file_access_rights(&status);

    printf("%c%s %d %s %s %d %.19s %s\n",

           get_file_type(&status),
           access_rights,
           get_file_links_num(&status),
           get_file_owner(&status),
           get_file_group(&status),
           get_file_size(&status),
           get_file_timestamp(&status),
           get_file_name(filename));

    free(access_rights);
}


struct stat get_file_status(char *filename) {
    struct stat status;

    if (stat(filename, &status) == -1) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    return status;
}

char get_file_type(struct stat *status) {
    switch (status->st_mode & S_IFMT) {
        case S_IFREG:
            return '-';
        case S_IFDIR:
            return 'd';
        default:
            return '?';
    }
}

char *get_file_access_rights(struct stat *status) {
    char rights_symbols[RIGHTS_SYMBS_NUM] = {'r', 'w', 'x'};
    int rights_types[RIGHTS_TYPES_NUM] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH,
                                          S_IXOTH};
    char *access_rights = malloc(RIGHTS_TYPES_NUM + 1);

    for (int i = 0; i < RIGHTS_TYPES_NUM; ++i) {
        access_rights[i] = '-';
    }

    for (int right_index = 0; right_index < RIGHTS_TYPES_NUM; right_index++) {
        if (status->st_mode & rights_types[right_index]) {
            access_rights[right_index] = rights_symbols[right_index % RIGHTS_SYMBS_NUM];
        }
    }

    return access_rights;
}

int get_file_links_num(struct stat *status) {
    return (int) status->st_nlink;
}

char *get_file_owner(struct stat *status) {
    /// struct passwd {
    ///         char    *pw_name;       /* имя пользователя */
    ///         /* ... */
    /// };

    struct passwd *owner_info = getpwuid(status->st_uid);
    if (owner_info == NULL) {
        perror("GET OWNER INFO");
        exit(EXIT_FAILURE);
    }

    return owner_info->pw_name;
}

char *get_file_group(struct stat *status) {
    struct group *group_info = getgrgid(status->st_gid);
    if (group_info == NULL) {
        perror("GET OWNER INFO");
        exit(EXIT_FAILURE);
    }

    return group_info->gr_name;
}

int get_file_size(struct stat *status) {
    return (int) status->st_size;

//    if ((status->st_mode & S_IFMT) == S_IFREG) {
//        return status->st_size;
//    }
}

char *get_file_timestamp(struct stat *status) {
    char *time = ctime(&status->st_mtime);
    if (time == NULL) {
        perror("GET TIME");
        exit(EXIT_FAILURE);
    }

    return time;
}

char *get_file_name(char *filepath) {
    int filename_index = strlen(filepath) - 1;

    while (filepath[filename_index] != '/' && filename_index != 0) {
        filename_index--;
    }

    if (filename_index != 0) {
        filename_index++;
    }

    return &filepath[filename_index];
}
