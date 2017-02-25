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

    class stack
    {
    public:
        stack(HSQUIRRELVM hVm)
            : mVm(hVm)
        {

        }

        void set_top(SQInteger top)
        {
            sq_settop(mVm, top);
        }

        SQInteger top()
        {
            return sq_gettop(mVm);
        }

        void pop_elements(SQInteger numElements = 1)
        {
            sq_pop(mVm, numElements);
        }

        void push(const SQChar *s, SQInteger len)
        {
            sq_pushstring(mVm, s, len);
        }

        void push(const SQFloat f)
        {
            sq_pushfloat(mVm, f);
        }

        void push(const SQInteger n)
        {
            sq_pushinteger(mVm, n);
        }

        void push(const SQUserPointer p)
        {
            sq_pushuserpointer(mVm, p);
        }

        void push(const SQBool b)
        {
            sq_pushbool(mVm, b);
        }

        void push()
        {
            sq_pushnull(mVm);
        }

        SQObjectType type(SQInteger idx)
        {
            return sq_gettype(mVm, idx);
        }

        const SQChar* get_string(SQInteger idx)
        {
            assert(verify_type(idx, OT_STRING));

            const SQChar* c = nullptr;
            sq_getstring(mVm, idx, &c);
            return c;
        }
        
        bool verify_type(SQInteger idx, SQObjectType expected)
        {
            return type(idx) == expected;
        }

    private:
        HSQUIRRELVM mVm;
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
            stack s(mVmHandle);
            s.push(functionName, strlen(functionName));

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