#pragma once
#ifndef MODULAR_HPP
#define MODULAR_HPP
#include <iostream>
#include <duktape.h>
#include <stdexcept>
#include <raylib.h>

inline Color getColor(duk_context *ctx, int offset) {
  Color c = {0, 0, 0, 255}; // Initialize color to black with full opacity

  // Check if the argument at 'offset' is an object
  if (!duk_is_object(ctx, offset)) {
    // Handle error: Argument is not an object
    printf("Error: Argument at offset %d is not an object\n", offset);
    // print the type
    printf("Type: %d\n", duk_get_type(ctx, offset));
    return c;
  }

  // Get the 'r', 'g', 'b', and 'a' properties from the object
  duk_get_prop_string(ctx, offset, "r");
  duk_get_prop_string(ctx, offset, "g");
  duk_get_prop_string(ctx, offset, "b");
  duk_get_prop_string(ctx, offset, "a");

  // Check if all properties exist and are numbers
  if (duk_is_number(ctx, -4) && duk_is_number(ctx, -3) &&
      duk_is_number(ctx, -2) && duk_is_number(ctx, -1)) {
    // Extract RGBA values
    c.r = duk_get_int(ctx, -4);
    c.g = duk_get_int(ctx, -3);
    c.b = duk_get_int(ctx, -2);
    c.a = duk_get_int(ctx, -1);
  } else {
    // Handle error: Properties missing or not numbers
    printf("Error: RGBA properties missing or not numbers\n");
  }

  // Pop values from the stack
  duk_pop_n(ctx, 4);

  return c;
}
typedef int (*duk_fn) (duk_context *ctx);

// Structure to represent a function in the module
typedef struct duk_func {
    const char *name;
    duk_fn func;
} duk_func;

// Function to register a module with its functions
inline void registerModule(duk_context *ctx, duk_func* functions) {
    // Create a new object to represent the module
    duk_push_object(ctx);

    // Register each function in the module object
    for (int i = 0; functions[i].name != NULL; ++i) {
        duk_push_c_function(ctx, functions[i].func, DUK_VARARGS);

        // Set a property on the function to store the function name (useful for error messages)
        duk_push_string(ctx, functions[i].name);
        duk_put_prop_string(ctx, -2, "__name");

        duk_put_prop_string(ctx, -2, functions[i].name);
    }

    // Define the module with the specified name in the global object
}

inline void setGlobalModule(duk_context *ctx, const char* moduleName) {
    duk_put_global_string(ctx, moduleName);
}
inline void setObjectIntVal(duk_context *ctx, const char *name, int val) {
    duk_push_number(ctx, val);
    duk_put_prop_string(ctx, -2, name);
    
}
inline void setObjectStringVal(duk_context *ctx, const char *name, const char *val) {
    duk_push_string(ctx, val);
    duk_put_prop_string(ctx, -2, name);
}

// Function to retrieve a function from the module object
inline duk_fn getModuleFunction(duk_context *ctx, const char* moduleName, const char* functionName) {
    duk_get_global_string(ctx, moduleName);  // Get the module object
    if (!duk_is_object(ctx, -1)) {
        throw std::runtime_error("Module not found");
    }

    duk_get_prop_string(ctx, -1, functionName);  // Get the function from the module object
    if (!duk_is_function(ctx, -1)) {
        duk_pop_2(ctx);  // Pop the module object and non-function value from the stack
        throw std::runtime_error("Function not found");
    }

    duk_fn func = (duk_fn)duk_get_c_function(ctx, -1);  // Get the function pointer
    duk_pop_2(ctx);  // Pop the function and module object from the stack

    return func;
}
#endif