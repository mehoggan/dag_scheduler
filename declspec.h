#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__) || \
    defined(__CYGWIN__) || defined(__MINGW32__)
    #if defined(U_DAGTASKS_SHARED)
        #if defined(U_DAGTASKS_EXPORTS)
            #define DLLSPEC_U_DAGTASKS __declspec(dllexport)
        #else
            #define DLLSPEC_U_DAGTASKS __declspec(dllimport)
        #endif
    #elif defined(U_DAGTASKS_STATIC)
        #define DLLSPEC_U_DAGTASKS
    #else
        #define DLLSPEC_U_DAGTASKS
    #endif
#else
    #define DLLSPEC_U_DAGTASKS
#endif
