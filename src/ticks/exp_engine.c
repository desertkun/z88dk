#include "exp_engine.h"
#include "backend.h"
#include <string.h>
#include <stdio.h>

struct expression_result_t expression_result;

enum expression_result_type_t get_expression_result_type() {
    return expression_result.type;
}

void set_expression_result_unknown() {
    expression_result.type = EXPRESSION_RESULT_UNKNOWN;
}

struct expression_result_t* set_expression_result(enum expression_result_type_t type) {
    expression_result.type = type;
    return &expression_result;
}

void expression_value_to_pointer(struct expression_result_t *from, struct expression_result_t *to,
                                 enum expression_result_type_t pointer_type) {

    if (from->type == EXPRESSION_RESULT_POINTER) {
        *to = *from;
        to->as_pointer.underlying_type = pointer_type;
        return;
    }

    to->type = EXPRESSION_RESULT_POINTER;
    to->as_pointer.underlying_type = pointer_type;

    switch (from->type) {
        case EXPRESSION_RESULT_FLOAT: {
            to->as_pointer.ptr = (uint16_t)from->as_float;
            to->as_pointer.element_size = from->memory_size;
            return;
        }
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_UCHAR:
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_UINT16:
        case EXPRESSION_RESULT_INT32:
        case EXPRESSION_RESULT_UINT32: {
            to->as_pointer.ptr = (uint16_t)from->as_int;
            to->as_pointer.element_size = from->memory_size;
            return;
        }
        default: {
            to->type = EXPRESSION_RESULT_ERROR;
            char tp[128];
            expression_result_type_to_string(from, tp);
            sprintf(to->as_error, "Cannot convert type %s to a pointer", tp);
            return;
        }
    }
}

void expression_dereference_pointer(struct expression_result_t *from, struct expression_result_t *to) {
    extern backend_t bk;

    if (from->type != EXPRESSION_RESULT_POINTER) {
        to->type = EXPRESSION_RESULT_ERROR;
        char tp[128];
        expression_result_type_to_string(from, tp);
        sprintf(to->as_error, "Cannot dereference type: %s", tp);
        return;
    }

    to->type = from->as_pointer.underlying_type;
    to->memory_location = from->as_pointer.ptr;
    to->memory_size = from->as_pointer.element_size;

    int16_t data = from->as_pointer.ptr;

    switch (from->as_pointer.underlying_type) {
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_UCHAR: {
            to->as_int = bk.get_memory(data);
            break;
        }
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_UINT16: {
            to->as_int = (bk.get_memory(data + 1) << 8) + bk.get_memory(data);
            break;
        }
        case EXPRESSION_RESULT_INT32:
        case EXPRESSION_RESULT_UINT32: {
            to->as_int = (bk.get_memory(data + 3) << 24) + (bk.get_memory(data + 2) << 16) + (bk.get_memory(data + 1) << 8) + bk.get_memory(data);
            break;
        }
        case EXPRESSION_RESULT_FLOAT: {
            to->type = EXPRESSION_RESULT_ERROR;
            sprintf(to->as_error, "Cannot dereference float (not implemented)");
            break;
        }
        default: {
            to->type = EXPRESSION_RESULT_ERROR;
            char tp[128];
            expression_result_type_to_string(from, tp);
            sprintf(to->as_error, "Cannot dereference type (not implemented): %s", tp);
            break;
        }
    }
}

enum expression_result_type_t expression_string_get_type(const char* str) {
    if (strcmp(str, "char") == 0) {
        return EXPRESSION_RESULT_CHAR;
    }
    if (strcmp(str, "unsigned char") == 0) {
        return EXPRESSION_RESULT_UCHAR;
    }
    if (strcmp(str, "int8_t") == 0) {
        return EXPRESSION_RESULT_CHAR;
    }
    if (strcmp(str, "uint8_t") == 0) {
        return EXPRESSION_RESULT_UCHAR;
    }
    if (strcmp(str, "int16_t") == 0 || strcmp(str, "int") == 0 || strcmp(str, "short") == 0) {
        return EXPRESSION_RESULT_INT16;
    }
    if (strcmp(str, "uint16_t") == 0 || strcmp(str, "unsigned int") == 0 || strcmp(str, "unsigned short") == 0) {
        return EXPRESSION_RESULT_UINT16;
    }
    if (strcmp(str, "int32_t") == 0 || strcmp(str, "long") == 0) {
        return EXPRESSION_RESULT_INT32;
    }
    if (strcmp(str, "uint32_t") == 0 || strcmp(str, "unsigned long") == 0) {
        return EXPRESSION_RESULT_UINT32;
    }

    return EXPRESSION_RESULT_UNKNOWN;
}

