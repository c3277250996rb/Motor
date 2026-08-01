#include "main.h"
#include <stdlib.h>

#define SIGN(x) (((x) > 0) - ((x) < 0))
#define LABS_LIMIT_CHECK(limit, check) (labs(check) > limit ? SIGN(check) * limit : check)
#define MAXMINI_LIMIT_CHECK(max, min, check) (check > max ? max : check < min ? min : check)
