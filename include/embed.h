// Embedding data is easy on linux and windows (but subtly different)
// since it can be done during linking. On MacOS it can't be done at
// linking since the addresses can only be determined at runtime, and
// ASLR makes this even more of a pain.
//
// https://gareus.org/wiki/embedding_resources_in_executables
// https://stackoverflow.com/questions/28978788/crash-reading-bytes-from-getsectbyname

#ifdef __APPLE__
  #include <mach-o/getsect.h>
  #include <mach-o/ldsyms.h>

  static unsigned long __discard;

  #define EXTLD(NAME) \
    extern const unsigned char _section$__DATA__ ## NAME [];

  #define LDLEN(NAME) (getsectbyname("__DATA", "__" #NAME)->size)
  #define LDVAR(NAME) ((const char*) getsectiondata  (&_mh_execute_header, "__DATA", "__" #NAME, &__discard))

#elif (defined __WIN32__)  /* mingw */

  #define EXTLD(NAME) \
    extern const unsigned char binary_ ## NAME ## _start[]; \
    extern const unsigned char binary_ ## NAME ## _end[];

  #define LDVAR(NAME) \
    binary_ ## NAME ## _start

  #define LDLEN(NAME) \
  ((binary_ ## NAME ## _end) - (binary_ ## NAME ## _start))

#else /* gnu/linux ld */

  #define EXTLD(NAME) \
    extern const unsigned char _binary_ ## NAME ## _start[]; \
    extern const unsigned char _binary_ ## NAME ## _end[];

  #define LDVAR(NAME) \
    _binary_ ## NAME ## _start

  #define LDLEN(NAME) \
    ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))

#endif

// Extern the embedded files
EXTLD(build_bndl_lua)

