//*****************************************************************************
// Copyright 2017-2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#include <cstring>
#include <map>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <onnxifi.h>

#include "ngraph/file_util.hpp"
#include "ngraph/runtime/backend_manager.hpp"
#include "util/ndarray.hpp"

// ===============================================[ onnxGetBackendIDs ] =======

constexpr std::size_t g_default_backend_ids_count{10};

TEST(onnxifi, get_backend_ids)
{
    onnxBackendID backendIDs[g_default_backend_ids_count];
    std::size_t count{g_default_backend_ids_count};
    ::onnxStatus status{::onnxGetBackendIDs(backendIDs, &count)};
    EXPECT_TRUE(status == ONNXIFI_STATUS_SUCCESS);
    EXPECT_TRUE(count == ngraph::runtime::BackendManager::get_registered_backends().size());
}

TEST(onnxifi, get_backend_ids_buffer_null)
{
    std::size_t count{0};
    ::onnxStatus status{::onnxGetBackendIDs(nullptr, &count)};
    EXPECT_TRUE(status == ONNXIFI_STATUS_FALLBACK);
    EXPECT_TRUE(count == ngraph::runtime::BackendManager::get_registered_backends().size());
}

TEST(onnxifi, get_backend_ids_count_null)
{
    ::onnxBackendID backendIDs[g_default_backend_ids_count];
    ::onnxStatus status{::onnxGetBackendIDs(backendIDs, nullptr)};
    EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_POINTER);
}

TEST(onnxifi, get_backend_ids_null)
{
    ::onnxStatus status{::onnxGetBackendIDs(nullptr, nullptr)};
    EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_POINTER);
}

TEST(onnxifi, get_backend_ids_consistency_check)
{
    ::onnxBackendID first_ids[g_default_backend_ids_count];
    std::size_t first_count{g_default_backend_ids_count};
    EXPECT_TRUE(::onnxGetBackendIDs(first_ids, &first_count) == ONNXIFI_STATUS_SUCCESS);
    EXPECT_TRUE(first_count == ngraph::runtime::BackendManager::get_registered_backends().size());
    onnxBackendID second_ids[g_default_backend_ids_count];
    std::size_t second_count{g_default_backend_ids_count};
    EXPECT_TRUE(::onnxGetBackendIDs(second_ids, &second_count) == ONNXIFI_STATUS_SUCCESS);
    EXPECT_TRUE(second_count == ngraph::runtime::BackendManager::get_registered_backends().size());
    EXPECT_TRUE(first_count == second_count);
    EXPECT_TRUE(std::memcmp(first_ids, second_ids, first_count) == 0);
}

// ==============================================[ onnxGetBackendInfo ] =======

namespace
{
    constexpr std::size_t g_default_info_value_size{50};

    namespace status
    {
        std::string to_string(::onnxStatus status)
        {
#define ONNXIFI_STATUS_(value__)                                                                   \
    case ONNXIFI_STATUS_##value__: return "ONNXIFI_STATUS_" #value__;

            switch (status)
            {
                ONNXIFI_STATUS_(SUCCESS);
                ONNXIFI_STATUS_(FALLBACK);
                ONNXIFI_STATUS_(INVALID_ID);
                ONNXIFI_STATUS_(INVALID_SIZE);
                ONNXIFI_STATUS_(INVALID_POINTER);
                ONNXIFI_STATUS_(INVALID_PROTOBUF);
                ONNXIFI_STATUS_(INVALID_MODEL);
                ONNXIFI_STATUS_(INVALID_BACKEND);
                ONNXIFI_STATUS_(INVALID_GRAPH);
                ONNXIFI_STATUS_(INVALID_EVENT);
                ONNXIFI_STATUS_(INVALID_STATE);
                ONNXIFI_STATUS_(INVALID_NAME);
                ONNXIFI_STATUS_(INVALID_SHAPE);
                ONNXIFI_STATUS_(INVALID_DATATYPE);
                ONNXIFI_STATUS_(INVALID_MEMORY_TYPE);
                ONNXIFI_STATUS_(INVALID_MEMORY_LOCATION);
                ONNXIFI_STATUS_(INVALID_FENCE_TYPE);
                ONNXIFI_STATUS_(INVALID_PROPERTY);
                ONNXIFI_STATUS_(UNSUPPORTED_TAG);
                ONNXIFI_STATUS_(UNSUPPORTED_VERSION);
                ONNXIFI_STATUS_(UNSUPPORTED_OPERATOR);
                ONNXIFI_STATUS_(UNSUPPORTED_ATTRIBUTE);
                ONNXIFI_STATUS_(UNSUPPORTED_SHAPE);
                ONNXIFI_STATUS_(UNSUPPORTED_DATATYPE);
                ONNXIFI_STATUS_(NO_SYSTEM_MEMORY);
                ONNXIFI_STATUS_(NO_DEVICE_MEMORY);
                ONNXIFI_STATUS_(NO_SYSTEM_RESOURCES);
                ONNXIFI_STATUS_(NO_DEVICE_RESOURCES);
                ONNXIFI_STATUS_(BACKEND_UNAVAILABLE);
                ONNXIFI_STATUS_(INTERNAL_ERROR);
            default: return "UNKNOWN (" + std::to_string(status) + ")";
            }
        }
    }

