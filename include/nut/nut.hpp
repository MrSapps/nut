#pragma once

#include <squirrel.h>
#include <sqstdio.h> 
#include <sqstdaux.h> 

namespace nut
{
    class stack_saver
    {
    public:
        stack_saver(HSQUIRRELVM hVm)
            : mVm(hVm)
        {
            mTop = sq_gettop(mVm);
        }

        ~stack_saver()
        {
            sq_settop(mVm, mTop);
        }
    private:
        HSQUIRRELVM mVm;
        int mTop;
    };

    class vm
    {
    public:
        vm()
        {
            const SQInteger kInitialStackSize = 1024;
            mVmHandle = sq_open(kInitialStackSize);
            sq_setforeignptr(mVmHandle, this);
            sqstd_seterrorhandlers(mVmHandle);
            sq_setprintfunc(mVmHandle, printfunc, errfunc);
        }

        ~vm()
        {
            if (mVmHandle)
            {
                sq_close(mVmHandle);
            }
        }

        template<class FunctionType>
        void set_function(const char* functionName, FunctionType func)
        {

        }

        void script(const char* script)
        {
            stack_saver ss(mVmHandle);
        }

    private:
        vm* ThisFromVm(HSQUIRRELVM vm)
        {
            return reinterpret_cast<nut::vm*>(sq_getforeignptr(vm));
        }

        static void errfunc(HSQUIRRELVM /*v*/, const SQChar* s, ...)
        {
            char buffer[1024] = {};
            va_list arglist;
            va_start(arglist, s);
            vsprintf(buffer, s, arglist);
            va_end(arglist);
        }

        static void printfunc(HSQUIRRELVM /*v*/, const SQChar* s, ...)
        {
            char buffer[1024] = {};
            va_list arglist;
            va_start(arglist, s);
            vsprintf(buffer, s, arglist);
            va_end(arglist);
        }

    public:
        HSQUIRRELVM mVmHandle = nullptr;
    };
}