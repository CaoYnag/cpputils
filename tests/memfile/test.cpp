#include <iostream>
using namespace std;

void test_memfile1()
{
    // in windows, use 'NUL'
    FILE* fp = fopen("/dev/null", "wb");
    char buff[32];
    setbuf(fp, buff);
    fwrite("hello,world", 12, 1, fp);
    fclose(fp);
    cout << buff << endl;
}
void test_memfile2()
{
    FILE* fp = fopen("/dev/null", "wb");
    char buff[32];
    setbuffer(fp, buff, 32);
    fwrite("hello,world", 12, 1, fp);
    fclose(fp);
    cout << buff << endl;
}
void test_memfile3()
{
    char buff[32];
    FILE* fp = fmemopen(buff, 32, "wb");
    fwrite("hello,world", 12, 1, fp);
    fclose(fp);
    cout << buff << endl;
}

int main(int argc, char* argv[])
{
    test_memfile1();
    test_memfile2();
    test_memfile3();
    return 0;
}