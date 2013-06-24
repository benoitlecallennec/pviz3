#ifdef NBCPP_EXPORTS
#define NBCPPDLL_API __declspec(dllexport)
#else
#define NBCPPDLL_API __declspec(dllimport)
#endif