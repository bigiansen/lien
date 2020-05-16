#pragma once

#include <ien/debug.hpp>
#include <stdexcept>
#include <string>

#define LIEN_ASSERT_USE_EXCEPTIONS 1

#if LIEN_ASSERT_USE_EXCEPTIONS == 1
	#define LIEN_ASSERT_FAILURE() throw std::logic_error("Assertion failure!")
#else
	#define LIEN_ASSERT_FAILURE() abort()
#endif

#define LIEN_ASSERT(cond) \
	if(!(cond)) { \
		fprintf(stderr, "Assertion failed!\nIn function [%s] -> (%s)\n", __FUNCTION__, #cond); \
		LIEN_ASSERT_FAILURE(); \
	}

#define LIEN_ASSERT_MSG(cond, failmsg) \
	if(!(cond)) { \
		fprintf(stderr, "Assertion failed!\nIn function [%s] -> (%s)\nMessage: %s\n", __FUNCTION__, #cond, failmsg); \
		LIEN_ASSERT_FAILURE(); \
	}

#ifndef NDEBUG

	#define LIEN_DEBUG_ASSERT(cond) \
		if(!(cond)) { \
			fprintf(stderr, "Assertion failed!\nAt %s:%d\nIn function [%s] -> (%s)\n", __FILE__, __LINE__, __FUNCTION__, #cond); \
			LIEN_ASSERT_FAILURE(); \
		}

	#define LIEN_DEBUG_ASSERT_MSG(cond, failmsg) \
		if(!(cond)) { \
			fprintf(stderr, "Assertion failed!\nAt %s:%d\nIn function [%s] -> (%s)\nMessage: %s\n", __FILE__, __LINE__, __FUNCTION__, #cond, failmsg); \
			LIEN_ASSERT_FAILURE(); \
		}

#else
	#define LIEN_DEBUG_ASSERT(cond)
	#define LIEN_DEBUG_ASSERT_MSG(cond)
#endif

#define LIEN_NOT_IMPLEMENTED() runtime_assert(false, "Method not implemented!")