
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <string>

#ifndef STACK_TRACE_FILE(STACK_TRACE_FILE)
#define STACK_TRACE_FILE(STACK_TRACE_FILE)	tracer::Trace::LogToFile(STACK_TRACE_FILE)
#endif
#ifndef STACK_TRACE_FUNC(func_name)
#define STACK_TRACE_FUNC(func_name)    tracer::Trace __TRACE__(func_name, "()")
#endif
#ifndef STACK_TRACE_FUNC_ARG1(func_name, argfmt, arg)
#define STACK_TRACE_FUNC_ARG1(func_name, argfmt, arg)   \
            tracer::Trace __TRACE__(func_name, argfmt, arg)
#endif
#ifndef STACK_TRACE_FUNC_ARG2(func_name, argfmt, arg1, arg2)
#define STACK_TRACE_FUNC_ARG2(func_name, argfmt, arg1, arg2)   \
            tracer::Trace __TRACE__(func_name, argfmt, arg1, arg2)
#endif
// more macros define....



namespace	tracer
{
    class	Trace
    {
    public:
    	explicit Trace(char *func_name, const char* argsfmt, ...)
    	{
            char fmt[256] ={0};
            sprintf(fmt, "%s%s", func_name, argsfmt);
    	    va_list arglist;
    	    va_start(arglist, argsfmt);
    		LogMsg(depth_, depth_ * 2, fmt,  arglist);
    		va_end(arglist);
    		++depth_;
    	}

    	~Trace()
    	{
    		--depth_;
    	}

    	/// special the global log file.
    	void static LogToFile(const char *STACK_TRACE_FILE)
    	{
    		STACK_TRACE_FILE_ = STACK_TRACE_FILE;
    	}

		
		
		void static setLoggingNeeded(bool);

		bool static isLoggingNeeded(void);
		

    private:
    	void LogMsg(int depth, int align, const char *fmt, va_list args)
    	{
    		FILE	*fp = fopen(STACK_TRACE_FILE_.c_str(), "a+");
    		if (fp == NULL)
    		{
    			return;
    		}


    		time_t		curTime;
    		time(&curTime);

    		char	timeStamp[32] = { 0 };
    		strftime(timeStamp, sizeof(timeStamp), "%Y%m%d.%H%M%S", localtime(&curTime));

    		// only log the timestamp when the time changes
    		unsigned int len = fprintf( fp, "%s %*.*s> (%d)",
    				(last_invoke_time_ != curTime) ? timeStamp : "               ",
    				2 * depth,
    				2 * depth,
    				nest_,
    				depth);
    		last_invoke_time_ = curTime;
    		len += vfprintf(fp, fmt, args);
    		len += fwrite("\n", 1, 1, fp);
    		fflush(fp);
    		fclose(fp);
    	}

    private:
    	// the debug trace filename
    	static std::string	STACK_TRACE_FILE_;

		static bool LoggingNeeded;

    	// function call stack depth
    	static int			depth_;
    	static const char*  nest_;
    	static time_t       last_invoke_time_;
    };

    std::string Trace::STACK_TRACE_FILE_  = "";
    int         Trace::depth_       = 0;

	bool Trace::LoggingNeeded;

    // arbitrarily support nesting 34 deep for no particular reason
    const char* Trace::nest_        = "| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | ";
    time_t      Trace::last_invoke_time_ = 0;
}	// end namespace tracer


// Thanks to Mockey Chen from www.codeproject.com for this code