void expression_result_type_to_string(struct expression_result_t* result, char* buffer) {
    switch (result->type) {
        case EXPRESSION_RESULT_UNKNOWN: {
            strcpy(buffer, "void");
            break;
        }
        case EXPRESSION_RESULT_CHAR: {
            strcpy(buffer, "char");
            break;
        }
        case EXPRESSION_RESULT_UCHAR: {
            strcpy(buffer, "unsigned char");
            break;
        }
        case EXPRESSION_RESULT_INT16: {
            strcpy(buffer, "int16_t");
            break;
        }
        case EXPRESSION_RESULT_INT32: {
            strcpy(buffer, "int32_t");
            break;
        }
        case EXPRESSION_RESULT_UINT16: {
            strcpy(buffer, "uint16_t");
            break;
        }
        case EXPRESSION_RESULT_UINT32: {
            strcpy(buffer, "uint32_t");
            break;
        }
        case EXPRESSION_RESULT_FLOAT: {
            strcpy(buffer, "float");
            break;
        }
        case EXPRESSION_RESULT_POINTER: {
            struct expression_result_t p = *result;
            p.type = result->as_pointer.underlying_type;
            p.memory_location = result->as_pointer.ptr;
            p.memory_size = result->as_pointer.element_size;
            char pointer_type[128];
            expression_result_type_to_string(&p, pointer_type);
            sprintf(buffer, "%s*", pointer_type);
            break;
        }
        case EXPRESSION_RESULT_STRING: {
            strcpy(buffer, "char*");
            break;
        }
        default: {
            break;
        }
    }
}

void expression_math_add(struct expression_result_t* a, struct expression_result_t* b, struct expression_result_t* result) {
    if (a->type != b->type) {
        struct expression_result_t local_3;
        convert_expression(b, &local_3, a->type);
        expression_math_add(a, &local_3, result);
        return;
    }

    switch (a->type) {
        case EXPRESSION_RESULT_FLOAT: {
            result->as_float = a->as_float + b->as_float;
            return;
        }
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_UCHAR:
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_UINT16:
        case EXPRESSION_RESULT_INT32:
        case EXPRESSION_RESULT_UINT32: {
            result->as_int = a->as_int + b->as_int;
            return;
        }
        default: {
            result->type = EXPRESSION_RESULT_ERROR;
            char tp[128];
            expression_result_type_to_string(a, tp);
            sprintf(result->as_error, "Cannot perform math '+' on type %s", tp);
            break;
        }
    }
}

void expression_math_sub(struct expression_result_t* a, struct expression_result_t* b, struct expression_result_t* result) {
    if (a->type != b->type) {
        struct expression_result_t local_3;
        convert_expression(b, &local_3, a->type);
        expression_math_sub(a, &local_3, result);
        return;
    }

    switch (a->type) {
        case EXPRESSION_RESULT_FLOAT: {
            result->as_float = a->as_float - b->as_float;
            return;
        }
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_UCHAR:
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_UINT16:
        case EXPRESSION_RESULT_INT32:
        case EXPRESSION_RESULT_UINT32: {
            result->as_int = a->as_int - b->as_int;
            return;
        }
        default: {
            result->type = EXPRESSION_RESULT_ERROR;
            char tp[128];
            expression_result_type_to_string(a, tp);
            sprintf(result->as_error, "Cannot perform math '-' on type %s", tp);
            break;
        }
    }
}

void expression_math_mul(struct expression_result_t* a, struct expression_result_t* b, struct expression_result_t* result) {
    if (a->type != b->type) {
        struct expression_result_t local_3;
        convert_expression(b, &local_3, a->type);
        expression_math_mul(a, &local_3, result);
        return;
    }

    switch (a->type) {
        case EXPRESSION_RESULT_FLOAT: {
            result->as_float = a->as_float * b->as_float;
            return;
        }
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_UCHAR:
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_UINT16:
        case EXPRESSION_RESULT_INT32:
        case EXPRESSION_RESULT_UINT32:
        {
            result->as_int = a->as_int * b->as_int;
            return;
        }
        default:
        {
            result->type = EXPRESSION_RESULT_ERROR;
            char tp[128];
            expression_result_type_to_string(a, tp);
            sprintf(result->as_error, "Cannot perform math '-' on type %s", tp);
            break;
        }
    }
}

void expression_math_div(struct expression_result_t* a, struct expression_result_t* b, struct expression_result_t* result) {
    if (a->type != b->type) {
        struct expression_result_t local_3;
        convert_expression(b, &local_3, a->type);
        expression_math_div(a, &local_3, result);
        return;
    }

    switch (a->type) {
        case EXPRESSION_RESULT_FLOAT: {
            if (b->as_float == 0) {
                result->type = EXPRESSION_RESULT_ERROR;
                sprintf(result->as_error, "Division by zero.");
                return;
            }
            result->as_float = a->as_float / b->as_float;
            return;
        }
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_UCHAR:
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_UINT16:
        case EXPRESSION_RESULT_INT32:
        case EXPRESSION_RESULT_UINT32:
        {
            if (b->as_int == 0) {
                result->type = EXPRESSION_RESULT_ERROR;
                sprintf(result->as_error, "Division by zero.");
                return;
            }
            result->as_int = a->as_int / b->as_int;
            return;
        }
        default:
        {
            result->type = EXPRESSION_RESULT_ERROR;
            char tp[128];
            expression_result_type_to_string(a, tp);
            sprintf(result->as_error, "Cannot perform math '-' on type %s", tp);
            break;
        }
    }
}

