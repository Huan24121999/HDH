#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	unsigned char bootable;
	unsigned char header_start;
	unsigned short int cylasec_start;
	unsigned char disk_format;
	unsigned char header_finish;
    unsigned short int cylasec_finish;
    unsigned int sec_relative;
    unsigned int sec_count;
} partition_struct;
partition_struct *partition;
FILE *file_pointer;

void read_partition_table();
unsigned short int* analyse(unsigned short int cylasec);



int main(int argv, char **argc)
{
    char path[10];
    if (argc[1])
		strcpy (path, argc[1]);
    else
		strcpy (path, "/dev/sda");
    
    file_pointer = fopen (path, "r");
    read_partition_table();
    fclose (file_pointer);
}

unsigned short int* analyse (unsigned short int cylasec)
{
    static unsigned short int result[2];
    result[1] = cylasec & 63;
    result[0] = ((cylasec & 65280) >> 8) | ((cylasec & 192) <<2);
    return result;
}


void read_partition_table()
{
    int i;
    unsigned short int * tmp;
    unsigned char buff[512];
    //fseek(file_pointer, 0, SEEK_SET);
    fread (buff, 512, 1, file_pointer);
    for (i = 446; i < 512; i++)
		printf(" %02X%c", buff[i], i % 16 != 13 ? ' ': '\n');
    partition = (partition_struct*) (buff + 446);
    printf ("\n\n+------+-----------------+------+-----------------+----------+----------+----------+\n");
    printf ("|      |      START      |      |      FINISH     |        SECTOR       |          |\n");
    printf ("| BOOT +-----------------+ DISK +-----------------+---------------------| CAPACITY |\n");
    printf ("|      | HDR | CYL | SEC |      | HDR | CYL | SEC | RELATIVE |   COUNT  |          |\n");
    printf ("+------+-----------------+------+-----------------+---------------------+----------+\n");

    for (i =0; i < 4; i++)
		{
			printf ("| %3s  ", partition->bootable == 0x80 ? "YES":"NO");
			printf ("| %3d ", partition->header_start);
			tmp = analyse (partition->cylasec_start);
			printf ("|%5d| %3d ", /*tmp[0]*/ partition->sec_relative / 255 / 63, tmp[1]);
			switch (partition->disk_format)
				{
				case 0x83: printf ("| Linux");
					break;
				case 0x82: printf ("| Swap ");
					break;
				case 0xAF: printf ("|  HFS ");
					break; 
				case 0x05:
				case 0x85:
				case 0x0F:
					printf ("|Extend");
					break;
				default: printf ("| %2Xh  ", partition->disk_format);
				}
			printf ("| %3d ",partition->header_finish);
			tmp=analyse (partition->cylasec_finish);
			printf ("|%5d| %3d ", /*tmp[0]*/ (partition->sec_relative + partition->sec_count) / 255 / 63 -1, tmp[1]);
			printf ("|%10d", partition->sec_relative);
			printf ("|%10d", partition->sec_count);
			printf ("| %4.1d GiB |\n", partition->sec_count / 2 / 1024 / 1024);
			printf ("+------+-----------------+------+-----------------+---------------------+----------+\n");
			partition++;
		}
}