    namespace device_type
    {
        std::string to_string(::onnxEnum device_type)
        {
#define ONNXIFI_DEVICE_TYPE_(value__)                                                              \
    case ONNXIFI_DEVICE_TYPE_##value__: return "ONNXIFI_DEVICE_TYPE_" #value__;

            switch (device_type)
            {
                ONNXIFI_DEVICE_TYPE_(NPU);
                ONNXIFI_DEVICE_TYPE_(DSP);
                ONNXIFI_DEVICE_TYPE_(GPU);
                ONNXIFI_DEVICE_TYPE_(CPU);
                ONNXIFI_DEVICE_TYPE_(FPGA);
                ONNXIFI_DEVICE_TYPE_(HETEROGENEOUS);
            default: return "UNKNOWN (" + std::to_string(device_type) + ")";
            }
        }
    }

    namespace error
    {
        struct status : std::runtime_error
        {
            explicit status(::onnxStatus status, ::onnxStatus expected = ONNXIFI_STATUS_SUCCESS)
                : std::runtime_error{::status::to_string(status) +
                                     ": unexpected status; expected " +
                                     ::status::to_string(expected)}
            {
            }
        };

    } // namespace error

    std::vector<::onnxBackendID> get_backend_ids()
    {
        std::size_t count{g_default_backend_ids_count};
        ::onnxStatus status{::onnxGetBackendIDs(nullptr, &count)};
        if (status != ONNXIFI_STATUS_FALLBACK)
        {
            throw error::status{status, ONNXIFI_STATUS_FALLBACK};
        }
        std::vector<::onnxBackendID> backend_ids(count);
        status = ::onnxGetBackendIDs(backend_ids.data(), &count);
        if (status == ONNXIFI_STATUS_FALLBACK)
        {
            backend_ids.resize(count);
            status = ::onnxGetBackendIDs(backend_ids.data(), &count);
        }
        if (status != ONNXIFI_STATUS_SUCCESS)
        {
            throw error::status{status};
        }
        if (backend_ids.empty())
        {
            throw std::runtime_error{"no backends registered"};
        }
        return backend_ids;
    }

    template <typename T>
    void backend_info_test_success(const std::vector<::onnxBackendID>& backend_ids,
                                   ::onnxBackendInfo infoType)
    {
        for (const auto& id : backend_ids)
        {
            std::size_t info_value_size{sizeof(T)};
            T info_value{0};
            ::onnxStatus status{::onnxGetBackendInfo(id, infoType, &info_value, &info_value_size)};
            if (status != ONNXIFI_STATUS_SUCCESS)
            {
                throw error::status{status};
            }
        }
    }

    template <>
    void backend_info_test_success<char[]>(const std::vector<::onnxBackendID>& backend_ids,
                                           ::onnxBackendInfo infoType)
    {
        for (const auto& id : backend_ids)
        {
            std::size_t info_value_size{g_default_info_value_size};
            std::vector<char> info_value(g_default_info_value_size);
            ::onnxStatus status{
                ::onnxGetBackendInfo(id, infoType, info_value.data(), &info_value_size)};
            if (status == ONNXIFI_STATUS_FALLBACK)
            {
                info_value.resize(info_value_size);
                status = ::onnxGetBackendInfo(id, infoType, info_value.data(), &info_value_size);
            }
            if (status != ONNXIFI_STATUS_SUCCESS)
            {
                throw error::status{status};
            }
        }
    }

