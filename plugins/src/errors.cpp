#include "errors.h"

namespace errors
{
	error not_enough_args = "not enough arguments (%d expected, got %d)";
	error pointer_invalid = "%s reference is invalid (value 0x%x)";
	error cannot_acquire = "%s reference cannot be acquired (null or invalid)";
	error cannot_release = "%s reference cannot be released (null, invalid, or local)";
	error operation_not_supported = "this operation is not supported for the current state of the %s instance";
	error out_of_range = "%s is out of range";
	error key_not_present = "the key is not present in the map";
	error func_not_found = "%s function '%s' was not found";
	error var_not_found = "%s variable '%s' was not found";
	error arg_empty = "argument is empty";
	error inner_error = "%s function '%s' has raised an AMX error %d: %s";
	error no_debug_error = "debug info is not available";
	error unhandled_exception = "unhandled C++ exception: %s";
}

errors::native_error::native_error(const char *format, va_list args, int level) : message(vsnprintf(NULL, 0, format, args), '\0'), level(level)
{
	vsprintf(&message[0], format, args);
}

errors::native_error::native_error(const char *format, int code, ...) : level(level)
{
	va_list args;
	va_start(args, code);
	message = std::string(vsnprintf(NULL, 0, format, args), '\0');
	vsprintf(&message[0], format, args);
	va_end(args);
}

errors::native_error::native_error(std::string &&message, int level) : level(level), message(std::move(message))
{

}

errors::end_of_arguments_error::end_of_arguments_error(const cell *argbase, size_t required) : argbase(argbase), required(required)
{

}

[[noreturn]] void amx_FormalError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	errors::native_error err(format, args, 3);
	va_end(args);
	throw err;
}

[[noreturn]] void amx_LogicError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	errors::native_error err(format, args, 2);
	va_end(args);
	throw err;
}

const char *amx::StrError(int errnum)
{
	static const char *messages[] = {
		/* AMX_ERR_NONE */ "(none)",
		/* AMX_ERR_EXIT */ "Forced exit",
		/* AMX_ERR_ASSERT */ "Assertion failed",
		/* AMX_ERR_STACKERR */ "Stack/heap collision (insufficient stack size)",
		/* AMX_ERR_BOUNDS */ "Array index out of bounds",
		/* AMX_ERR_MEMACCESS */ "Invalid memory access",
		/* AMX_ERR_INVINSTR */ "Invalid instruction",
		/* AMX_ERR_STACKLOW */ "Stack underflow",
		/* AMX_ERR_HEAPLOW */ "Heap underflow",
		/* AMX_ERR_CALLBACK */ "No (valid) native function callback",
		/* AMX_ERR_NATIVE */ "Native function failed",
		/* AMX_ERR_DIVIDE */ "Divide by zero",
		/* AMX_ERR_SLEEP */ "(sleep mode)",
		/* 13 */ "(reserved)",
		/* 14 */ "(reserved)",
		/* 15 */ "(reserved)",
		/* AMX_ERR_MEMORY */ "Out of memory",
		/* AMX_ERR_FORMAT */ "Invalid/unsupported P-code file format",
		/* AMX_ERR_VERSION */ "File is for a newer version of the AMX",
		/* AMX_ERR_NOTFOUND */ "File or function is not found",
		/* AMX_ERR_INDEX */ "Invalid index parameter (bad entry point)",
		/* AMX_ERR_DEBUG */ "Debugger cannot run",
		/* AMX_ERR_INIT */ "AMX not initialized (or doubly initialized)",
		/* AMX_ERR_USERDATA */ "Unable to set user data field (table full)",
		/* AMX_ERR_INIT_JIT */ "Cannot initialize the JIT",
		/* AMX_ERR_PARAMS */ "Parameter error",
		/* AMX_ERR_DOMAIN */ "Domain error, expression result does not fit in range",
		/* AMX_ERR_GENERAL */ "General error (unknown or unspecific error)",
	};
	if(errnum < 0 || (size_t)errnum >= sizeof(messages) / sizeof(*messages))
		return "(unknown)";
	return messages[errnum];
}
