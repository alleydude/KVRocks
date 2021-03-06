/*
 * Copyright 2018-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <folly/experimental/pushmi/executor.h>
#include <folly/experimental/pushmi/trampoline.h>

namespace folly {
namespace pushmi {

// very poor perf example executor.
//

struct new_thread_executor;

struct new_thread_task {
  using properties = property_set<
      is_sender<>,
      is_never_blocking<>,
      is_single<>>;

  PUSHMI_TEMPLATE(class Out)
  (requires Receiver<Out>)
  void submit(Out out) && {
    std::thread t{[out = std::move(out)]() mutable {
      auto tr = ::folly::pushmi::trampoline();
      ::folly::pushmi::submit(::folly::pushmi::schedule(tr), std::move(out));
    }};
    // pass ownership of thread to out
    t.detach();
  }
};

struct new_thread_executor {
  using properties = property_set<is_executor<>, is_concurrent_sequence<>>;

  new_thread_task schedule() {
    return {};
  }
};


inline new_thread_executor new_thread() {
  return {};
}

} // namespace pushmi
} // namespace folly