    template <typename T>
    void backend_info_test_fallback(const std::vector<::onnxBackendID>& backend_ids,
                                    ::onnxBackendInfo infoType)
    {
        for (const auto& id : backend_ids)
        {
            std::size_t info_value_size{0};
            T info_value{0};
            ::onnxStatus status{::onnxGetBackendInfo(id, infoType, &info_value, &info_value_size)};
            if (status != ONNXIFI_STATUS_FALLBACK)
            {
                throw error::status{status, ONNXIFI_STATUS_FALLBACK};
            }
            if (info_value_size == 0)
            {
                throw std::runtime_error{"zero number of elements returned"};
            }
        }
    }

    template <>
    void backend_info_test_fallback<char[]>(const std::vector<::onnxBackendID>& backend_ids,
                                            ::onnxBackendInfo infoType)
    {
        for (const auto& backend_id : backend_ids)
        {
            std::size_t info_value_size{0};
            std::vector<char> info_value{};
            ::onnxStatus status{
                ::onnxGetBackendInfo(backend_id, infoType, info_value.data(), &info_value_size)};
            if (status != ONNXIFI_STATUS_FALLBACK)
            {
                throw error::status{status};
            }
            if (info_value_size == 0)
            {
                throw std::runtime_error{"zero number of elements returned"};
            }
        }
    }

    void backend_info_test_fallback_nullptr(const std::vector<::onnxBackendID>& backend_ids,
                                            ::onnxBackendInfo infoType)
    {
        for (const auto& backend_id : backend_ids)
        {
            std::size_t info_value_size{0};
            ::onnxStatus status{
                ::onnxGetBackendInfo(backend_id, infoType, nullptr, &info_value_size)};
            if (status != ONNXIFI_STATUS_FALLBACK)
            {
                throw error::status{status};
            }
            if (info_value_size == 0)
            {
                throw std::runtime_error{"zero number of elements returned"};
            }
        }
    }

    void backend_info_test_invalid_pointer(const std::vector<::onnxBackendID>& backend_ids,
                                           ::onnxBackendInfo infoType)
    {
        for (const auto& backend_id : backend_ids)
        {
            ::onnxStatus status{::onnxGetBackendInfo(backend_id, infoType, nullptr, nullptr)};
            if (status != ONNXIFI_STATUS_INVALID_POINTER)
            {
                throw error::status{status, ONNXIFI_STATUS_INVALID_POINTER};
            }
        }
    }

    template <typename T>
    void backend_info_test_result(const std::vector<::onnxBackendID>& backend_ids,
                                  ::onnxBackendInfo infoType,
                                  const std::function<bool(T, std::size_t)>& fn)
    {
        for (const auto& id : backend_ids)
        {
            std::size_t info_value_size{sizeof(T)};
            T info_value{};
            ::onnxStatus status{::onnxGetBackendInfo(id, infoType, &info_value, &info_value_size)};
            if (status != ONNXIFI_STATUS_SUCCESS)
            {
                throw error::status{status};
            }
            if (!fn(info_value, info_value_size))
            {
                throw std::runtime_error{"received information does not match"};
            }
        }
    }

    template <>
    void backend_info_test_result<char[]>(const std::vector<::onnxBackendID>& backend_ids,
                                          ::onnxBackendInfo infoType,
                                          const std::function<bool(char[], std::size_t)>& fn)
    {
        for (const auto& id : backend_ids)
        {
            std::size_t info_value_size{0};
            ::onnxStatus status{::onnxGetBackendInfo(id, infoType, nullptr, &info_value_size)};
            if (status != ONNXIFI_STATUS_FALLBACK)
            {
                throw error::status{status, ONNXIFI_STATUS_FALLBACK};
            }
            std::vector<char> info_value(info_value_size);
            status = ::onnxGetBackendInfo(id, infoType, info_value.data(), &info_value_size);
            if (status != ONNXIFI_STATUS_SUCCESS)
            {
                throw error::status{status};
            }
            if (!fn(info_value.data(), info_value_size))
            {
                throw std::runtime_error{"received information does not match"};
            }
        }
    }

} // namespace {anonymous}

