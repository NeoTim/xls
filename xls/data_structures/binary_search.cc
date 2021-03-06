// Copyright 2020 The XLS Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "xls/data_structures/binary_search.h"

#include "xls/common/logging/logging.h"
#include "xls/common/status/ret_check.h"
#include "xls/common/status/status_macros.h"

namespace xls {

int64 BinarySearchMaxTrue(int64 start, int64 end,
                          absl::FunctionRef<bool(int64 i)> f) {
  XLS_CHECK_LE(start, end);
  XLS_CHECK(f(start))
      << "Lowest value in range fails condition of binary search.";
  if (f(end)) {
    return end;
  }
  return BinarySearchMaxTrueWithStatus(
             start, end, [&](int64 i) -> absl::StatusOr<bool> { return f(i); })
      .value();
}

int64 BinarySearchMinTrue(int64 start, int64 end,
                          absl::FunctionRef<bool(int64 i)> f) {
  XLS_CHECK_LE(start, end);
  XLS_CHECK(f(end))
      << "Highest value in range fails condition of binary search.";
  if (f(start)) {
    return start;
  }
  return BinarySearchMinTrueWithStatus(
             start, end, [&](int64 i) -> absl::StatusOr<bool> { return f(i); })
      .value();
}

absl::StatusOr<int64> BinarySearchMaxTrueWithStatus(
    int64 start, int64 end,
    absl::FunctionRef<absl::StatusOr<bool>(int64 i)> f) {
  XLS_RET_CHECK_LE(start, end);
  XLS_ASSIGN_OR_RETURN(bool f_start, f(start));
  if (!f_start) {
    return absl::InvalidArgumentError(
        "Lowest value in range fails condition of binary search.");
  }
  XLS_ASSIGN_OR_RETURN(bool f_end, f(end));
  if (f_end) {
    return end;
  }
  int64 highest_true = start;
  int64 lowest_false = end;
  while (highest_true < lowest_false - 1) {
    int64 middle = highest_true + (lowest_false - highest_true) / 2;
    XLS_ASSIGN_OR_RETURN(bool f_middle, f(middle));
    if (f_middle) {
      highest_true = middle;
    } else {
      lowest_false = middle;
    }
  }
  return highest_true;
}

absl::StatusOr<int64> BinarySearchMinTrueWithStatus(
    int64 start, int64 end,
    absl::FunctionRef<absl::StatusOr<bool>(int64 i)> f) {
  XLS_RET_CHECK_LE(start, end);
  XLS_ASSIGN_OR_RETURN(bool f_end, f(end));
  if (!f_end) {
    return absl::InvalidArgumentError(
        "Highest value in range fails condition of binary search.");
  }
  XLS_ASSIGN_OR_RETURN(bool f_start, f(start));
  if (f_start) {
    return start;
  }
  int64 highest_false = start;
  int64 lowest_true = end;
  while (highest_false < lowest_true - 1) {
    int64 middle = highest_false + (lowest_true - highest_false) / 2;
    XLS_ASSIGN_OR_RETURN(bool f_middle, f(middle));
    if (f_middle) {
      lowest_true = middle;
    } else {
      highest_false = middle;
    }
  }
  return lowest_true;
}

}  // namespace xls
