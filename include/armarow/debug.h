/*******************************************************************************
 *
 * Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2010 Philipp Werner <philipp.werner@student.ovgu.de>
 *               2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/
#ifndef __ARMAROW_DEBUG_h__
#define __ARMAROW_DEBUG_h__

/*! \defgroup UnitTest Logging and Debugging
 *  \todo documentation of the Group Logging and Debugging
 */
#ifdef __GNUC__
#define FUNCTION_SIGNATURE  __PRETTY_FUNCTION__
#else
#define FUNCTION_SIGNATURE  __FUNCSIG__
#endif

#include "logging/logging.h"

#ifdef ARMAROW_DEBUG_DISABLE
LOGGING_DISABLE_LEVEL(::logging::Trace);
LOGGING_DISABLE_LEVEL(::logging::Info);
#endif

#define TRACE_FUNCTION do {                                         \
    ::logging::log::emit< ::logging::Trace>()                       \
       << '"' << FUNCTION_SIGNATURE << '"'                          \
       << PROGMEMSTRING(" -> "__FILE__":" __TOSTR__(__LINE__)" ")   \
       << ::logging::log::endl;                                     \
} while(0)

#if !defined(ASSERT_FAILED_HANDLER)
#include <stdlib.h>

/*!
 * \brief Default FAMOUSO_ASSERT handler
 * \param expr Expression evaluated to false
 * \param file File which contains assert
 * \param line Line which contains assert
 *
 * Writes error message to ::logging::log
 */
template<typename ExprT, typename FileT, typename LineT> static inline
void __assert_failed_handler(ExprT expr, FileT file, LineT line) {
    using ::logging::log;
    log::emit() << file << ':' << line
        << PROGMEMSTRING(": Assertion '") << expr
        << PROGMEMSTRING("' failed.") << log::endl;
    abort();
}


/*! \brief Name of the assert handler function
 *
 *  This preprocessor symbol may be defined before including
 *  debug.h to overwrite the default assert handler function.
 */
#define ASSERT_FAILED_HANDLER ::__assert_failed_handler
#endif

#undef assert
#undef assert_only_def

#if defined(NDEBUG)
#define assert(expr)
#define assert_only_def(var_def)
#else
/*! \brief  Print diagnostic message and abort program if
 *          \p expr evaluates to false
 *  \param expr Expression to test for
 *
 *  Useful to find bugs and spot wrong usage of interfaces by
 *  checking assumptions that should be always true. Do not use
 *  this macro to check for errors that may happen in NDEBUG
 *  case, e.g. invalid user input.
 *
 *  Evaluates to nothing if preprocessor symbol NDEBUG is defined.
 *
 *  \par Overwrite handler function:
 *  An assert handler function has the following signature:
 *  \code void foobar(const char * expr, const char * file, int line); \endcode
 *  It should never return if you are not interested in undefined
 *  behaviour.
 *  To get your handler called on assertion failure define the preprocessor
 *  symbol ASSERT_FAILED_HANDLER before including debug.h:
 *  \code #define ASSERT_FAILED_HANDLER foobar \endcode
 */
#define assert(expr) ((expr) ? ((void)0) :                          \
        ASSERT_FAILED_HANDLER (PROGMEMSTRING(#expr),                \
                               PROGMEMSTRING(__FILE__),             \
                               PROGMEMSTRING(__TOSTR__(__LINE__))))

/*! \brief  Define local variable only used with assert test
 *  \param var_def Definition of a variable
 *
 *  Use this to define variables with meaningful names to
 *  to get better assert messages while avoiding the
 *  'unused variable' warning in NDEBUG case.
 *
 *  Evaluates to var_def if NDEBUG is not defined.
 *  Evaluates to nothing if NDEBUG is defined.
 */
#define assert_only_def(var_def) var_def
#endif

#endif  //__ARMAROW_DEBUG_h__
