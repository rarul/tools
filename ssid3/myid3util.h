#ifndef _MYID3UTIL_H_
#define _MYID3UTIL_H_

namespace MyID3Util {

const char *genre_name(unsigned char genre_code);
size_t char_length_by_byte(const char *text, unsigned char encode);
bool is_valid_frame_text(const char *text, size_t size);
char *strcpy_hex(char *dest, const char *src);
char *strcpy_charcode(char *dest, const char *src, size_t src_len, const char *charcode);
bool is_maybe_ascii(const char *src);
bool is_maybe_charcode(const char *src, size_t src_len, const char *charcode);
bool detect_charcode(const char *src, size_t src_len, char *charcode);
char *strcpy_maybe_ascii(char *dest, const char *src);
char *strcpy_maybe_charcode(char *dest, const char *src, size_t src_len, const char *charcode);

} // namespace MyID3Util

#endif /* _MYID3BASE_H_ */
