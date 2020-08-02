/**
 * Copyright (c) 2019, 2020, Alphaharrius. All rights reserved.
 */
#include "Vexel.h"

v_frame_object *
v_make_frame(u32 ret_len) 
{
  /**
   * Memory safe frame allocation method, 
   * this method uses procedural allocation 
   * validation to see if each essentials 
   * are correctly allocated, failed to 
   * do will deallocate all previous 
   * allocations.
   */
  v_frame_object *frame = malloc(sizeof(v_frame_object));
  if (!frame) return frame;
  u8 *dd_pos = calloc(V_DS_LEN, SIZE_8);
  if (!dd_pos) {
    free(frame);
    return dd_pos;
  }

  u64 *ds_pos = calloc(V_DS_LEN, SIZE_64);
  if (!ds_pos) {
    free(frame);
    free(dd_pos);
    return ds_pos;
  }

  u64 *rs_pos = calloc(ret_len, SIZE_64);
  if (!rs_pos) {
    free(frame);
    free(dd_pos);
    free(ds_pos);
    return rs_pos;
  }

  frame->dd_pos = frame->dd_base = dd_pos;
  frame->dd_roof = dd_pos + V_DS_LEN;
  frame->ds_pos = frame->ds_base = ds_pos;
  frame->ds_roof = dd_pos + V_DS_LEN;
  frame->rs_pos = frame->rs_base = rs_pos;
  frame->rs_len = ret_len;

  return frame;
}

void 
v_free_frame(v_frame_object *frame) {
  if (!frame) {
    return;
  }
  if (frame->cs_base) {
    free(frame->cs_base);
  }
  if (frame->dd_base) {
    free(frame->dd_base);
  }
  if (frame->ds_base) {
    free(frame->ds_base);
  }
  if (frame->rs_base) {
    free(frame->rs_base);
  }
  free(frame);
}
