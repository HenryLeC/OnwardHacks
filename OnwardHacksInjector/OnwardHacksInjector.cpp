#include "Injector//injector.h"
#include <iostream> // For input / output and exceptions

int main()
{
    try
    {
        Injector injector;

        const char* char_process = "Onward.exe";
        const char* char_dll = "OnwardHacks.dll";
        wchar_t wchar_process[30], wchar_dll[30];
        size_t process_length = strlen(char_process);
        size_t dll_length = strlen(char_dll);
        mbstowcs_s(&process_length, wchar_process, char_process, process_length);
        mbstowcs_s(&dll_length, wchar_dll, char_dll, dll_length);

        injector.attach(wchar_process);
        injector.inject(wchar_dll);

        std::cout << "Injected Successfully!" << std::endl;
    }
    catch (const std::runtime_error& e) // To catch any exceptions
    {
        std::cerr << e.what() << '\n';
        system("pause");
        return 1;
    }

    return 0;
}