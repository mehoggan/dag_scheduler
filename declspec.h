#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__) || \
    defined(__CYGWIN__) || defined(__MINGW32__)
    #if defined(DAGTASKS_SHARED)
        #if defined(DAGTASKS_EXPORTS)
            #define DLLSPEC_DAGTASKS __declspec(dllexport)
        #else
            #define DLLSPEC_DAGTASKS __declspec(dllimport)
        #endif
    #elif defined(DAGTASKS_STATIC)
        #define DLLSPEC_DAGTASKS
    #else
        #define DLLSPEC_DAGTASKS
    #endif
#else
    #define DLLSPEC_DAGTASKS
#endif
