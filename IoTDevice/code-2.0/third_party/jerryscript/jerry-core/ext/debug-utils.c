/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: add debug utils.
 * Create: 2020/11/20
 */

#include <stdio.h>

#include "debug-utils.h"
#include "ecma-conversion.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "vm.h"

void PrintObjectValueProperties(ecma_value_t value)
{
  if (!ecma_is_value_object(value)) {
    return;
  }
  ecma_object_t* object = ecma_get_object_from_value(value);
  PrintObjectProperties(object);
}

void PrintObjectProperties(ecma_object_t* object)
{
  jmem_cpointer_t prop_iter_cp = object->u1.property_list_cp;

  while (prop_iter_cp != JMEM_CP_NULL) {
    ecma_property_header_t *prop_iter_p = ECMA_GET_NON_NULL_POINTER (ecma_property_header_t, prop_iter_cp);
    JERRY_ASSERT (ECMA_PROPERTY_IS_PROPERTY_PAIR (prop_iter_p));

    ecma_property_pair_t *prop_pair_p = (ecma_property_pair_t *) prop_iter_p;

    for (int i = 0; i < ECMA_PROPERTY_PAIR_ITEM_COUNT; i++) {
      if (ECMA_PROPERTY_IS_NAMED_PROPERTY (prop_iter_p->types[i])) {
        if (ECMA_PROPERTY_GET_NAME_TYPE (prop_iter_p->types[i]) == ECMA_DIRECT_STRING_MAGIC
            && prop_pair_p->names_cp[i] >= LIT_NON_INTERNAL_MAGIC_STRING__COUNT) {
          printf("Skipped direct string with big lit property\n");
          continue;
        }

        ecma_string_t *prop_name = ecma_string_from_property_name (prop_iter_p->types[i],
                                                                   prop_pair_p->names_cp[i]);
        ecma_property_value_t prop_value_p = prop_pair_p->values[i];

        ecma_value_t value_value;
        if (ecma_is_value_object(prop_value_p.value)) {
          // Expand it more?
          value_value = ecma_op_to_string (prop_value_p.value);
        } else {
          value_value = ecma_op_to_string (prop_value_p.value);
        }
        // handle value_value is error value?
        ecma_string_t *str_p = ecma_get_string_from_value(value_value);
        JERRY_ASSERT (str_p != NULL);

        ecma_string_t* separator_str = ecma_new_ecma_string_from_utf8((const lit_utf8_byte_t *)" :> ", 4);
        prop_name = ecma_concat_ecma_strings(prop_name, separator_str);
        prop_name = ecma_concat_ecma_strings(prop_name, str_p);

        ECMA_STRING_TO_UTF8_STRING(prop_name, buf, buf_size);
        printf("PROPERTY PAIR : ");
        for (int ii = 0; ii < (int)buf_size; ++ii) {
          printf("%c", buf[ii]);
        }
	printf("\n");

        ecma_deref_ecma_string(prop_name);
        ecma_deref_ecma_string(str_p);
        ecma_deref_ecma_string(separator_str);
      }
    }
    prop_iter_cp = prop_iter_p->next_property_cp;
  }
}

void PrintString(ecma_string_t* str)
{
  ECMA_STRING_TO_UTF8_STRING(str, buf, buf_size);
  printf("String: ");
  for (int i = 0; i < (int)buf_size; ++i) {
    printf("%c", buf[i]);
  }
  printf("\n");
}
