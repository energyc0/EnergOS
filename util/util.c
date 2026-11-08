#include "util.h"

size_t Util_Strlen(const char *str) {
  const char *end = str;
  while (*end != '\0') {
    ++end;
  }
  return (size_t)(end - str);
}

int Util_Strcmp(const char *left, const char *right) {
  while (*left != '\0' && *left == *right) {
    ++left;
    ++right;
  }
  return (int)(unsigned char)*left - (int)(unsigned char)*right;
}

int Util_Strncmp(const char *left, const char *right, size_t count) {
  while (count > 0 && *left != '\0' && *left == *right) {
    ++left;
    ++right;
    --count;
  }
  if (count == 0) {
    return 0;
  }
  return (int)(unsigned char)*left - (int)(unsigned char)*right;
}

size_t Util_Strlcpy(char *destination, const char *source,
                    size_t destination_size) {
  const size_t source_length = Util_Strlen(source);
  if (destination_size > 0) {
    size_t copy_length = source_length;
    if (copy_length >= destination_size) {
      copy_length = destination_size - 1;
    }
    for (size_t i = 0; i < copy_length; ++i) {
      destination[i] = source[i];
    }
    destination[copy_length] = '\0';
  }
  return source_length;
}

size_t Util_Itoa(int32_t value, char *destination, size_t destination_size) {
  char reversed[10];
  size_t digit_count = 0;
  uint32_t magnitude;

  if (value < 0) {
    magnitude = (uint32_t)(-(value + 1)) + 1u;
  } else {
    magnitude = (uint32_t)value;
  }

  do {
    reversed[digit_count++] = (char)('0' + magnitude % 10u);
    magnitude /= 10u;
  } while (magnitude != 0);

  const size_t result_length = digit_count + (value < 0 ? 1u : 0u);
  if (destination_size > 0) {
    size_t output_length = result_length;
    if (output_length >= destination_size) {
      output_length = destination_size - 1;
    }

    size_t output_index = 0;
    if (value < 0 && output_index < output_length) {
      destination[output_index++] = '-';
    }
    while (output_index < output_length) {
      destination[output_index] = reversed[digit_count - 1];
      --digit_count;
      ++output_index;
    }
    destination[output_index] = '\0';
  }
  return result_length;
}

size_t Util_Itohex(uint32_t value, char *destination, size_t destination_size) {
  static const char digits[] = "0123456789ABCDEF";
  const size_t result_length = 10;

  if (destination_size > 0) {
    size_t output_length = result_length;
    if (output_length >= destination_size) {
      output_length = destination_size - 1;
    }

    const char prefix[] = "0x";
    size_t output_index = 0;
    while (output_index < output_length && output_index < 2) {
      destination[output_index] = prefix[output_index];
      ++output_index;
    }
    while (output_index < output_length) {
      const size_t digit_index = output_index - 2;
      const unsigned int shift = (unsigned int)(28 - 4 * digit_index);
      destination[output_index] = digits[(value >> shift) & 0x0Fu];
      ++output_index;
    }
    destination[output_index] = '\0';
  }
  return result_length;
}

void memcpy(void *dest, const void *src, size_t n) {
  for (size_t i = 0; i < n; i++) ((char *)dest)[i] = ((char *)src)[i];
}

void memset(void *dest, int value, size_t n) {
  for (size_t i = 0; i < n; i++) ((char *)dest)[i] = value;
}