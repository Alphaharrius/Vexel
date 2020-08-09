#include "Vexel.h"
/**
 * The coding style of te Vexel Project
 */

/**
 * Variables
 * - Use predefined types: u8, u18, u32, u64.
 * - Use short form with underscore separator.
 * - Instantiate dangling pointer to NULL
 */
u32 arr_len;
u32 *arr_ptr = NULL;

/**
 * Functions
 * - Use complete form with underscore separator.
 * - For vm internal methods and api, use error as return type.
 * - For vm operators, throw error using v_throw_error method.
 */
typedef enum {error_none} error_status;

error_status 
v_address_from_list_pointer_index(u32 ptr_idx) 
{
  u8 ANY_LOGIC;

  return error_none;
}