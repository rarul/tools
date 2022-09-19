#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <iconv.h>


static void do_output_code(const char *in_str, size_t in_length,
                           const char *out_charcode, const char *out_printcode = nullptr) {
  if (out_printcode == nullptr) {
    out_printcode = out_charcode;
  }
  printf("[%5s] ", out_printcode);

  auto handle = iconv_open(out_charcode, "UTF-8"); // (to, from)
  if (handle == (iconv_t)-1) {
    printf("iconv_open err\n");
    return;
  }
  char *in_p = const_cast<char*>(in_str);
  auto out_len = 3*in_length;
  char out_buf[out_len];
  char *out_p = &out_buf[0];
  auto retval = iconv(handle, &in_p, &in_length, &out_p, &out_len);
  iconv_close(handle);
  if (retval == (size_t)-1) {
    printf("iconv err\n");
    return;
  }

  // go for printing
  for (char *p = &out_buf[0]; p!=out_p; p++) {
    printf("%02x", static_cast<unsigned char>(*p));
  }
  printf("\n");
}


static void do_ucode(const char *in_str, size_t in_length) {
  printf("##### %s\n", in_str);
  do_output_code(in_str, in_length, "UTF8");
  do_output_code(in_str, in_length, "UTF16LE", "UTF16");
  do_output_code(in_str, in_length, "CP932");
  do_output_code(in_str, in_length, "EUCJP");
  do_output_code(in_str, in_length, "ISO-2022-JP-3", "JIS");
}

static void usage() {
  printf("./myucode [-16 | -8 | -s | -e | -j] str1 str2 str3 ...\n"
         "\t" "-16 : str is hex UTF16 (LE BOM less)\n"
         "\t" " -8 : str is hex UTF8\n"
         "\t" " -s : str is hex CP932\n"
         "\t" " -e : str is hex EUCJP\n"
         "\t" " -j : str is hex ISO-2022-JP3\n"
         "\t" "none: str is direct UTF8\n");
  exit(1);
}

static char hex2val(const char in_str) {
  if (in_str >= 'a' && in_str <= 'f') {
    return 10 + in_str - 'a';
  } else if (in_str >= 'A' && in_str <= 'F') {
    return 10 + in_str - 'A';
  } else if (in_str >= '0' && in_str <= '9') {
    return in_str - '0';
  }
  return 0;
}

static char hex2val(const char *in_str) {
  return 16 * hex2val(in_str[0]) + hex2val(in_str[1]);
}

static int conv_to_utf8(const char *in_str, size_t in_length,
                        char *out_str, size_t out_length,
                        const char *in_charcode) {
  auto handle = iconv_open("UTF-8", in_charcode); // (to, from)
  if (handle == (iconv_t)-1) {
    return 1;
  }
  char *in_p = const_cast<char*>(in_str);
  char *out_p = out_str;
  auto retval = iconv(handle, &in_p, &in_length, &out_p, &out_length);
  iconv_close(handle);
  if (retval == (size_t)-1) {
    return 1;
  }

  out_p[0] = '\0';
  return 0;
}

int main(int argc, char *argv[]) {
  int i = 1;
  const char *cov_code = nullptr;
  for( ; i<argc; i++) {
    if (argv[i][0] != '-') {
      break;
    }
    switch (argv[i][1]) {
      case '1':
        cov_code = "UTF16LE";
        break;
      case '8':
        cov_code = "UTF8";
        break;
      case 's':
        cov_code = "CP932";
        break;
      case 'e':
        cov_code = "EUCJP";
        break;
      case 'j':
        cov_code = "ISO-2022-JP3";
        break;
      default:
        usage();
        break;
    }
  }

  if (cov_code == nullptr) {
    // input is direct UTF-8
    for( ; i<argc; i++) {
      do_ucode(argv[i], strlen(argv[i]));
    }
  } else {
    // input is something hex
    for( ; i<argc; i++) {
      auto length = strlen(argv[i]) / 2;
      char buf[length];
      for (size_t j=0; j<length; j++) {
        buf[j] = hex2val(argv[i]+2*j);

      }
      char utf8buf[3*length];
      auto ret_utf8 = conv_to_utf8(buf, length, utf8buf, 3*length, cov_code);
      if (ret_utf8 != 0) {
        printf("##### including invalid char: %s\n", argv[i]);
        continue;
      }
      do_ucode(utf8buf, strlen(utf8buf));
    }
  }
  return 0;
}
