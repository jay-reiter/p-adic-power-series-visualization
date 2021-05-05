#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
	DIR* d;
	struct dirent* dir;

	// change to ./pngs
	chdir("./pngs");
	d = opendir(".");

	unsigned n = 0;
	if(d) {
		while ((dir = readdir(d)) != NULL) {
			unsigned file_num;
			unsigned p_num;
			// read file number from string
			sscanf(dir->d_name + 4, "%u_num_%u.png", &file_num, &p_num);
			
			char* new_name = NULL;
			asprintf(&new_name, "%04u.png", file_num);
			
			printf("got digit: %u; renaming %s to %s\n", file_num, dir->d_name, new_name);

			rename(dir->d_name, new_name);
			free(new_name);

		}
		closedir(d);
	}
	printf("Num files = %u", n);
	return 0;
}
