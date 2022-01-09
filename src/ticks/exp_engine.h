#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <inttypes.h>

enum expression_result_type_t {
    EXPRESSION_RESULT_UNKNOWN = 0,
    EXPRESSION_RESULT_ERROR,
    EXPRESSION_RESULT_CHAR,
    EXPRESSION_RESULT_UCHAR,
    EXPRESSION_RESULT_INT16,
    EXPRESSION_RESULT_UINT16,
    EXPRESSION_RESULT_INT32,
    EXPRESSION_RESULT_UINT32,
    EXPRESSION_RESULT_FLOAT,
    EXPRESSION_RESULT_POINTER,
    EXPRESSION_RESULT_STRING,
};

struct expression_result_t {
    union {
        int32_t as_int;
        uint32_t as_uint;
        float as_float;
    };

    struct {
        enum expression_result_type_t underlying_type;
        uint16_t element_size;
        uint16_t ptr;
    } as_pointer;

    char as_error[128];
    uint16_t memory_location;
    uint16_t memory_size;
    enum expression_result_type_t type;
};

extern void evaluate_expression_string(const char* expr);
extern enum expression_result_type_t get_expression_result_type();

extern struct expression_result_t* set_expression_result(enum expression_result_type_t type);
extern void convert_expression(struct expression_result_t* from, struct expression_result_t* to, enum expression_result_type_t type);
extern void expression_result_type_to_string(struct expression_result_t* result, char* buffer);
extern void expression_dereference_pointer(struct expression_result_t *from, struct expression_result_t *to);
extern void expression_value_to_pointer(struct expression_result_t *from, struct expression_result_t *to,
                                        enum expression_result_type_t pointer_type);
extern void expression_math_add(struct expression_result_t* a, struct expression_result_t* b, struct expression_result_t* result);
extern void expression_math_sub(struct expression_result_t* a, struct expression_result_t* b, struct expression_result_t* result);
extern void expression_math_mul(struct expression_result_t* a, struct expression_result_t* b, struct expression_result_t* result);
extern void expression_math_div(struct expression_result_t* a, struct expression_result_t* b, struct expression_result_t* result);
extern enum expression_result_type_t expression_string_get_type(const char* str);
extern uint8_t is_primitive_integer_type(struct expression_result_t* exp);
extern void expression_result_value_to_string(struct expression_result_t* result, char* buffer);
extern void set_expression_result_unknown();
extern void set_expression_result_error(const char* error);
extern struct expression_result_t* get_expression_result();

struct lookup_t {
    const char* symbol_name;
};

extern enum expression_result_type_t debug_lookup_symbol(struct lookup_t* lookup, struct expression_result_t* result);


#endif