#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>

static int g_opt_parse_byte = 1;
static int g_opt_addr_label = 1;
static int g_opt_char_extra = 0;

static void show_usage() {
  printf(
      "myhexer [-options] FILENAME [START_OFFSET] [SIZE] [HEX_CHARS]\n"
      "FILENAME : To show or edit.\n"
      "START_OFFSET : Offset of the FILENAME in HEX. (Set 0 if omitted.)\n"
      "SIZE : SIZE to show in HEX. (Set 100 if omitted.)\n)"
      "HEX_CHARS : Write to the FILENAME of OFFSET with SIZE. (Show only if ommitted.)\n"
      "-options:\n"
      "\t-n : Do not show address label.\n"
      "\t-c : Show extra characters.\n"
      "\t-2 : Little endian by 2 bytes.\n"
      "\t-4 : Little endian by 4 bytes.\n"
         );
}

static char* decode_hex_string(const char *hex_string, size_t* write_size) {
  size_t out_write_size = strlen(hex_string) / 2;
  char *out_data = static_cast<char*>(malloc(1 + out_write_size));
  for (size_t i=0; i<out_write_size; i++) {
    char transdata[3];
    memcpy (&transdata[0], &hex_string[2*i], 2);
    transdata[2] = '\0';
    char *endptr = nullptr;
    out_data[i] = static_cast<char>(strtol(transdata, &endptr, 16));
    if (endptr == nullptr) {
      printf("parse fail for invalid hex char: %s\n", transdata);
      free(out_data);
      return nullptr;
    }
  }
  *write_size = out_write_size;
  return out_data;
}

static unsigned char displayable_char(const unsigned char val) {
  if (0x20 <= val && val <= 0x7e) {
    return val;
  }
  return '.';
}
static void do_myhexer_read(const char *filename, const off_t my_offset, const size_t my_size) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    err(1, "cannot open file: %s", filename);
  }
  lseek(fd, my_offset, SEEK_SET);
  for (size_t i=0; i<my_size; i+=16) {
    unsigned char buf[16];
    memset (buf, 0, sizeof(buf));
    ssize_t read_size = read (fd, buf, sizeof(buf));
    if (read_size < 0) {
      err(1, "cannot read file");
    } else if (read_size == 0) {
      break;
    }
    // show addr label
    if (g_opt_addr_label) {
      printf("%010lx:", my_offset + i);
    }
    // show data
    for (ssize_t j=0; j<16; j+=g_opt_parse_byte) {
      switch (g_opt_parse_byte) {
        case 8:
          if (j < read_size) {
            printf(" %016llx", *reinterpret_cast<long long*>(&buf[j]));
          } else {
            printf("                 ");
          }
          break;
        case 4:
          if (j < read_size) {
            printf(" %08x", *reinterpret_cast<int*>(&buf[j]));
          } else {
            printf("         ");
          }
          break;
        case 2:
          if (j < read_size) {
            printf(" %04x", *reinterpret_cast<short*>(&buf[j]));
          } else {
            printf("     ");
          }
          break;
        case 1:
        default:
          if (j < read_size) {
            printf(" %02x", buf[j]);
          } else {
            printf("   ");
          }
          break;
      }
    }
    // show extra char
    if (g_opt_char_extra) {
      printf("  ");
      for (int i=0; i<16; i++) {
        int endian_pos = g_opt_parse_byte * (i/g_opt_parse_byte) + (g_opt_parse_byte - 1 - i%g_opt_parse_byte);
        printf("%c", displayable_char(buf[endian_pos]));
      }
    }
    printf("\n");
  }
  close(fd);
}

static void do_myhexer_write(const char *filename, const off_t my_offset, const size_t my_size, const char *write_data) {
  int fd = open(filename, O_WRONLY);
  if (fd < 0) {
    err(1, "cannot open file: %s", filename);
  }
  lseek(fd, my_offset, SEEK_SET);
  ssize_t write_size = write (fd, write_data, my_size);
  if (write_size < 0) {
    warn("write fail: %s", filename);
  } else if (write_size == 0) {
    warnx("write EOF: %s", filename);
  } else if (my_size != static_cast<size_t>(write_size)) {
    warnx("request size %lx, written size %lx", my_size, write_size);
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  // parse options
  int i = 0;
  for( i=1; i<argc; i++) {
    if (argv[i][0] != '-') {
      break;
    }
    switch (argv[i][1]) {
      case 'n':
        g_opt_addr_label = 0;
        break;
      case 'c':
        g_opt_char_extra = 1;
        break;
      case '2':
        g_opt_parse_byte = 2;
        break;
      case '4':
        g_opt_parse_byte = 4;
        break;
      case '8':
        g_opt_parse_byte = 8;
        break;
      default:
        break;
    }
  }

  const char *filename = nullptr;
  off_t my_offset = 0;
  size_t my_size = 256;
  const char *write_data = nullptr;
  // check remaining argument.
  argc -= i;
  argv += i;
  if (argc < 1) {
    show_usage();
  }
  filename = argv[0];
  if (argc > 1) {
    my_offset = strtoll(argv[1], nullptr, 16);
  }
  if (argc > 2) {
    my_size = strtoll(argv[2], nullptr, 16);
  }
  if (argc > 3) {
    size_t write_size = 0;
    write_data = decode_hex_string(argv[3], &write_size);
    if (my_size != write_size) {
      if (write_data == nullptr) {
        printf("parse fail for HEX_CHARS: %s\n", argv[2]);
      } else {
        printf("SIZE and HEX_CHARS mismatch: %lx %lx\n", my_size, write_size);
      }
      show_usage();
    }
  }

  {
    struct stat sz;
    if (stat(filename, &sz)) {
      err(1, "cannot stat file: %s", filename);
    }
    if (sz.st_size <= my_offset) {
      errx(1, "Specified size/offset is too big, size: %lx offset: %lx, filesize: %lx",
           my_size, my_offset, sz.st_size);
    }
  }
  if (write_data == nullptr) {
    do_myhexer_read(filename, my_offset, my_size);
  } else {
    do_myhexer_write(filename, my_offset, my_size, write_data);
  }
  return 0;
}