uint8_t is_primitive_integer_type(struct expression_result_t* exp) {
    switch (exp->type) {
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_UCHAR:
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_UINT16:
        case EXPRESSION_RESULT_INT32:
        case EXPRESSION_RESULT_UINT32: {
            return 1;
        }
        default: {
            return 0;
        }
    }
}

void expression_result_value_to_string(struct expression_result_t* result, char* buffer) {
    switch (result->type) {
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_INT32: {
            sprintf(buffer, "%i", result->as_int);
            break;
        }
        case EXPRESSION_RESULT_UCHAR:
        case EXPRESSION_RESULT_UINT16:
        case EXPRESSION_RESULT_UINT32: {
            sprintf(buffer, "%u", result->as_uint);
            break;
        }
        case EXPRESSION_RESULT_FLOAT: {
            sprintf(buffer, "%f", result->as_float);
            break;
        }
        case EXPRESSION_RESULT_POINTER: {
            switch (result->as_pointer.underlying_type) {
                case EXPRESSION_RESULT_UCHAR:
                case EXPRESSION_RESULT_INT16:
                case EXPRESSION_RESULT_INT32:
                case EXPRESSION_RESULT_UINT16:
                case EXPRESSION_RESULT_UINT32:
                {
                    struct expression_result_t local;
                    expression_dereference_pointer(result, &local);
                    char buff[128];
                    expression_result_value_to_string(&local, buff);
                    sprintf(buffer, "%#04x(%s)", result->as_pointer.ptr, buff);
                    break;
                }
                case EXPRESSION_RESULT_CHAR: {
                    char buff [128];

                    int i = 0;
                    while (i < 128) {
                        char c = bk.get_memory(result->as_pointer.ptr + i);
                        if (c == 0) {
                            break;
                        }
                        buff[i++] = c;
                    }
                    sprintf(buffer, "%#04x(\"%s\")", result->as_pointer.ptr, buff);
                    break;
                }
                default: {
                    sprintf(buffer, "%#04x", result->as_pointer.ptr);
                }
            }

            break;
        }
        default: {
            break;
        }
    }
}

void convert_expression(struct expression_result_t* from, struct expression_result_t* to,
    enum expression_result_type_t type
) {
    to->type = type;
    to->memory_location = from->memory_location;

    switch (from->type) {
        case EXPRESSION_RESULT_FLOAT: {
            switch (type) {
                case EXPRESSION_RESULT_FLOAT: {
                    to->as_float = from->as_float;
                    break;
                }
                case EXPRESSION_RESULT_CHAR:
                case EXPRESSION_RESULT_UCHAR:
                case EXPRESSION_RESULT_INT16:
                case EXPRESSION_RESULT_UINT16:
                case EXPRESSION_RESULT_INT32:
                case EXPRESSION_RESULT_UINT32: {
                    to->as_int = (int32_t)from->as_float;
                    break;
                }
                default:
                {
                    break;
                }
            }

            break;
        }
        case EXPRESSION_RESULT_CHAR:
        case EXPRESSION_RESULT_UCHAR:
        case EXPRESSION_RESULT_INT16:
        case EXPRESSION_RESULT_UINT16:
        case EXPRESSION_RESULT_INT32:
        case EXPRESSION_RESULT_UINT32: {
            switch (type) {
                case EXPRESSION_RESULT_FLOAT: {
                    to->as_float = (int32_t)from->as_int;
                    break;
                }
                case EXPRESSION_RESULT_CHAR:
                case EXPRESSION_RESULT_UCHAR:
                case EXPRESSION_RESULT_INT16:
                case EXPRESSION_RESULT_UINT16:
                case EXPRESSION_RESULT_INT32:
                case EXPRESSION_RESULT_UINT32: {
                    to->as_int = from->as_int;
                    break;
                }

                case EXPRESSION_RESULT_POINTER: {
                    to->as_pointer.ptr = (uint16_t)from->as_int;
                    to->as_pointer.element_size = from->memory_size;
                    to->as_pointer.underlying_type = EXPRESSION_RESULT_UNKNOWN;
                }
                default: {
                    break;
                }
            }
            break;
        }

        case EXPRESSION_RESULT_POINTER: {
            switch (type) {
                case EXPRESSION_RESULT_FLOAT: {
                    to->as_float = (float)from->as_pointer.ptr;
                    break;
                }
                case EXPRESSION_RESULT_CHAR:
                case EXPRESSION_RESULT_UCHAR:
                case EXPRESSION_RESULT_INT16:
                case EXPRESSION_RESULT_UINT16:
                case EXPRESSION_RESULT_INT32:
                case EXPRESSION_RESULT_UINT32: {
                    to->as_int = (int32_t)from->as_pointer.ptr;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        default:
        {
            break;
        }
    }
}

void set_expression_result_error(const char* error) {
    expression_result.type = EXPRESSION_RESULT_ERROR;
    strcpy(expression_result.as_error, error);
}

struct expression_result_t* get_expression_result() {
    return &expression_result;
}
