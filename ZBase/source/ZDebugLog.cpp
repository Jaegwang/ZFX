//----------//
// ZDebugLog.h //
//-------------------------------------------------------//
// author: Wanho Choi   @ Dexter Studios                 //
//         Dohyeon Yang @ Dexter Studios                 //
// last update: 2017.04.06                               //
//-------------------------------------------------------//
#include <ZelosBase.h>
ZELOS_NAMESPACE_BEGIN

///default tab_size      = 4
///default space_level   = 2
ZDebugLog::ZDebugLog(bool on) : mOn(on)
{
    mTab_size       = 4;
    mSpace_level    = 2;
    mPrev_work      = "";
}
ZDebugLog::~ZDebugLog()
{
}
void
ZDebugLog::print_tab(int tab_size)
{
    for (int i = 0; i < tab_size; i++)
        cout << " ";
}
void ZDebugLog::in(const char *fmt, ...)
{
    if(!mOn)
        return;

    clock_t in_time = clock();

    va_list args;
    va_start(args, fmt);
    std::string work = format_arg_list(fmt, args);
    va_end(args);


    print_tab(mTab_size * mWork_stack.size());
    cout << "[+] " << work;

    mWork_stack.push(work);
    mTime_stack.push(in_time);

    mPrev_work = work;
}
void ZDebugLog::out(void)
{
    if(!mOn)
        return;

    clock_t in_time, out_time;
    int print_tab_size;
    string work;
    if(mWork_stack.size() < 1 || mTime_stack.size() < 1)
    {
        cout << "\n\nERROR : DebugLog stack is empty.\n\n";
        return;
    }
    work = mWork_stack.top();
    mWork_stack.pop();

    print_tab_size = mTab_size * mWork_stack.size();
    print_tab(mTab_size * mWork_stack.size());

    in_time = mTime_stack.top();
    mTime_stack.pop();
    out_time = clock();

    if(mPrev_work == work)
    {
        cout << "[-] ";
        for (int i = 0; i < 65; ++i)
            cout << "-";
    }
    else
    {
        cout << "[-] ";
        for (int i = 0; i < 65; ++i)
            cout << "-";

    }
    cout << " " << ((double) (out_time - in_time) / CLOCKS_PER_SEC) << "s." << endl;
}
void ZDebugLog::message(const char *fmt, ...)
{
    if(!mOn)
        return;

    va_list args;
    va_start(args, fmt);
    std::string msg = format_arg_list(fmt, args);
    va_end(args);

    print_tab(mTab_size * mWork_stack.size());
    cout << " - " << msg;
}
std::string ZDebugLog::format_arg_list(const char *fmt, va_list args)
{
    if (!fmt)
        return "";
    int   result = -1, length = 256;
    char *buffer = 0;
    while (result == -1)
    {
        if (buffer) delete [] buffer;
        buffer = new char [length + 1];
        memset(buffer, 0, length + 1);
        result = vsnprintf(buffer, length, fmt, args);
        length *= 2;
    }
    std::string s(buffer);
    delete [] buffer;
    return s;
}

ZELOS_NAMESPACE_END

