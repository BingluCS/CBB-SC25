#include<string>

#ifdef __cplusplus
extern "C" {  
#endif
    void readFile(const char* key);

    void writeFile(const char* key);
#ifdef __cplusplus
}
#endif

void readFile(std::string key);

void writeFile(std::string key);
