/*
 * Copyright 2024 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CONFIDENTIAL_FEDERATED_COMPUTE_TESTING_PARSE_TEXT_PROTO_H_
#define CONFIDENTIAL_FEDERATED_COMPUTE_TESTING_PARSE_TEXT_PROTO_H_

#include <type_traits>

#include "absl/strings/string_view.h"
#include "fcp/base/monitoring.h"
#include "google/protobuf/text_format.h"

namespace confidential_federated_compute {

// Forked from FCP. We can't use FCP's version of the macro since it uses
// ParseProtoHelper which is declared in the fcp namespace. We'd also like to
// keep those target private to discourage other repos from using them.
//
// Convenience macro for parsing text formatted protos in test code.
// The input string should include only the proto fields but not the proto
// itself. For example:
//
//   const MyProtoType foo = PARSE_TEXT_PROTO("foo:1 sub { bar:2 }");
//   const MyProtoType bar = PARSE_TEXT_PROTO(R"(
//       foo: 1
//       sub {
//         bar: 2
//       })");
//
// Note that the output of the macro has to be assigned to proper proto message
// type in order for the parsing to work.
#define PARSE_TEXT_PROTO(STR) ParseProtoHelper(STR)

class ParseProtoHelper {
 public:
  explicit ParseProtoHelper(absl::string_view string_view)
      : string_view_(string_view) {}

  template <class T>
  operator T() {  // NOLINT
    static_assert(std::is_base_of<google::protobuf::Message, T>::value &&
                  !std::is_same<google::protobuf::Message, T>::value);
    T msg;
    FCP_CHECK(google::protobuf::TextFormat::ParseFromString(
        std::string(string_view_),  // NOLINT
        &msg));
    return msg;
  }

 private:
  absl::string_view string_view_;
};

}  // namespace confidential_federated_compute

#endif  // CONFIDENTIAL_FEDERATED_COMPUTE_TESTING_PARSE_TEXT_PROTO_H_