TEST(onnxifi, get_backend_info_invalid_id)
{
    std::size_t info_value_size{g_default_info_value_size};
    char info_value[g_default_info_value_size];
    ::onnxStatus status{
        ::onnxGetBackendInfo(nullptr, ONNXIFI_BACKEND_VERSION, info_value, &info_value_size)};
    EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_ID);
}

TEST(onnxifi, get_backend_info_unsupported_attribute)
{
    auto backend_ids = get_backend_ids();
    std::size_t info_value_size{g_default_info_value_size};
    char info_value[g_default_info_value_size];
    for (const auto& backend_id : backend_ids)
    {
        ::onnxStatus status{
            ::onnxGetBackendInfo(backend_id, 9999999, info_value, &info_value_size)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_UNSUPPORTED_ATTRIBUTE);
    }
}

#define BACKEND_INFO_TEST_SUCCESS(type__, ids__, attribute__)                                      \
    backend_info_test_success<type__>(ids__, ONNXIFI_BACKEND_##attribute__);

#define BACKEND_INFO_TEST_FALLBACK(type__, ids__, attribute__)                                     \
    backend_info_test_fallback<type__>(ids__, ONNXIFI_BACKEND_##attribute__);

#define BACKEND_INFO_TEST_FALLBACK_NULL(ids__, attribute__)                                        \
    backend_info_test_fallback_nullptr(ids__, ONNXIFI_BACKEND_##attribute__);

#define BACKEND_INFO_TEST_INVALID_POINTER(ids__, attribute_)                                       \
    backend_info_test_invalid_pointer(ids__, ONNXIFI_BACKEND_##attribute_);

#define BACKEND_INFO_TEST_RESULT(type__, ids__, attribute__, function__)                           \
    backend_info_test_result<type__>(ids__, ONNXIFI_BACKEND_##attribute__, function__);

TEST(onnxifi, get_backend_info_onnxifi_version)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(uint64_t, ids, ONNXIFI_VERSION)
    BACKEND_INFO_TEST_FALLBACK(uint64_t, ids, ONNXIFI_VERSION)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, ONNXIFI_VERSION)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, ONNXIFI_VERSION)
    BACKEND_INFO_TEST_RESULT(
        uint64_t, ids, ONNXIFI_VERSION, [](uint64_t info_value, std::size_t info_value_size) {
            return (info_value == 1) && (info_value_size == sizeof(uint64_t));
        });
}

TEST(onnxifi, get_backend_info_name)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(char[], ids, NAME)
    BACKEND_INFO_TEST_FALLBACK(char[], ids, NAME)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, NAME)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, NAME)
    BACKEND_INFO_TEST_RESULT(
        char[], ids, NAME, [](const char* info_value, std::size_t info_value_size) {
            return /* TODO */ true;
        })
}

TEST(onnxifi, get_backend_info_vendor)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(char[], ids, VENDOR)
    BACKEND_INFO_TEST_FALLBACK(char[], ids, VENDOR)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, VENDOR)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, VENDOR)
    BACKEND_INFO_TEST_RESULT(
        char[], ids, VENDOR, [](const char* info_value, std::size_t info_value_size) {
            return /* TODO */ true;
        })
}

TEST(onnxifi, get_backend_info_version)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(char[], ids, VERSION)
    BACKEND_INFO_TEST_FALLBACK(char[], ids, VERSION)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, VERSION)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, VERSION)
    BACKEND_INFO_TEST_RESULT(
        char[], ids, VERSION, [](const char* info_value, std::size_t info_value_size) {
            return std::memcmp(info_value, NGRAPH_VERSION, info_value_size) == 0;
        });
}

TEST(onnxifi, get_backend_info_extensions)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(char[], ids, EXTENSIONS)
    BACKEND_INFO_TEST_FALLBACK(char[], ids, EXTENSIONS)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, EXTENSIONS)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, EXTENSIONS)
    BACKEND_INFO_TEST_RESULT(
        char[], ids, EXTENSIONS, [](const char* info_value, std::size_t info_value_size) {
            return /* TODO */ true;
        })
}

TEST(onnxifi, get_backend_info_device)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(char[], ids, DEVICE)
    BACKEND_INFO_TEST_FALLBACK(char[], ids, DEVICE)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, DEVICE)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, DEVICE)
    BACKEND_INFO_TEST_RESULT(
        char[], ids, DEVICE, [](const char* info_value, std::size_t info_value_size) {
            std::cout << "device: '" << std::string(info_value, info_value + info_value_size)
                      << "'\n";
            return true;
        })
}

