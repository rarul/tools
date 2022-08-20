#include <string>
#include <cstdio>
#include <cstring>
#include <vector>

#include <iconv.h>

#include "myid3util.h"


namespace MyID3Util {

// https://en.wikipedia.org/wiki/List_of_ID3v1_Genres
static const char *genre_table[] = {
    "Blues",
    "Classic Rock",
    "Country",
    "Dance",
    "Disco",
    "Funk",
    "Grunge",
    "Hip-Hop",
    "Jazz",
    "Metal",
    "New Age", // 10
    "Oldies",
    "Other",
    "Pop",
    "Rhythm and Blues",
    "Rap",
    "Reggae",
    "Rock",
    "Techno",
    "Industrial",
    "Alternative", // 20
    "Ska",
    "Death Metal",
    "Pranks",
    "Soundtrack",
    "Euro-Techno",
    "Ambient",
    "Trip-Hop",
    "Vocal",
    "Jazz & Funk",
    "Fusion", // 30
    "Trance",
    "Classical",
    "Instrumental",
    "Acid",
    "House",
    "Game",
    "Sound clip",
    "Gospel",
    "Noise",
    "Alternative Rock", // 40
    "Bass",
    "Soul",
    "Punk",
    "Space",
    "Meditative",
    "Instrumental Pop",
    "Instrumental Rock",
    "Ethnic",
    "Gothic",
    "Darkwave", // 50
    "Techno-Industrial",
    "Electronic",
    "Pop-Folk",
    "Eurodance",
    "Dream",
    "Southern Rock",
    "Comedy",
    "Cult",
    "Gangsta",
    "Top 40", // 60
    "Christian Rap",
    "Pop/Funk",
    "Jungle music",
    "Native US",
    "Cabaret",
    "New Wave",
    "Psychedelic",
    "Rave",
    "Showtunes",
    "Trailer", // 70
    "Lo-Fi",
    "Tribal",
    "Acid Punk",
    "Acid Jazz",
    "Polka",
    "Retro",
    "Musical",
    "Rock ’n’ Roll",
    "Hard Rock",
    "Folk", // 80
    "Folk-Rock",
    "National Folk",
    "Swing",
    "Fast Fusion",
    "Bebop",
    "Latin",
    "Revival",
    "Celtic",
    "Bluegrass",
    "Avantgarde", // 90
    "Gothic Rock",
    "Progressive Rock",
    "Psychedelic Rock",
    "Symphonic Rock",
    "Slow Rock",
    "Big Band",
    "Chorus",
    "Easy Listening",
    "Acoustic",
    "Humour", // 100
    "Speech",
    "Chanson",
    "Opera",
    "Chamber Music",
    "Sonata",
    "Symphony",
    "Booty Bass",
    "Primus",
    "Porn Groove",
    "Satire", // 110
    "Slow Jam",
    "Club",
    "Tango",
    "Samba",
    "Folklore",
    "Ballad",
    "Power Ballad",
    "Rhythmic Soul",
    "Freestyle",
    "Duet", // 120
    "Punk Rock",
    "Drum Solo",
    "A cappella",
    "Euro-House",
    "Dance Hall",
    "Goa music",
    "Drum & Bass",
    "Club-House",
    "Hardcore Techno",
    "Terror", // 130
    "Indie",
    "BritPop",
    "Negerpunk",
    "Polsk Punk",
    "Beat",
    "Christian Gangsta Rap",
    "Heavy Metal",
    "Black Metal",
    "Crossover",
    "Contemporary Christian", // 140
    "Christian Rock",
    "Merengue",
    "Salsa",
    "Thrash Metal",
    "Anime",
    "Jpop",
    "Synthpop",
    "Abstract",
    "Art Rock",
    "Baroque", // 150
    "Bhangra",
    "Big beat",
    "Breakbeat",
    "Chillout",
    "Downtempo",
    "Dub",
    "EBM",
    "Eclectic",
    "Electro",
    "Electroclash", // 160
    "Emo",
    "Experimental",
    "Garage",
    "Global",
    "IDM",
    "Illbient",
    "Industro-Goth",
    "Jam Band",
    "Krautrock",
    "Leftfield", // 170
    "Lounge",
    "Math Rock",
    "New Romantic",
    "Nu-Breakz",
    "Post-Punk",
    "Post-Rock",
    "Psytrance",
    "Shoegaze",
    "Space Rock",
    "Trop Rock", // 180
    "World Music",
    "Neoclassical",
    "Audiobook",
    "Audio Theatre",
    "Neue Deutsche Welle",
    "Podcast",
    "Indie-Rock",
    "G-Funk",
    "Dubstep",
    "Garage Rock", // 190
    "Psybient",
};

const char *genre_name(unsigned char genre_code) {
    if (genre_code < sizeof(genre_table)/sizeof(genre_table[0])) {
        return genre_table[genre_code];
    }
    return "(none)";
}

char *strcpy_hex(char *dest, const char *src) {
    char *retval = dest;
    while (1) {
        // fetch;
        char pval = *src;
        if (pval == '\0') {
            break;
        } else {
            sprintf(dest, "%02x", static_cast<unsigned char>(pval));
        }
        src++;
        dest += 2;
    }
    *dest = '\0';
    return retval;
}

char *strcpy_charcode(char *dest, const char *src, size_t src_len, const char *charcode) {
    if (strcasecmp(charcode, "ASCII") == 0 ||
        strcasecmp(charcode, "UTF-8") == 0) {
        return strcpy(dest, src);
    }

    if(memcmp(charcode, "UTF-16", 6) != 0) {
        src_len = 1 + strlen(src);
    } else {
        // searching "\0\0" is difficult, for there exists the case "\0\0\0".
        // so keep src_len
    }

    iconv_t ic = iconv_open("UTF-8", charcode);
    if (ic == reinterpret_cast<iconv_t>(-1)) {
        return nullptr;
    }

    char *in_p = const_cast<char*>(src);
    auto out_len = 2 * src_len;
    char *out_p = &dest[0];

    auto retval = iconv(ic, &in_p, &src_len, &out_p, &out_len);
    iconv_close(ic);
    if (retval == static_cast<size_t>(-1)) {
        return nullptr;
    }
    out_p[0] = '\0';
    return dest;
}

static bool is_maybe_8859(const char *src) {
    for (const char *p = src; ; p++) {
        // fetch
        unsigned char pval = *p;
        if (pval == '\0') {
            break;
        } else if ((0x20 <= pval && pval <= 0x7e) ||
                   (0xa0 <= pval /* always true && pval <= 0xff */)) {
            // OK
        } else {
            return false;
        }
    }
    return true;
}

bool is_maybe_ascii(const char *src) {
    for (const char *p = src; ; p++) {
        // fetch
        char pval = *p;
        if (pval == '\0') {
            break;
        } else if (!isprint(pval)) {
            return false;
        }
    }
    return true;
}

bool is_maybe_charcode(const char *src, size_t src_len, const char *charcode) {
    if (strcasecmp(charcode, "ASCII") == 0) {
        return is_maybe_ascii(src);
    }

    if(memcmp(charcode, "UTF-16", 6) != 0) {
        src_len = 1 + strlen(src);
    } else {
        // searching "\0\0" is difficult, for there exists the case "\0\0\0".
        // so keep src_len
    }

    iconv_t ic = iconv_open(charcode, charcode);
    if (ic == reinterpret_cast<iconv_t>(-1)) {
        return false;
    }

    char *in_p = const_cast<char*>(src);
    auto out_len = src_len;
    char buf[out_len];
    char *out_p = &buf[0];

    auto retval = iconv(ic, &in_p, &src_len, &out_p, &out_len);
    iconv_close(ic);
    if (retval == static_cast<size_t>(-1)) {
        return false;
    }
    return true;
}

bool detect_charcode(const char *src, size_t src_len, char *charcode) {
    if (is_maybe_ascii(src)) {
        strcpy(charcode, "ASCII");
        return true;
    }

    const std::vector<const char*> code_table = {
        "UTF-8",
        "CP932",
        "UTF-16",
#if 0
        // It seems always success to convert from "ISO-8859-1" using iconv.
        // So I prepared my original code checker.
        "ISO-8859-1",
#endif
    };
    for ( auto elem : code_table) {
        if (is_maybe_charcode(src, src_len, elem)) {
            strcpy (charcode, elem);
            return true;
        }
    }

    if (is_maybe_8859(src)) {
        strcpy(charcode, "ISO-8859-1");
        return true;
    }
    return false;
}


char *strcpy_maybe_ascii(char *dest, const char *src) {
    if (is_maybe_ascii(src)) {
        return strcpy(dest, src);
    }
    return strcpy_hex(dest, src);
}

char *strcpy_maybe_charcode(char *dest, const char *src, size_t src_len, const char *charcode) {
    if (strcasecmp(charcode, "ASCII") == 0) {
        return strcpy_maybe_ascii(dest, src);
    } else if (is_maybe_charcode(src, src_len, charcode)) {
        return strcpy_charcode(dest, src, src_len, charcode);
    }
    return strcpy_hex(dest, src);
}

} // namespace MyID3Util
