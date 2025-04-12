#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "infra/json.h"

#define DECLARE_SPTR(name) using name##SPtr = std::shared_ptr<name>;
#define FORWARD_DECLARE_SPTR(name) \
  class name;                      \
  DECLARE_SPTR(name);

#define DECLARE_UPTR(name) using name##UPtr = std::unique_ptr<name>;
#define FORWARD_DECLARE_UPTR(name) \
  class name;                      \
  DECLARE_UPTR(name);