TEST(onnxifi, get_backend_info_device_type)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(::onnxEnum, ids, DEVICE_TYPE)
    BACKEND_INFO_TEST_FALLBACK(::onnxEnum, ids, DEVICE_TYPE)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, DEVICE_TYPE)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, DEVICE_TYPE)
    BACKEND_INFO_TEST_RESULT(::onnxEnum,
                             ids,
                             DEVICE_TYPE,
                             [](::onnxEnum info_value, std::size_t info_value_size) -> bool {
                                 std::cout << "type: '" << device_type::to_string(info_value)
                                           << "'\n";
                                 return true;
                             })
}

TEST(onnxifi, get_backend_info_onnx_ir_version)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(char[], ids, ONNX_IR_VERSION)
    BACKEND_INFO_TEST_FALLBACK(char[], ids, ONNX_IR_VERSION)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, ONNX_IR_VERSION)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, ONNX_IR_VERSION)
    BACKEND_INFO_TEST_RESULT(
        char[], ids, ONNX_IR_VERSION, [](const char* info_value, std::size_t info_value_size) {
            return std::memcmp(info_value, ONNX_VERSION, info_value_size) == 0;
        })
}

TEST(onnxifi, get_backend_info_opset_version)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(char[], ids, OPSET_VERSION)
    BACKEND_INFO_TEST_FALLBACK(char[], ids, OPSET_VERSION)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, OPSET_VERSION)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, OPSET_VERSION)
    BACKEND_INFO_TEST_RESULT(
        char[], ids, OPSET_VERSION, [](const char* info_value, std::size_t info_value_size) {
            return std::memcmp(info_value, ONNX_OPSET_VERSION, info_value_size) == 0;
        })
}

TEST(onnxifi, get_backend_info_capabilities)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(::onnxBitfield, ids, CAPABILITIES)
    BACKEND_INFO_TEST_FALLBACK(::onnxBitfield, ids, CAPABILITIES)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, CAPABILITIES)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, CAPABILITIES)
    BACKEND_INFO_TEST_RESULT(::onnxBitfield,
                             ids,
                             CAPABILITIES,
                             [](::onnxBitfield info_value, std::size_t info_value_size) {
                                 return (info_value == ONNXIFI_CAPABILITY_THREAD_SAFE) &&
                                        (info_value_size == sizeof(::onnxBitfield));
                             })
}

TEST(onnxifi, get_backend_info_graph_init_properties)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(::onnxBitfield, ids, GRAPH_INIT_PROPERTIES)
    BACKEND_INFO_TEST_FALLBACK(::onnxBitfield, ids, GRAPH_INIT_PROPERTIES)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, GRAPH_INIT_PROPERTIES)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, GRAPH_INIT_PROPERTIES)
}

TEST(onnxifi, get_backend_info_synchronization_types)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(::onnxBitfield, ids, SYNCHRONIZATION_TYPES)
    BACKEND_INFO_TEST_FALLBACK(::onnxBitfield, ids, SYNCHRONIZATION_TYPES)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, SYNCHRONIZATION_TYPES)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, SYNCHRONIZATION_TYPES)
}

TEST(onnxifi, get_backend_info_memory_size)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(uint64_t, ids, MEMORY_SIZE)
    BACKEND_INFO_TEST_FALLBACK(uint64_t, ids, MEMORY_SIZE)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, MEMORY_SIZE)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, MEMORY_SIZE)
}

TEST(onnxifi, get_backend_info_max_graph_size)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(uint64_t, ids, MAX_GRAPH_SIZE)
    BACKEND_INFO_TEST_FALLBACK(uint64_t, ids, MAX_GRAPH_SIZE)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, MAX_GRAPH_SIZE)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, MAX_GRAPH_SIZE)
}

TEST(onnxifi, get_backend_info_max_graph_count)
{
    auto ids = get_backend_ids();
    BACKEND_INFO_TEST_SUCCESS(uint64_t, ids, MAX_GRAPH_COUNT)
    BACKEND_INFO_TEST_FALLBACK(uint64_t, ids, MAX_GRAPH_COUNT)
    BACKEND_INFO_TEST_FALLBACK_NULL(ids, MAX_GRAPH_COUNT)
    BACKEND_INFO_TEST_INVALID_POINTER(ids, MAX_GRAPH_COUNT)
}

