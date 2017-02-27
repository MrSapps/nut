#pragma once

#include <squirrel.h>
#include <sqstdio.h> 
#include <sqstdaux.h> 
#include <type_traits>

namespace nut
{
    template <typename R, typename ... Types>
    constexpr std::integral_constant<unsigned, sizeof ...(Types)> getArgumentCount(R(*f)(Types ...))
    {
        return std::integral_constant<unsigned, sizeof ...(Types)>{};
    }

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

        template<class T>
        T Read(SQInteger index) const;

        template<>
        const SQChar* Read(SQInteger idx) const
        {
            assert(verify_type(idx, OT_STRING));

            const SQChar* c = nullptr;
            sq_getstring(mVm, idx, &c);
            return c;
        }

        template<>
        float Read(SQInteger idx) const
        {
            assert(verify_type(idx, OT_FLOAT));

            float f = 0.0f;
            sq_getfloat(mVm, idx, &f);
            return f;
        }

        template<>
        std::string Read(SQInteger idx) const
        {
            assert(verify_type(idx, OT_STRING));

            const SQChar* c = nullptr;
            sq_getstring(mVm, idx, &c);
            return c;
        }

        void push(const SQChar *s)
        {
            sq_pushstring(mVm, s, -1);
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

        template <typename T, typename... Ts>
        void push(const T value, const Ts... values) 
        {
            push(value);
            push(values...);
        }

        SQObjectType type(SQInteger idx) const
        {
            return sq_gettype(mVm, idx);
        }

        bool verify_type(SQInteger idx, SQObjectType expected) const
        {
            return type(idx) == expected;
        }

        // General type trait struct
        template <typename... Ts>
        struct pop_trait 
        {
            // base case: creates a tuple containing one element
            template <typename T>
            static auto worker(const stack& s, const int index)
            {
                return std::make_tuple(s.Read<T>(index));
            }

            // inductive case
            template <typename ArgsFirstItem, typename ArgsSecondItem, typename... ArgsRemaining>
            static auto worker(const stack& s, const int index)
            {
                auto head = std::make_tuple(s.Read<ArgsFirstItem>(index));
                return std::tuple_cat(head, worker<ArgsSecondItem, ArgsRemaining...>(s, index + 1));
            }

            static auto apply(stack& s)
            {
                const auto top = s.top();
                auto ret = worker<Ts...>(s, -static_cast<int>(sizeof...(Ts)));
                s.pop_elements(sizeof...(Ts));
                return ret;
            }
        };


        // Specialization for singular type
        template <typename T>
        struct pop_trait<T> 
        {
            static auto apply(stack &s)
            {
                auto ret = s.Read<T>(-1);
                s.pop_elements(1);
                return ret;
            }
        };

        template <typename... T>
        auto pop()
        {
            return pop_trait<T...>::apply(*this);
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

        static SQInteger NativeCallHandler(HSQUIRRELVM v)
        {
            stack s(v);
            SQStackInfos si;
            sq_stackinfos(v, 0, &si);


            SQInteger nargs = sq_gettop(v); //number of arguments
            for (SQInteger n = 1; n <= nargs; n++)
            {
                printf("arg %d is ", n);
                switch (sq_gettype(v, n))
                {
                case OT_NULL:
                    printf("null");
                    break;
                case OT_INTEGER:
                    printf("integer");
                    break;
                case OT_FLOAT:
                    printf("float");
                    break;
                case OT_STRING:
                    printf("string");
                    break;
                case OT_TABLE:
                    printf("table");
                    break;
                case OT_ARRAY:
                    printf("array");
                    break;
                case OT_USERDATA:
                    printf("userdata");
                    break;
                case OT_CLOSURE:
                    printf("closure(function)");
                    break;
                case OT_NATIVECLOSURE:
                    printf("native closure(C function)");
                    break;
                case OT_GENERATOR:
                    printf("generator");
                    break;
                case OT_USERPOINTER:
                    printf("userpointer");
                    break;
                default:
                    return sq_throwerror(v, "invalid param"); //throws an exception
                }
                printf("\n");
            }


            //sq_pushinteger(v, nargs); //push the number of arguments as return value
            //return 1; //1 because 1 value is returned
            return 0;
        }



        template<class FunctionType>
        void set_function(const char* functionName, FunctionType funcPtr)
        {
            stack s(mVmHandle);

            size_t numArgs = decltype(getArgumentCount(funcPtr))::value;

            // Store a mapping of functionName to funcPtr of FunctionType with numArgs
     
            sq_pushroottable(mVmHandle);
            s.push(functionName, strlen(functionName));
            sq_newclosure(mVmHandle, NativeCallHandler, 0);
            sq_setnativeclosurename(mVmHandle, -1, functionName);
            sq_newslot(mVmHandle, -3, SQTrue);
            //sq_call(mVmHandle, 0, SQFalse, SQFalse);
            s.pop_elements(1);
        }

        void script(const char* script)
        {
            //stack_saver ss(mVmHandle);

            /*
            struct StrAndPos
            {
                const char* mScript;
            };
            
            StrAndPos info{ script };

            SQRESULT r = sq_compile(mVmHandle, [](SQUserPointer ptr) -> SQInteger
            {
                StrAndPos* s = (StrAndPos*)ptr;
                if (!*(s->mScript))
                {
                    return 0;
                }
                char ret = *(s->mScript);
                s->mScript++;
                return ret;
            }, &info, "", 1);
            */

            SQRESULT r = sq_compilebuffer(mVmHandle, script, strlen(script), "generated", SQFalse);

            if (SQ_SUCCEEDED(r))
            {
                sq_pushroottable(mVmHandle);
                if (SQ_SUCCEEDED(sq_call(mVmHandle, 1, SQFalse, SQFalse)))
                {

                }
                stack s(mVmHandle);
               // s.pop_elements(1);
            }
        }

    private:
        vm* ThisFromVm(HSQUIRRELVM vm)
        {
            return reinterpret_cast<nut::vm*>(sq_getforeignptr(vm));
        }

        static void errfunc(HSQUIRRELVM /*v*/, const SQChar* s, ...)
        {
            va_list arglist;
            va_start(arglist, s);
            vprintf(s, arglist);
            va_end(arglist);
        }

        static void printfunc(HSQUIRRELVM /*v*/, const SQChar* s, ...)
        {
            va_list arglist;
            va_start(arglist, s);
            vprintf(s, arglist);
            va_end(arglist);
        }

    public:
        HSQUIRRELVM mVmHandle = nullptr;
    };
}