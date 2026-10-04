#ifndef BASE_ARRAY_H
#define BASE_ARRAY_H

#define CreateSlice(type) \
  struct Sclice_##type {  \
    type *data;           \
    U64 len;              \
  }

#define CreateDynamicArray(type) \
  struct  DynamicArray_##type    \
    type *data;                  \
    U64 len;                     \
    U64 cap;                     \
  }

#define SliceFromDA(da, type) (Sclice_##type) { \
da.data,                                        \
da.len,                                         \
}

CreateSlice(void);

#define SliceFromTo(slice, start, end) \
  (Sclice_void) { slice.data + (sizeof(*slice.data) * start), end - start }

#define SliceTo(slice, end) \
  (Sclice_void) { slice, slice.len - end}

#define SliceFrom(slice, start) \
  (Sclice_void) { slice.data + (sizeof(*slice.data) * start), slice.len - start}

#endif