// =================================================[ onnxInitBackend ] =======

namespace
{
    std::map<::onnxBackendID, ::onnxBackend> get_initialized_backends()
    {
        std::map<::onnxBackendID, ::onnxBackend> initialized_backends;
        auto backend_ids = get_backend_ids();
        std::vector<::onnxBackend> backends(backend_ids.size());
        // first initialize all available backends
        for (std::size_t i = 0; i < backend_ids.size(); ++i)
        {
            ::onnxStatus status{::onnxInitBackend(backend_ids.at(i), nullptr, &backends.at(i))};
            if (status != ONNXIFI_STATUS_SUCCESS)
            {
                throw error::status{status};
            }
            initialized_backends.emplace(std::make_pair(backend_ids.at(i), backends.at(i)));
        }
        return initialized_backends;
    }

} // anonymous

TEST(onnxifi, init_backend)
{
    auto backend_ids = get_backend_ids();
    for (const auto& backend_id : backend_ids)
    {
        ::onnxBackend backend;
        ::onnxStatus status{::onnxInitBackend(backend_id, nullptr, &backend)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_SUCCESS);
    }
}

// ONNXIFI_STATUS_INVALID_ID The function call failed because backendID
//                                   is not an ONNXIFI backend ID.
TEST(onnxifi, init_backend_invalid_id)
{
    ::onnxBackend backend;
    ::onnxStatus status{::onnxInitBackend(0, nullptr, &backend)};
    EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_ID);
    EXPECT_TRUE(backend == nullptr);
}

// ONNXIFI_STATUS_INVALID_POINTER The function call failed because
//                                backend pointer is NULL.
TEST(onnxifi, init_backend_invalid_pointer)
{
    auto backend_ids = get_backend_ids();
    ::onnxStatus status{::onnxInitBackend(backend_ids.at(0), nullptr, nullptr)};
    EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_POINTER);
}

// ONNXIFI_STATUS_BACKEND_UNAVAILABLE The function call failed because
//                                    the backend was disconnected or
//                                    uninstalled from the system.
TEST(onnxifi, DISABLED_init_backend_backend_unavaiable)
{
    auto backends = get_initialized_backends();
    // release all backends without releasing its ids
    for (const auto& backend : backends)
    {
        ::onnxStatus status{::onnxReleaseBackend(backend.second)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_SUCCESS);
    }
    // now check whether we may initialize them back.
    for (auto& backend : backends)
    {
        ::onnxStatus status{::onnxInitBackend(backend.first, nullptr, &backend.second)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_BACKEND_UNAVAILABLE);
        EXPECT_TRUE(backend.second == nullptr);
    }
}
// ===================================================[ onnxInitGraph ] =======

TEST(onnxifi, init_graph)
{
    auto backends = get_initialized_backends();
    std::string serialized_model =
        file_util::read_file_to_string(file_util::path_join(SERIALIZED_ZOO, "onnx/add_abc.onnx"));

    uint32_t weights_count{0};
    ::onnxGraph graph;

    for (const auto& backend : backends)
    {
        ::onnxStatus status{::onnxInitGraph(backend.second,
                                            nullptr,
                                            serialized_model.size(),
                                            serialized_model.data(),
                                            weights_count,
                                            nullptr,
                                            &graph)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_SUCCESS);
    }
}

// ONNXIFI_STATUS_INVALID_BACKEND The function call failed because
//                                backend is not an ONNXIFI backend
//                                handle.
TEST(onnxifi, init_graph_invalid_backend)
{
    std::string serialized_model =
        file_util::read_file_to_string(file_util::path_join(SERIALIZED_ZOO, "onnx/add_abc.onnx"));

    uint32_t weights_count{0};
    ::onnxGraph graph;
    ::onnxStatus status{::onnxInitGraph(0,
                                        nullptr,
                                        serialized_model.size(),
                                        serialized_model.data(),
                                        weights_count,
                                        nullptr,
                                        &graph)};
    EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_BACKEND);
    EXPECT_TRUE(graph == nullptr);
}

// ONNXIFI_STATUS_INVALID_POINTER The function call failed because
//                                onnxModel or graph pointer is NULL, or
//                                weightDescriptors pointer is NULL
//                                while weightsCount is non-zero.
TEST(onnxifi, init_graph_invalid_pointer)
{
    auto backends = get_initialized_backends();
    std::string serialized_model =
        file_util::read_file_to_string(file_util::path_join(SERIALIZED_ZOO, "onnx/add_abc.onnx"));

    uint32_t weights_count{0};
    ::onnxGraph graph;
    for (const auto& backend : backends)
    {
        weights_count = 0;
        // onnxModel = nullptr
        ::onnxStatus status{::onnxInitGraph(backend.second,
                                            nullptr,
                                            serialized_model.size(),
                                            nullptr /*onnxModel*/,
                                            weights_count,
                                            nullptr,
                                            &graph)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_POINTER);
        EXPECT_TRUE(graph == nullptr);

        // graph = nullptr
        ::onnxStatus status{::onnxInitGraph(backend.second,
                                            nullptr,
                                            serialized_model.size(),
                                            serialized_model.data(),
                                            weights_count,
                                            nullptr,
                                            nullptr /*graph*/)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_POINTER);

        // weightDescriptors == nullptr && weightsCount != 0
        weights_count = 100;
        ::onnxStatus status{::onnxInitGraph(backend.second,
                                            nullptr,
                                            serialized_model.size(),
                                            nullptr,
                                            weights_count,
                                            nullptr /*weightDescriptors*/,
                                            &graph)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_POINTER);
        EXPECT_TRUE(graph == nullptr);
    }
}

// ONNXIFI_STATUS_INVALID_SIZE The function call failed because
//                             onnxModelSize is 0.
TEST(onnxifi, init_graph_invalid_size)
{
    auto backends = get_initialized_backends();
    std::string serialized_model =
        file_util::read_file_to_string(file_util::path_join(SERIALIZED_ZOO, "onnx/add_abc.onnx"));

    uint32_t weights_count{0};
    ::onnxGraph graph;
    for (const auto& backend : backends)
    {
        ::onnxStatus status{::onnxInitGraph(backend.second,
                                            nullptr,
                                            0 /*onnxModelSize*/,
                                            serialized_model.data(),
                                            weights_count,
                                            nullptr,
                                            &graph)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_SIZE);
        EXPECT_TRUE(graph == nullptr);
    }
}

// ONNXIFI_STATUS_INVALID_PROTOBUF The function call failed because it
//                                 couldn't parse the serialized
//                                 protobuf as an ONNX ModelProto
//                                 message.
TEST(onnxifi, init_graph_invalid_protobuf)
{
    auto backends = get_initialized_backends();
    std::string serialized_model{"invalid protobuf string"};

    uint32_t weights_count{0};
    ::onnxGraph graph;
    for (const auto& backend : backends)
    {
        ::onnxStatus status{::onnxInitGraph(backend.second,
                                            nullptr,
                                            serialized_model.size(),
                                            serialized_model.data(),
                                            weights_count,
                                            nullptr,
                                            &graph)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_INVALID_PROTOBUF);
        EXPECT_TRUE(graph == nullptr);
    }
}

namespace
{
    using ModelWithTensors =
        std::tuple<std::string, std::vector<::onnxTensorDescriptorV1>, uint32_t>;

    ModelWithTensors get_default_model_with_tensor_descriptors()
    {
        std::string serialized_model = file_util::read_file_to_string(
            file_util::path_join(SERIALIZED_ZOO, "onnx/conv_with_strides_padding_bias.onnx"));

        // ---- input ----
        // uint32_t weights_count{35};
        std::vector<uint64_t> input_data_shape{1, 1, 7, 5};
        auto input_data{test::NDArray<float, 4>({{{{0, 1, 2, 3, 4},
                                                   {5, 6, 7, 8, 9},
                                                   {10, 11, 12, 13, 14},
                                                   {15, 16, 17, 18, 19},
                                                   {20, 21, 22, 23, 24},
                                                   {25, 26, 27, 28, 29},
                                                   {30, 31, 32, 33, 34}}}})
                            .get_vector()};
        ::onnxTensorDescriptorV1 input_tensor_descriptor{
            ONNXIFI_TAG_TENSOR_DESCRIPTOR_V1, // tag
            "A",                              // name - correspond to ValueInfoProto.name in one of
                                              // ModelProto.graph.input or ModelProto.graph.output
            ONNXIFI_DATATYPE_FLOAT32,         // dataType
            ONNXIFI_MEMORY_TYPE_CPU,          // memoryType
            4,                                // dimensions
            input_data_shape.data(),          // shape
            reinterpret_cast<::onnxPointer>(input_data.data()) // buffer
        };

        // ---- weights ----
        // uint32_t weights_count{9};
        std::vector<uint64_t> weights_data_shape{1, 1, 3, 3};
        auto weights_data{
            test::NDArray<float, 4>({{{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}}).get_vector()};
        ::onnxTensorDescriptorV1 weights_tensor_descriptor{
            ONNXIFI_TAG_TENSOR_DESCRIPTOR_V1, // tag
            "B",                              // name - correspond to ValueInfoProto.name in one of
                                              // ModelProto.graph.input or ModelProto.graph.output
            ONNXIFI_DATATYPE_FLOAT32,         // dataType
            ONNXIFI_MEMORY_TYPE_CPU,          // memoryType
            4,                                // dimensions
            weights_data_shape.data(),        // shape
            reinterpret_cast<::onnxPointer>(weights_data.data()) // buffer
        };

        // ---- bias ----
        // uint32_t weights_count{1};
        std::vector<uint64_t> bias_data_shape{1};
        std::vector<float> bias_data{2};
        ::onnxTensorDescriptorV1 bias_tensor_descriptor{
            ONNXIFI_TAG_TENSOR_DESCRIPTOR_V1, // tag
            "C",                              // name - correspond to ValueInfoProto.name in one of
                                              // ModelProto.graph.input or ModelProto.graph.output
            ONNXIFI_DATATYPE_FLOAT32,         // dataType
            ONNXIFI_MEMORY_TYPE_CPU,          // memoryType
            0,                                // dimensions
            bias_data_shape.data(),           // shape
            reinterpret_cast<::onnxPointer>(bias_data.data()) // buffer
        };

        std::vector<::onnxTensorDescriptorV1> tensor_descriptors{
            input_tensor_descriptor, weights_tensor_descriptor, bias_tensor_descriptor};
        uint32_t weights_count{input_data.size() + weights_data.size() + bias_data.size()};
        return std::make_tuple(serialized_model, tensor_descriptors, weights_count);
    }
} // namespace  anonymous

TEST(onnxifi, init_graph_model_with_tensor_descriptors)
{
    auto backends = get_initialized_backends();
    auto model_with_tensors = get_default_model_with_tensor_descriptors();

    auto serialized_model = std::get<0>(model_with_tensors);
    auto tensor_descriptors = std::get<1>(model_with_tensors);
    uint32_t weights_count{std::get<2>(model_with_tensors)};

    ::onnxGraph graph;
    for (const auto& backend : backends)
    {
        ::onnxStatus status{::onnxInitGraph(backend.second,
                                            nullptr,
                                            serialized_model.size(),
                                            serialized_model.data(),
                                            weights_count,
                                            tensor_descriptors.data(),
                                            &graph)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_SUCCESS);
    }
}

// ONNXIFI_STATUS_BACKEND_UNAVAILABLE The function call failed because
//                                    the backend was disconnected or
//                                    uninstalled from the system.
TEST(onnxifi, DISABLED_init_graph_backend_unavailable)
{
    auto backends = get_initialized_backends();
    // simulate disconnected backend
    for (const auto& backend : backends)
    {
        ::onnxStatus status{::onnxReleaseBackend(backend.second)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_SUCCESS);
    }
    std::string serialized_model =
        file_util::read_file_to_string(file_util::path_join(SERIALIZED_ZOO, "onnx/add_abc.onnx"));

    uint32_t weights_count{0};
    ::onnxGraph graph;
    for (const auto& backend : backends)
    {
        ::onnxStatus status{::onnxInitGraph(backend.second,
                                            nullptr,
                                            serialized_model.size(),
                                            serialized_model.data(),
                                            weights_count,
                                            nullptr,
                                            &graph)};
        EXPECT_TRUE(status == ONNXIFI_STATUS_BACKEND_UNAVAILABLE);
        EXPECT_TRUE(graph == nullptr);
    }
}