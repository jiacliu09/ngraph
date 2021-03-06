//*****************************************************************************
// Copyright 2017-2020 Intel Corporation
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

#pragma once

#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ngraph/coordinate.hpp"
#include "ngraph/log.hpp"
#include "ngraph/ops.hpp"
#include "ngraph/runtime/aligned_buffer.hpp"
#include "ngraph/runtime/backend.hpp"
#include "ngraph/runtime/host_tensor.hpp"
#include "ngraph/runtime/interpreter/int_backend_visibility.hpp"
#include "ngraph/runtime/reference/abs.hpp"
#include "ngraph/runtime/reference/acos.hpp"
#include "ngraph/runtime/reference/acosh.hpp"
#include "ngraph/runtime/reference/add.hpp"
#include "ngraph/runtime/reference/all.hpp"
#include "ngraph/runtime/reference/allreduce.hpp"
#include "ngraph/runtime/reference/and.hpp"
#include "ngraph/runtime/reference/any.hpp"
#include "ngraph/runtime/reference/argmax.hpp"
#include "ngraph/runtime/reference/argmin.hpp"
#include "ngraph/runtime/reference/asin.hpp"
#include "ngraph/runtime/reference/asinh.hpp"
#include "ngraph/runtime/reference/atan.hpp"
#include "ngraph/runtime/reference/atan2.hpp"
#include "ngraph/runtime/reference/atanh.hpp"
#include "ngraph/runtime/reference/avg_pool.hpp"
#include "ngraph/runtime/reference/batch_mat_mul.hpp"
#include "ngraph/runtime/reference/batch_norm.hpp"
#include "ngraph/runtime/reference/broadcast.hpp"
#include "ngraph/runtime/reference/broadcast_distributed.hpp"
#include "ngraph/runtime/reference/ceiling.hpp"
#include "ngraph/runtime/reference/clamp.hpp"
#include "ngraph/runtime/reference/concat.hpp"
#include "ngraph/runtime/reference/constant.hpp"
#include "ngraph/runtime/reference/convert.hpp"
#include "ngraph/runtime/reference/convolution.hpp"
#include "ngraph/runtime/reference/copy.hpp"
#include "ngraph/runtime/reference/cos.hpp"
#include "ngraph/runtime/reference/cosh.hpp"
#include "ngraph/runtime/reference/cum_sum.hpp"
#include "ngraph/runtime/reference/dequantize.hpp"
#include "ngraph/runtime/reference/divide.hpp"
#include "ngraph/runtime/reference/dot.hpp"
#include "ngraph/runtime/reference/embedding_lookup.hpp"
#include "ngraph/runtime/reference/equal.hpp"
#include "ngraph/runtime/reference/erf.hpp"
#include "ngraph/runtime/reference/exp.hpp"
#include "ngraph/runtime/reference/floor.hpp"
#include "ngraph/runtime/reference/gather.hpp"
#include "ngraph/runtime/reference/gather_nd.hpp"
#include "ngraph/runtime/reference/generate_mask.hpp"
#include "ngraph/runtime/reference/greater.hpp"
#include "ngraph/runtime/reference/greater_equal.hpp"
#include "ngraph/runtime/reference/less.hpp"
#include "ngraph/runtime/reference/less_equal.hpp"
#include "ngraph/runtime/reference/log.hpp"
#include "ngraph/runtime/reference/lrn.hpp"
#include "ngraph/runtime/reference/matmul.hpp"
#include "ngraph/runtime/reference/max.hpp"
#include "ngraph/runtime/reference/max_pool.hpp"
#include "ngraph/runtime/reference/maximum.hpp"
#include "ngraph/runtime/reference/mean.hpp"
#include "ngraph/runtime/reference/min.hpp"
#include "ngraph/runtime/reference/minimum.hpp"
#include "ngraph/runtime/reference/multiply.hpp"
#include "ngraph/runtime/reference/negate.hpp"
#include "ngraph/runtime/reference/not.hpp"
#include "ngraph/runtime/reference/not_equal.hpp"
#include "ngraph/runtime/reference/one_hot.hpp"
#include "ngraph/runtime/reference/or.hpp"
#include "ngraph/runtime/reference/pad.hpp"
#include "ngraph/runtime/reference/power.hpp"
#include "ngraph/runtime/reference/product.hpp"
#include "ngraph/runtime/reference/quantize.hpp"
#include "ngraph/runtime/reference/random_uniform.hpp"
#include "ngraph/runtime/reference/range.hpp"
#include "ngraph/runtime/reference/recv.hpp"
#include "ngraph/runtime/reference/relu.hpp"
#include "ngraph/runtime/reference/replace_slice.hpp"
#include "ngraph/runtime/reference/reshape.hpp"
#include "ngraph/runtime/reference/result.hpp"
#include "ngraph/runtime/reference/reverse.hpp"
#include "ngraph/runtime/reference/reverse_sequence.hpp"
#include "ngraph/runtime/reference/round.hpp"
#include "ngraph/runtime/reference/scatter_add.hpp"
#include "ngraph/runtime/reference/scatter_nd_add.hpp"
#include "ngraph/runtime/reference/select.hpp"
#include "ngraph/runtime/reference/send.hpp"
#include "ngraph/runtime/reference/shape_of.hpp"
#include "ngraph/runtime/reference/sigmoid.hpp"
#include "ngraph/runtime/reference/sign.hpp"
#include "ngraph/runtime/reference/sin.hpp"
#include "ngraph/runtime/reference/sinh.hpp"
#include "ngraph/runtime/reference/slice.hpp"
#include "ngraph/runtime/reference/softmax.hpp"
#include "ngraph/runtime/reference/sqrt.hpp"
#include "ngraph/runtime/reference/strided_slice.hpp"
#include "ngraph/runtime/reference/subtract.hpp"
#include "ngraph/runtime/reference/sum.hpp"
#include "ngraph/runtime/reference/tan.hpp"
#include "ngraph/runtime/reference/tanh.hpp"
#include "ngraph/runtime/reference/topk.hpp"
#include "ngraph/runtime/reference/xor.hpp"
#include "ngraph/runtime/tensor.hpp"
#include "ngraph/slice_plan.hpp"
#include "ngraph/state/bernoulli_rng_state.hpp"
#include "ngraph/state/uniform_rng_state.hpp"
#include "ngraph/util.hpp"

namespace ngraph
{
    namespace runtime
    {
        namespace interpreter
        {
            class INTBackend;
            class INTExecutable;

            // This expands the op list in op_tbl.hpp into a list of enumerations that look like
            // this:
            // Abs,
            // Acos,
            // ...
            enum class OP_TYPEID
            {
#define NGRAPH_OP(NAME, VERSION) NAME##_v##VERSION,
#include "ngraph/op_version_tbl.hpp"
#undef NGRAPH_OP
                UnknownOp
            };
        }
    }
}

class INTERPRETER_BACKEND_API ngraph::runtime::interpreter::INTExecutable : public Executable
{
    friend class INTBackend;

public:
    INTExecutable(const std::shared_ptr<Function>& function,
                  bool enable_performance_collection = false);

    bool call(const std::vector<std::shared_ptr<Tensor>>& outputs,
              const std::vector<std::shared_ptr<Tensor>>& inputs) override;

    virtual void save(std::ostream& output_stream) override;

    void set_nan_check(bool enable);

    std::vector<PerformanceCounter> get_performance_data() const override;

    std::shared_ptr<runtime::Tensor> create_input_tensor(size_t input_index) override;

    std::shared_ptr<runtime::Tensor> create_output_tensor(size_t output_index) override;

    std::vector<std::shared_ptr<runtime::Tensor>>
        create_input_tensor(size_t input_index, size_t pipeline_depth) override;

    std::vector<std::shared_ptr<runtime::Tensor>>
        create_output_tensor(size_t output_index, size_t pipeline_depth) override;

protected:
    INTExecutable(const std::string& model_string);

    template <typename T>
    std::vector<T> as_vector(const HostTensor* tensor) const
    {
        vector<T> result;
        switch (tensor->get_element_type())
        {
        case element::Type_t::i8:
        {
            const int8_t* p = tensor->get_data_ptr<const int8_t>();
            for (size_t i = 0; i < tensor->get_element_count(); ++i)
            {
                result.push_back(static_cast<T>(p[i]));
            }
            break;
        }
        case element::Type_t::i16:
        {
            const int16_t* p = tensor->get_data_ptr<const int16_t>();
            for (size_t i = 0; i < tensor->get_element_count(); ++i)
            {
                result.push_back(static_cast<T>(p[i]));
            }
            break;
        }
        case element::Type_t::i32:
        {
            const int32_t* p = tensor->get_data_ptr<const int32_t>();
            for (size_t i = 0; i < tensor->get_element_count(); ++i)
            {
                result.push_back(static_cast<T>(p[i]));
            }
            break;
        }
        case element::Type_t::i64:
        {
            const int64_t* p = tensor->get_data_ptr<const int64_t>();
            for (size_t i = 0; i < tensor->get_element_count(); ++i)
            {
                result.push_back(static_cast<T>(p[i]));
            }
            break;
        }
        case element::Type_t::u8:
        {
            const uint8_t* p = tensor->get_data_ptr<const uint8_t>();
            for (size_t i = 0; i < tensor->get_element_count(); ++i)
            {
                result.push_back(static_cast<T>(p[i]));
            }
            break;
        }
        case element::Type_t::u16:
        {
            const uint16_t* p = tensor->get_data_ptr<const uint16_t>();
            for (size_t i = 0; i < tensor->get_element_count(); ++i)
            {
                result.push_back(static_cast<T>(p[i]));
            }
            break;
        }
        case element::Type_t::u32:
        {
            const uint32_t* p = tensor->get_data_ptr<const uint32_t>();
            for (size_t i = 0; i < tensor->get_element_count(); ++i)
            {
                result.push_back(static_cast<T>(p[i]));
            }
            break;
        }
        case element::Type_t::u64:
        {
            const uint64_t* p = tensor->get_data_ptr<const uint64_t>();
            for (size_t i = 0; i < tensor->get_element_count(); ++i)
            {
                result.push_back(static_cast<T>(p[i]));
            }
            break;
        }
        default: throw runtime_error("Unsupported type reading int64_t tensor"); break;
        }
        return result;
    }

    Coordinate as_coordinate(const HostTensor* tensor) const;
    Strides as_strides(const HostTensor* tensor) const;
    Shape as_shape(const HostTensor* tensor) const;
    AxisSet as_axis_set(const HostTensor* tensor) const;
    AxisVector as_axis_vector(const HostTensor* tensor) const;

    std::shared_ptr<ngraph::op::v0::Parameter> get_parameter(size_t index) const;
    std::shared_ptr<ngraph::op::v0::Result> get_result(size_t index) const;
    int get_alignment() const { return 64; }
    bool m_is_compiled = false;
    bool m_nan_check_enabled = false;
    bool m_performance_counters_enabled = false;
    std::shared_ptr<Function> m_function;
    std::unordered_map<std::shared_ptr<const Node>, stopwatch> m_timer_map;
    NodeVector m_nodes;
    std::unordered_map<const Node*, std::shared_ptr<State>> m_states;
    std::set<std::string> m_unsupported_op_name_list;

    static OP_TYPEID get_typeid(const Node& node);

    static void perform_nan_check(const std::vector<std::shared_ptr<HostTensor>>&,
                                  const Node* op = nullptr);

    virtual void generate_calls(const element::Type& type,
                                const Node& op,
                                const std::vector<std::shared_ptr<HostTensor>>& outputs,
                                const std::vector<std::shared_ptr<HostTensor>>& inputs);

    template <typename T>
    void op_engine(const Node& node,
                   const std::vector<std::shared_ptr<HostTensor>>& out,
                   const std::vector<std::shared_ptr<HostTensor>>& args)
    {
// We want to check that every OP_TYPEID enumeration is included in the list.
// These GCC flags enable compile-time checking so that if an enumeration
// is not in the list an error is generated.
#if defined(__GNUC__) && !(__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wswitch"
#pragma GCC diagnostic error "-Wswitch-enum"
#endif
        switch (get_typeid(node))
        {
        case OP_TYPEID::Abs_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::abs<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Acos_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::acos<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Acosh_v3:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::acosh<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Add_v1:
        {
            const op::v1::Add* add = static_cast<const op::v1::Add*>(&node);
            Shape output_shape =
                add->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::add<T>(args[0]->get_data_ptr<const T>(),
                              args[1]->get_data_ptr<const T>(),
                              out[0]->get_data_ptr<T>(),
                              args[0]->get_shape(),
                              args[1]->get_shape(),
                              add->get_autob());
            break;
        }
        case OP_TYPEID::All_v0:
        {
            const op::v0::All* all = static_cast<const op::v0::All*>(&node);
            reference::all(args[0]->get_data_ptr<const char>(),
                           out[0]->get_data_ptr<char>(),
                           args[0]->get_shape(),
                           node.get_output_shape(0),
                           all->get_reduction_axes());
            break;
        }
        case OP_TYPEID::AllReduce_v0:
        {
            const ngraph::op::v0::AllReduce* allreduce =
                static_cast<const ngraph::op::v0::AllReduce*>(&node);
            reference::allreduce<T>(args[0]->get_data_ptr<T>(),
                                    out[0]->get_data_ptr<T>(),
                                    node.get_input_element_type(0),
                                    allreduce->get_reduce_type(),
                                    static_cast<int>(shape_size(args[0]->get_shape())));
            break;
        }
        case OP_TYPEID::Any_v0:
        {
            const op::v0::Any* any = static_cast<const op::v0::Any*>(&node);
            reference::any(args[0]->get_data_ptr<const char>(),
                           out[0]->get_data_ptr<char>(),
                           args[0]->get_shape(),
                           node.get_output_shape(0),
                           any->get_reduction_axes());
            break;
        }
        case OP_TYPEID::ArgMin_v0:
        {
            const op::v0::ArgMin* argmin = static_cast<const op::v0::ArgMin*>(&node);
            auto element_type = node.get_output_element_type(0);
            if (element_type == element::i64)
            {
                reference::argmin<T, int64_t>(args[0]->get_data_ptr<const T>(),
                                              out[0]->get_data_ptr<int64_t>(),
                                              args[0]->get_shape(),
                                              node.get_output_shape(0),
                                              argmin->get_reduction_axis());
            }
            else if (element_type == element::i32)
            {
                reference::argmin<T, int32_t>(args[0]->get_data_ptr<const T>(),
                                              out[0]->get_data_ptr<int32_t>(),
                                              args[0]->get_shape(),
                                              node.get_output_shape(0),
                                              argmin->get_reduction_axis());
            }
            else
            {
                throw ngraph_error("Unexpected type");
            }
            break;
        }
        case OP_TYPEID::ArgMax_v0:
        {
            const op::v0::ArgMax* argmax = static_cast<const op::v0::ArgMax*>(&node);
            auto element_type = node.get_output_element_type(0);
            if (element_type == element::i64)
            {
                reference::argmax<T, int64_t>(args[0]->get_data_ptr<const T>(),
                                              out[0]->get_data_ptr<int64_t>(),
                                              args[0]->get_shape(),
                                              node.get_output_shape(0),
                                              argmax->get_reduction_axis());
            }
            else if (element_type == element::i32)
            {
                reference::argmax<T, int32_t>(args[0]->get_data_ptr<const T>(),
                                              out[0]->get_data_ptr<int32_t>(),
                                              args[0]->get_shape(),
                                              node.get_output_shape(0),
                                              argmax->get_reduction_axis());
            }
            else
            {
                throw ngraph_error("Unexpected type");
            }
            break;
        }
        case OP_TYPEID::Asin_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::asin<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Asinh_v3:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::asinh<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Atan_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::atan<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Atanh_v3:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::atanh<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
            break;
        }
        case OP_TYPEID::Atan2_v0:
        {
            const op::v0::Atan2* op = static_cast<const op::v0::Atan2*>(&node);
            Shape output_shape =
                op->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            size_t element_count = shape_size(output_shape);
            reference::atan2<T>(args[0]->get_data_ptr<const T>(),
                                args[1]->get_data_ptr<const T>(),
                                out[0]->get_data_ptr<T>(),
                                element_count);
            break;
        }
        case OP_TYPEID::AvgPool_v0:
        {
            const op::v0::AvgPool* avg_pool = static_cast<const op::v0::AvgPool*>(&node);

            reference::avg_pool<T>(args[0]->get_data_ptr<const T>(),
                                   out[0]->get_data_ptr<T>(),
                                   args[0]->get_shape(),
                                   node.get_output_shape(0),
                                   avg_pool->get_window_shape(),
                                   avg_pool->get_window_movement_strides(),
                                   avg_pool->get_padding_below(),
                                   avg_pool->get_padding_above(),
                                   avg_pool->get_include_padding_in_avg_computation());
            break;
        }
        case OP_TYPEID::BatchMatMul_v0:
        {
            reference::batch_mat_mul(args[0]->get_data_ptr<const T>(),
                                     args[1]->get_data_ptr<const T>(),
                                     out[0]->get_data_ptr<T>(),
                                     args[0]->get_shape(),
                                     args[1]->get_shape(),
                                     node.get_output_shape(0));
            break;
        }

        case OP_TYPEID::BatchNormTraining_v0:
        {
            const ngraph::op::v0::BatchNormTraining* bn =
                static_cast<const ngraph::op::v0::BatchNormTraining*>(&node);
            reference::batch_norm_training<T>(bn->get_eps_value(),
                                              args[0]->get_data_ptr<const T>(),
                                              args[1]->get_data_ptr<const T>(),
                                              args[2]->get_data_ptr<const T>(),
                                              out[0]->get_data_ptr<T>(),
                                              out[1]->get_data_ptr<T>(),
                                              out[2]->get_data_ptr<T>(),
                                              node.get_input_shape(2));
            break;
        }
        case OP_TYPEID::BatchNormInference_v0:
        {
            const ngraph::op::v0::BatchNormInference* bn =
                static_cast<const ngraph::op::v0::BatchNormInference*>(&node);
            reference::batch_norm_inference<T>(bn->get_eps_value(),
                                               args[0]->get_data_ptr<const T>(),
                                               args[1]->get_data_ptr<const T>(),
                                               args[2]->get_data_ptr<const T>(),
                                               args[3]->get_data_ptr<const T>(),
                                               args[4]->get_data_ptr<const T>(),
                                               out[0]->get_data_ptr<T>(),
                                               node.get_input_shape(2));
            break;
        }
        case OP_TYPEID::BatchNormTrainingBackprop_v0:
        {
            const ngraph::op::v0::BatchNormTrainingBackprop* bn_bprop =
                static_cast<const ngraph::op::v0::BatchNormTrainingBackprop*>(&node);
            reference::batch_norm_backprop(bn_bprop->get_eps_value(),
                                           args[0]->get_data_ptr<const T>(),
                                           args[1]->get_data_ptr<const T>(),
                                           args[2]->get_data_ptr<const T>(),
                                           args[3]->get_data_ptr<const T>(),
                                           args[4]->get_data_ptr<const T>(),
                                           args[5]->get_data_ptr<const T>(),
                                           out[0]->get_data_ptr<T>(),
                                           out[1]->get_data_ptr<T>(),
                                           out[2]->get_data_ptr<T>(),
                                           node.get_input_shape(2));
            break;
        }
        case OP_TYPEID::AvgPoolBackprop_v0:
        {
            const op::v0::AvgPoolBackprop* apb = static_cast<const op::v0::AvgPoolBackprop*>(&node);
            reference::avg_pool_backprop<T>(args[0]->get_data_ptr<const T>(),
                                            out[0]->get_data_ptr<T>(),
                                            args[0]->get_shape(),
                                            node.get_output_shape(0),
                                            apb->get_window_shape(),
                                            apb->get_window_movement_strides(),
                                            apb->get_padding_below(),
                                            apb->get_padding_above(),
                                            apb->get_include_padding_in_avg_computation());
            break;
        }
        case OP_TYPEID::Broadcast_v0:
        {
            const op::v0::Broadcast* broadcast = static_cast<const op::v0::Broadcast*>(&node);
            Shape in_shape = args[0]->get_shape();
            Shape out_shape = node.get_output_shape(0);
            AxisSet broadcast_axes = broadcast->get_broadcast_axes();
            reference::broadcast<T>(args[0]->get_data_ptr<const T>(),
                                    out[0]->get_data_ptr<T>(),
                                    in_shape,
                                    out_shape,
                                    broadcast_axes);
            break;
        }
        case OP_TYPEID::BroadcastDistributed_v0:
        {
            const ngraph::op::v0::BroadcastDistributed* broadcast =
                static_cast<const ngraph::op::v0::BroadcastDistributed*>(&node);
            int rank_ID;
            rank_ID = get_distributed_interface()->get_rank();
            int root_id = broadcast->get_root_id();
            if (rank_ID == root_id)
            {
                reference::broadcastdistributed<T>(
                    args[0]->get_data_ptr<T>(),
                    node.get_input_element_type(0),
                    static_cast<int>(shape_size(args[0]->get_shape())),
                    root_id);
                auto memSize = static_cast<int>(shape_size(args[0]->get_shape())) * sizeof(T);
                memcpy(out[0]->get_data_ptr<T>(), args[0]->get_data_ptr<T>(), memSize);
            }
            else
            {
                reference::broadcastdistributed<T>(
                    out[0]->get_data_ptr<T>(),
                    node.get_input_element_type(0),
                    static_cast<int>(shape_size(args[0]->get_shape())),
                    root_id);
            }
            break;
        }
        case OP_TYPEID::BroadcastLike_v0: break;
        case OP_TYPEID::Ceiling_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::ceiling<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Clamp_v0:
        {
            const op::v0::Clamp* clamp = static_cast<const op::v0::Clamp*>(&node);
            out[0]->set_shape(args[0]->get_shape());
            reference::clamp<T>(args[0]->get_data_ptr<const T>(),
                                out[0]->get_data_ptr<T>(),
                                clamp->get_min<T>(),
                                clamp->get_max<T>(),
                                shape_size(args[0]->get_shape()));
            break;
        }
        case OP_TYPEID::Concat_v0:
        {
            const op::v0::Concat* concat = static_cast<const op::v0::Concat*>(&node);
            std::vector<const T*> in_args;
            std::vector<Shape> in_shapes;
            for (size_t i = 0; i < node.get_input_size(); i++)
            {
                in_args.push_back(args[i]->get_data_ptr<const T>());
                in_shapes.push_back(node.get_input_shape(i));
            }
            reference::concat<T>(in_args,
                                 out[0]->get_data_ptr<T>(),
                                 in_shapes,
                                 node.get_output_shape(0),
                                 concat->get_concatenation_axis());
            break;
        }
        case OP_TYPEID::Constant_v0:
        {
            const op::v0::Constant* c = static_cast<const op::v0::Constant*>(&node);
            size_t element_count = shape_size(node.get_output_shape(0));
            reference::constant<T>(c->get_data_ptr<T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Convert_v0:
        {
            // const op::v0::Convert* c = static_cast<const op::v0::Convert*>(&node);
            element::Type type = node.get_output_element_type(0);
            std::stringstream ss;
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            switch (type)
            {
            case element::Type_t::boolean:
                reference::convert_to_bool<T>(
                    args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<char>(), element_count);
                break;
            case element::Type_t::f32:
                reference::convert<T>(
                    args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<float>(), element_count);
                break;
            case element::Type_t::f64:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<double>(),
                                      element_count);
                break;
            case element::Type_t::i8:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<int8_t>(),
                                      element_count);
                break;
            case element::Type_t::i16:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<int16_t>(),
                                      element_count);
                break;
            case element::Type_t::i32:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<int32_t>(),
                                      element_count);
                break;
            case element::Type_t::i64:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<int64_t>(),
                                      element_count);
                break;
            case element::Type_t::u8:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<uint8_t>(),
                                      element_count);
                break;
            case element::Type_t::u16:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<uint16_t>(),
                                      element_count);
                break;
            case element::Type_t::u32:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<uint32_t>(),
                                      element_count);
                break;
            case element::Type_t::u64:
                reference::convert<T>(args[0]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<uint64_t>(),
                                      element_count);
                break;
            case element::Type_t::undefined:
            case element::Type_t::dynamic:
            case element::Type_t::u1:
            case element::Type_t::bf16:
            case element::Type_t::f16:
                ss << "unsupported element type " << type << " op Convert";
                throw std::runtime_error(ss.str());
            }
            break;
        }
        case OP_TYPEID::Convolution_v0:
        {
            const op::v0::Convolution* c = static_cast<const op::v0::Convolution*>(&node);
            reference::convolution<T>(args[0]->get_data_ptr<const T>(),
                                      args[1]->get_data_ptr<const T>(),
                                      out[0]->get_data_ptr<T>(),
                                      args[0]->get_shape(),
                                      args[1]->get_shape(),
                                      node.get_output_shape(0),
                                      c->get_window_movement_strides(),
                                      c->get_window_dilation_strides(),
                                      c->get_padding_below(),
                                      c->get_padding_above(),
                                      c->get_data_dilation_strides());

            break;
        }
        case OP_TYPEID::ConvolutionBackpropFilters_v0:
        {
            const op::v0::ConvolutionBackpropFilters* c =
                static_cast<const op::v0::ConvolutionBackpropFilters*>(&node);
            reference::convolution_backprop_filter<T>(
                args[0]->get_data_ptr<const T>(), // input
                args[1]->get_data_ptr<const T>(), // delta_convolution_output
                out[0]->get_data_ptr<T>(),        // delta_filter
                c->get_input_shape(0),            // input_shape
                c->get_input_shape(1),            // convolution_output_shape
                c->get_filters_shape(),           // filter_shape
                c->get_window_dilation_strides_forward(),
                c->get_window_movement_strides_forward(),
                c->get_padding_below_forward(),
                c->compute_backward_in_pad_above(),
                c->get_data_dilation_strides_forward());
            break;
        }
        case OP_TYPEID::ConvolutionBackpropData_v0:
        {
            // Note that args[1] and args[0] are switched here from the usual order.
            const op::v0::ConvolutionBackpropData* c =
                static_cast<const op::v0::ConvolutionBackpropData*>(&node);
            reference::convolution_backprop_in<T>(args[1]->get_data_ptr<const T>(),
                                                  args[0]->get_data_ptr<const T>(),
                                                  out[0]->get_data_ptr<T>(),
                                                  c->get_input_shape(1),
                                                  c->get_input_shape(0),
                                                  c->get_data_batch_shape(),
                                                  c->get_data_dilation_strides_forward(),
                                                  c->get_window_dilation_strides_forward(),
                                                  c->compute_backward_delta_out_pad_below(),
                                                  c->compute_backward_delta_out_pad_above(),
                                                  c->get_window_movement_strides_forward());
            break;
        }
        case OP_TYPEID::Cos_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::cos<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Cosh_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::cosh<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::CumSum_v0:
        {
            const op::v0::CumSum* cumsum = static_cast<const op::v0::CumSum*>(&node);
            auto axis_et = node.get_input_element_type(1);
            if (axis_et == element::i32)
            {
                reference::cumsum<T, int32_t>(args[0]->get_data_ptr<const T>(),
                                              args[1]->get_data_ptr<const int32_t>(),
                                              out[0]->get_data_ptr<T>(),
                                              args[0]->get_shape(),
                                              cumsum->is_exclusive(),
                                              cumsum->is_reverse());
            }
            else if (axis_et == element::i64)
            {
                reference::cumsum<T, int64_t>(args[0]->get_data_ptr<const T>(),
                                              args[1]->get_data_ptr<const int64_t>(),
                                              out[0]->get_data_ptr<T>(),
                                              args[0]->get_shape(),
                                              cumsum->is_exclusive(),
                                              cumsum->is_reverse());
            }
            break;
        }
        case OP_TYPEID::CropAndResize_v0:
        {
            throw unsupported_op("Unsupported op 'CropAndResize_v0'");
            break;
        }
        case OP_TYPEID::Dequantize_v0:
        {
            const op::v0::Dequantize* dequantize = static_cast<const op::v0::Dequantize*>(&node);
            auto type = dequantize->get_output_element_type(0);

            if (type == element::f32)
            {
                reference::dequantize<T>(args[0]->get_data_ptr<const T>(),
                                         args[1]->get_data_ptr<const float>(),
                                         args[2]->get_data_ptr<const T>(),
                                         out[0]->get_data_ptr<float>(),
                                         args[0]->get_shape(),
                                         args[1]->get_shape(),
                                         dequantize->get_axes());
            }
            else if (type == element::f64)
            {
                reference::dequantize<T>(args[0]->get_data_ptr<const T>(),
                                         args[1]->get_data_ptr<const double>(),
                                         args[2]->get_data_ptr<const T>(),
                                         out[0]->get_data_ptr<double>(),
                                         args[0]->get_shape(),
                                         args[1]->get_shape(),
                                         dequantize->get_axes());
            }
            else
            {
                std::stringstream ss;
                ss << "unsupported element type " << type << " op Dequantize";
                throw std::runtime_error(ss.str());
            }

            break;
        }
        case OP_TYPEID::Divide_v1:
        {
            const op::v1::Divide* divide = static_cast<const op::v1::Divide*>(&node);
            Shape output_shape =
                divide->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::divide<T>(args[0]->get_data_ptr<const T>(),
                                 args[1]->get_data_ptr<const T>(),
                                 out[0]->get_data_ptr<T>(),
                                 args[0]->get_shape(),
                                 args[1]->get_shape(),
                                 divide->get_autob(),
                                 divide->is_pythondiv());
            break;
        }
        case OP_TYPEID::Dot_v0:
        {
            const op::v0::Dot* dot = static_cast<const op::v0::Dot*>(&node);

            reference::dot(args[0]->get_data_ptr<const T>(),
                           args[1]->get_data_ptr<const T>(),
                           out[0]->get_data_ptr<T>(),
                           args[0]->get_shape(),
                           args[1]->get_shape(),
                           node.get_output_shape(0),
                           dot->get_reduction_axes_count());
            break;
        }
        case OP_TYPEID::DynSlice_v0:
        {
            const op::v0::DynSlice* op = static_cast<const op::v0::DynSlice*>(&node);
            Shape input_shape = args[0]->get_shape();
            SlicePlan slice_plan = make_slice_plan(input_shape,
                                                   as_vector<int64_t>(args[1].get()),
                                                   as_vector<int64_t>(args[2].get()),
                                                   as_vector<int64_t>(args[3].get()),
                                                   op->get_lower_bounds_mask(),
                                                   op->get_upper_bounds_mask(),
                                                   op->get_new_axis(),
                                                   op->get_shrink_axis(),
                                                   op->get_ellipsis_mask());
            out[0]->set_shape(slice_plan.reshape_out_shape);
            reference::strided_slice<T>(args[0]->get_data_ptr<const T>(),
                                        out[0]->get_data_ptr<T>(),
                                        input_shape,
                                        slice_plan);
            break;
        }
        case OP_TYPEID::EmbeddingLookup_v0:
        {
            const op::v0::EmbeddingLookup* embed =
                static_cast<const op::v0::EmbeddingLookup*>(&node);
            auto type = embed->input(0).get_element_type();
            size_t element_count = shape_size(embed->get_input_shape(0));

            if (type == element::f32)
            {
                reference::embedding<T, float>(args[0]->get_data_ptr<const float>(),
                                               args[1]->get_data_ptr<const T>(),
                                               out[0]->get_data_ptr<T>(),
                                               element_count,
                                               embed->get_output_shape(0));
            }
            else if (type == element::f64)
            {
                reference::embedding<T, double>(args[0]->get_data_ptr<const double>(),
                                                args[1]->get_data_ptr<const T>(),
                                                out[0]->get_data_ptr<T>(),
                                                element_count,
                                                embed->get_output_shape(0));
            }
            else if (type == element::i32)
            {
                reference::embedding<T, int32_t>(args[0]->get_data_ptr<const int>(),
                                                 args[1]->get_data_ptr<const T>(),
                                                 out[0]->get_data_ptr<T>(),
                                                 element_count,
                                                 embed->get_output_shape(0));
            }
            else if (type == element::i64)
            {
                reference::embedding<T, int64_t>(args[0]->get_data_ptr<const int64_t>(),
                                                 args[1]->get_data_ptr<const T>(),
                                                 out[0]->get_data_ptr<T>(),
                                                 element_count,
                                                 embed->get_output_shape(0));
            }
            else
            {
                throw ngraph_error(std::string("Unsupported index type ") + type.c_type_string() +
                                   std::string("in EmbeddingLookup"));
            }
            break;
        }
        case OP_TYPEID::Equal_v1:
        {
            auto equal = static_cast<const op::v1::Equal*>(&node);
            Shape output_shape =
                equal->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::equal<T>(args[0]->get_data_ptr<const T>(),
                                args[1]->get_data_ptr<const T>(),
                                out[0]->get_data_ptr<char>(),
                                args[0]->get_shape(),
                                args[1]->get_shape(),
                                equal->get_autob());
            break;
        }
        case OP_TYPEID::Erf_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::erf<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Exp_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::exp<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
#ifdef INTERPRETER_USE_HYBRID
        case OP_TYPEID::FunctionCall_v0:
        {
            auto f = static_cast<const runtime::hybrid::op::v0::FunctionCall*>(&node);
            auto backend = f->get_backend();
            auto executable = f->get_executable();

            std::vector<std::shared_ptr<Tensor>> outputs;
            std::vector<std::shared_ptr<Tensor>> inputs;
            for (const std::shared_ptr<HostTensor>& t : out)
            {
                auto backend_tensor = backend->create_tensor(
                    t->get_element_type(), t->get_shape(), t->get_data_ptr());
                outputs.push_back(backend_tensor);
            }
            for (const std::shared_ptr<HostTensor>& t : args)
            {
                auto backend_tensor = backend->create_tensor(
                    t->get_element_type(), t->get_shape(), t->get_data_ptr());
                inputs.push_back(backend_tensor);
            }
            executable->call(outputs, inputs);
            break;
        }
#endif
        case OP_TYPEID::Floor_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::floor<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Gather_v0:
        {
            const op::v0::Gather* gather = static_cast<const op::v0::Gather*>(&node);
            if (node.get_input_element_type(1) == element::i64)
            {
                reference::gather<T, int64_t>(args[0]->get_data_ptr<T>(),
                                              args[1]->get_data_ptr<int64_t>(),
                                              out[0]->get_data_ptr<T>(),
                                              args[0]->get_shape(),
                                              args[1]->get_shape(),
                                              node.get_output_shape(0),
                                              gather->get_axis());
            }
            else if (node.get_input_element_type(1) == element::i32)
            {
                reference::gather<T, int32_t>(args[0]->get_data_ptr<T>(),
                                              args[1]->get_data_ptr<int32_t>(),
                                              out[0]->get_data_ptr<T>(),
                                              args[0]->get_shape(),
                                              args[1]->get_shape(),
                                              node.get_output_shape(0),
                                              gather->get_axis());
            }
            else
            {
                throw ngraph_error("Unexpected type");
            }
            break;
        }
        case OP_TYPEID::GatherND_v0:
        {
            if (node.get_input_element_type(1) == element::i64)
            {
                reference::gather_nd<T, int64_t>(args[0]->get_data_ptr<T>(),
                                                 args[1]->get_data_ptr<int64_t>(),
                                                 out[0]->get_data_ptr<T>(),
                                                 args[0]->get_shape(),
                                                 args[1]->get_shape(),
                                                 node.get_output_shape(0));
            }
            else if (node.get_input_element_type(1) == element::i32)
            {
                reference::gather_nd<T, int32_t>(args[0]->get_data_ptr<T>(),
                                                 args[1]->get_data_ptr<int32_t>(),
                                                 out[0]->get_data_ptr<T>(),
                                                 args[0]->get_shape(),
                                                 args[1]->get_shape(),
                                                 node.get_output_shape(0));
            }
            else
            {
                throw ngraph_error("Unexpected type");
            }
            break;
        }
        case OP_TYPEID::GenerateMask_v0:
        {
            bool use_seed = static_cast<bool>(args[2]->get_data_ptr<const int32_t>()[0]);
            if (m_states.count(&node) == 0)
            {
                const op::v0::GenerateMask* gm = static_cast<const op::v0::GenerateMask*>(&node);
                auto seed = use_seed ? gm->get_seed() : 0;
                m_states[&node] =
                    std::unique_ptr<State>(new BernoulliRNGState(seed, gm->get_probability()));
            }

            bool training = static_cast<bool>(args[0]->get_data_ptr<const T>()[0]);
            auto state = static_cast<BernoulliRNGState*>(m_states.at(&node).get());
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            if (!use_seed)
            {
                reference::generate_mask<T>(
                    out[0]->get_data_ptr<T>(), element_count, state, training);
            }
            else
            {
                uint64_t seed = static_cast<uint64_t>(args[3]->get_data_ptr<const T>()[0]);
                double prob = static_cast<double>(args[4]->get_data_ptr<const T>()[0]);
                reference::generate_mask_no_state<T>(
                    out[0]->get_data_ptr<T>(), element_count, training, seed, prob);
            }
            break;
        }
        case OP_TYPEID::Greater_v1:
        {
            auto greater = static_cast<const op::v1::Greater*>(&node);
            Shape output_shape =
                greater->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::greater<T>(args[0]->get_data_ptr<const T>(),
                                  args[1]->get_data_ptr<const T>(),
                                  out[0]->get_data_ptr<char>(),
                                  args[0]->get_shape(),
                                  args[1]->get_shape(),
                                  greater->get_autob());
            break;
        }
        case OP_TYPEID::GreaterEqual_v1:
        {
            auto greater_eq = static_cast<const op::v1::GreaterEq*>(&node);
            Shape output_shape =
                greater_eq->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::greater_eq<T>(args[0]->get_data_ptr<const T>(),
                                     args[1]->get_data_ptr<const T>(),
                                     out[0]->get_data_ptr<char>(),
                                     args[0]->get_shape(),
                                     args[1]->get_shape(),
                                     greater_eq->get_autob());
            break;
        }
        case OP_TYPEID::Less_v1:
        {
            auto less = static_cast<const op::v1::Less*>(&node);
            Shape output_shape =
                less->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::less<T>(args[0]->get_data_ptr<const T>(),
                               args[1]->get_data_ptr<const T>(),
                               out[0]->get_data_ptr<char>(),
                               args[0]->get_shape(),
                               args[1]->get_shape(),
                               less->get_autob());
            break;
        }
        case OP_TYPEID::LessEqual_v1:
        {
            auto less_equal = static_cast<const op::v1::LessEqual*>(&node);
            reference::less_equal<T>(args[0]->get_data_ptr<const T>(),
                                     args[1]->get_data_ptr<const T>(),
                                     out[0]->get_data_ptr<char>(),
                                     args[0]->get_shape(),
                                     args[1]->get_shape(),
                                     less_equal->get_autob());
            break;
        }
        case OP_TYPEID::Log_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::log<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::LogicalAnd_v1:
        {
            auto logical_and = static_cast<const op::v1::LogicalAnd*>(&node);
            reference::logical_and(args[0]->get_data_ptr<const T>(),
                                   args[1]->get_data_ptr<const T>(),
                                   out[0]->get_data_ptr<T>(),
                                   args[0]->get_shape(),
                                   args[1]->get_shape(),
                                   logical_and->get_autob());
            break;
        }
        case OP_TYPEID::LogicalNot_v1:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::logical_not(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::LogicalOr_v1:
        {
            auto logical_or = static_cast<const op::v1::LogicalOr*>(&node);
            reference::logical_or(args[0]->get_data_ptr<const T>(),
                                  args[1]->get_data_ptr<const T>(),
                                  out[0]->get_data_ptr<T>(),
                                  args[0]->get_shape(),
                                  args[1]->get_shape(),
                                  logical_or->get_autob());
            break;
        }
        case OP_TYPEID::LogicalXor_v1:
        {
            auto logical_xor = static_cast<const op::v1::LogicalXor*>(&node);
            reference::logical_xor(args[0]->get_data_ptr<const T>(),
                                   args[1]->get_data_ptr<const T>(),
                                   out[0]->get_data_ptr<T>(),
                                   args[0]->get_shape(),
                                   args[1]->get_shape(),
                                   logical_xor->get_autob());
            break;
        }
        case OP_TYPEID::LRN_v0:
        {
            const op::v0::LRN* lrn = static_cast<const op::v0::LRN*>(&node);
            reference::lrn<T>(args[0]->get_data_ptr<const T>(),
                              lrn->get_reduction_axes(),
                              out[0]->get_data_ptr<T>(),
                              args[0]->get_shape(),
                              lrn->get_alpha(),
                              lrn->get_beta(),
                              lrn->get_bias(),
                              lrn->get_nsize());
            break;
        }
        case OP_TYPEID::MatMul_v0:
        {
            const op::v0::MatMul* op = static_cast<const op::v0::MatMul*>(&node);
            reference::matmul<T>(args[0]->get_data_ptr<const T>(),
                                 args[1]->get_data_ptr<const T>(),
                                 out[0]->get_data_ptr<T>(),
                                 args[0]->get_shape(),
                                 args[1]->get_shape(),
                                 out[0]->get_shape(),
                                 op->get_transpose_a(),
                                 op->get_transpose_b());
            break;
        }
        case OP_TYPEID::Max_v0:
        {
            const op::v0::Max* max = static_cast<const op::v0::Max*>(&node);
            reference::max<T>(args[0]->get_data_ptr<const T>(),
                              out[0]->get_data_ptr<T>(),
                              args[0]->get_shape(),
                              max->get_reduction_axes());
            break;
        }
        case OP_TYPEID::Maximum_v1:
        {
            auto maximum = static_cast<const op::v1::Maximum*>(&node);
            Shape output_shape =
                maximum->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::maximum<T>(args[0]->get_data_ptr<const T>(),
                                  args[1]->get_data_ptr<const T>(),
                                  out[0]->get_data_ptr<T>(),
                                  args[0]->get_shape(),
                                  args[1]->get_shape(),
                                  maximum->get_autob());
            break;
        }
        case OP_TYPEID::MaxPool_v0:
        {
            const op::v0::MaxPool* max_pool = static_cast<const op::v0::MaxPool*>(&node);

            reference::max_pool<T>(args[0]->get_data_ptr<const T>(),
                                   out[0]->get_data_ptr<T>(),
                                   args[0]->get_shape(),
                                   node.get_output_shape(0),
                                   max_pool->get_window_shape(),
                                   max_pool->get_window_movement_strides(),
                                   max_pool->get_padding_below(),
                                   max_pool->get_padding_above());
            break;
        }
        case OP_TYPEID::MaxPoolBackprop_v0:
        {
            const op::v0::MaxPoolBackprop* max_pool_backprop =
                static_cast<const op::v0::MaxPoolBackprop*>(&node);

            reference::max_pool_backprop<T>(args[0]->get_data_ptr<const T>(),
                                            args[1]->get_data_ptr<const T>(),
                                            out[0]->get_data_ptr<T>(),
                                            args[1]->get_shape(),
                                            node.get_output_shape(0),
                                            max_pool_backprop->get_window_shape(),
                                            max_pool_backprop->get_window_movement_strides(),
                                            max_pool_backprop->get_padding_below(),
                                            max_pool_backprop->get_padding_above());
            break;
        }
        case OP_TYPEID::Min_v0:
        {
            const op::v0::Min* min = static_cast<const op::v0::Min*>(&node);
            reference::min<T>(args[0]->get_data_ptr<const T>(),
                              out[0]->get_data_ptr<T>(),
                              args[0]->get_shape(),
                              min->get_reduction_axes());
            break;
        }
        case OP_TYPEID::Minimum_v1:
        {
            auto minimum = static_cast<const op::v1::Minimum*>(&node);
            Shape output_shape =
                minimum->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::minimum<T>(args[0]->get_data_ptr<const T>(),
                                  args[1]->get_data_ptr<const T>(),
                                  out[0]->get_data_ptr<T>(),
                                  args[0]->get_shape(),
                                  args[1]->get_shape(),
                                  minimum->get_autob());
            break;
        }
        case OP_TYPEID::Multiply_v1:
        {
            auto multiply = static_cast<const op::v1::Multiply*>(&node);
            Shape output_shape =
                multiply->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::multiply<T>(args[0]->get_data_ptr<const T>(),
                                   args[1]->get_data_ptr<const T>(),
                                   out[0]->get_data_ptr<T>(),
                                   args[0]->get_shape(),
                                   args[1]->get_shape(),
                                   multiply->get_autob());
            break;
        }
        case OP_TYPEID::Negative_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::negate<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::NotEqual_v1:
        {
            auto not_equal = static_cast<const op::v1::NotEqual*>(&node);
            Shape output_shape =
                not_equal->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::not_equal<T>(args[0]->get_data_ptr<const T>(),
                                    args[1]->get_data_ptr<const T>(),
                                    out[0]->get_data_ptr<char>(),
                                    args[0]->get_shape(),
                                    args[1]->get_shape(),
                                    not_equal->get_autob());
            break;
        }
        case OP_TYPEID::OneHot_v0:
        {
            const op::v0::OneHot* oh = static_cast<const op::v0::OneHot*>(&node);
            reference::one_hot<T>(args[0]->get_data_ptr<const T>(),
                                  out[0]->get_data_ptr<T>(),
                                  args[0]->get_shape(),
                                  node.get_output_shape(0),
                                  oh->get_one_hot_axis());
            break;
        }
        case OP_TYPEID::Parameter_v0: break;
        case OP_TYPEID::Passthrough_v0:
        {
            const op::v0::Passthrough* passthrough = static_cast<const op::v0::Passthrough*>(&node);
            throw unsupported_op{"Unsupported op 'Passthrough_v0'"};
        }
        case OP_TYPEID::Pad_v0:
        {
            const op::v0::Pad* pad = static_cast<const op::v0::Pad*>(&node);

            reference::pad(args[0]->get_data_ptr<const T>(),
                           args[1]->get_data_ptr<const T>(),
                           out[0]->get_data_ptr<T>(),
                           args[0]->get_shape(),
                           node.get_output_shape(0),
                           pad->get_padding_below(),
                           pad->get_padding_above(),
                           pad->get_pad_mode());
            break;
        }
        case OP_TYPEID::Power_v1:
        {
            auto power = static_cast<const op::v1::Power*>(&node);
            Shape output_shape =
                power->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::power<T>(args[0]->get_data_ptr<const T>(),
                                args[1]->get_data_ptr<const T>(),
                                out[0]->get_data_ptr<T>(),
                                args[0]->get_shape(),
                                args[1]->get_shape(),
                                power->get_autob());
            break;
        }
        case OP_TYPEID::Product_v0:
        {
            const op::v0::Product* product = static_cast<const op::v0::Product*>(&node);
            reference::product<T>(args[0]->get_data_ptr<const T>(),
                                  out[0]->get_data_ptr<T>(),
                                  args[0]->get_shape(),
                                  product->get_reduction_axes());
            break;
        }
        case OP_TYPEID::Quantize_v0:
        {
            const op::v0::Quantize* quantize = static_cast<const op::v0::Quantize*>(&node);
            auto type = quantize->get_output_element_type(0);

            if (type == element::u8)
            {
                reference::quantize<T>(args[0]->get_data_ptr<const T>(),
                                       args[1]->get_data_ptr<const T>(),
                                       args[2]->get_data_ptr<const uint8_t>(),
                                       out[0]->get_data_ptr<uint8_t>(),
                                       args[0]->get_shape(),
                                       args[1]->get_shape(),
                                       quantize->get_axes(),
                                       quantize->get_round_mode());
            }
            else if (type == element::i8)
            {
                reference::quantize<T>(args[0]->get_data_ptr<const T>(),
                                       args[1]->get_data_ptr<const T>(),
                                       args[2]->get_data_ptr<const int8_t>(),
                                       out[0]->get_data_ptr<int8_t>(),
                                       args[0]->get_shape(),
                                       args[1]->get_shape(),
                                       quantize->get_axes(),
                                       quantize->get_round_mode());
            }
            else if (type == element::i32)
            {
                reference::quantize<T>(args[0]->get_data_ptr<const T>(),
                                       args[1]->get_data_ptr<const T>(),
                                       args[2]->get_data_ptr<const int32_t>(),
                                       out[0]->get_data_ptr<int32_t>(),
                                       args[0]->get_shape(),
                                       args[1]->get_shape(),
                                       quantize->get_axes(),
                                       quantize->get_round_mode());
            }
            else
            {
                std::stringstream ss;
                ss << "unsupported element type " << type << " op Quantize";
                throw std::runtime_error(ss.str());
            }

            break;
        }

        case OP_TYPEID::QuantizedConvolution_v0:
        {
            const op::v0::QuantizedConvolution* qc =
                static_cast<const op::v0::QuantizedConvolution*>(&node);

            auto input_element_type = qc->get_input_element_type(0);
            auto filter_element_type = qc->get_input_element_type(1);
            auto output_element_type = qc->get_output_element_type(0);

            if (input_element_type == element::u8 && filter_element_type == element::i8 &&
                output_element_type == element::i8)
            {
                reference::convolution<uint8_t, int8_t, int8_t, int32_t>(
                    args[0]->get_data_ptr<const uint8_t>(),
                    args[1]->get_data_ptr<const int8_t>(),
                    out[0]->get_data_ptr<int8_t>(),
                    args[0]->get_shape(),
                    args[1]->get_shape(),
                    node.get_output_shape(0),
                    qc->get_window_movement_strides(),
                    qc->get_window_dilation_strides(),
                    qc->get_padding_below(),
                    qc->get_padding_above(),
                    qc->get_data_dilation_strides(),
                    args[2]->get_data_ptr<const float>(),
                    args[3]->get_data_ptr<const uint8_t>(),
                    args[4]->get_data_ptr<const float>(),
                    args[5]->get_data_ptr<const int8_t>(),
                    args[6]->get_data_ptr<const float>(),
                    args[7]->get_data_ptr<const int8_t>());
            }
            else if (input_element_type == element::u8 && filter_element_type == element::u8 &&
                     output_element_type == element::u8)
            {
                reference::convolution<uint8_t, uint8_t, uint8_t, int32_t>(
                    args[0]->get_data_ptr<const uint8_t>(),
                    args[1]->get_data_ptr<const uint8_t>(),
                    out[0]->get_data_ptr<uint8_t>(),
                    args[0]->get_shape(),
                    args[1]->get_shape(),
                    node.get_output_shape(0),
                    qc->get_window_movement_strides(),
                    qc->get_window_dilation_strides(),
                    qc->get_padding_below(),
                    qc->get_padding_above(),
                    qc->get_data_dilation_strides(),
                    args[2]->get_data_ptr<const float>(),
                    args[3]->get_data_ptr<const uint8_t>(),
                    args[4]->get_data_ptr<const float>(),
                    args[5]->get_data_ptr<const uint8_t>(),
                    args[6]->get_data_ptr<const float>(),
                    args[7]->get_data_ptr<const uint8_t>());
            }
            else if (input_element_type == element::u8 && filter_element_type == element::i8 &&
                     output_element_type == element::i32)
            {
                reference::convolution<uint8_t, int8_t, int32_t, int32_t>(
                    args[0]->get_data_ptr<const uint8_t>(),
                    args[1]->get_data_ptr<const int8_t>(),
                    out[0]->get_data_ptr<int32_t>(),
                    args[0]->get_shape(),
                    args[1]->get_shape(),
                    node.get_output_shape(0),
                    qc->get_window_movement_strides(),
                    qc->get_window_dilation_strides(),
                    qc->get_padding_below(),
                    qc->get_padding_above(),
                    qc->get_data_dilation_strides(),
                    args[2]->get_data_ptr<const float>(),
                    args[3]->get_data_ptr<const uint8_t>(),
                    args[4]->get_data_ptr<const float>(),
                    args[5]->get_data_ptr<const int8_t>(),
                    args[6]->get_data_ptr<const float>(),
                    args[7]->get_data_ptr<const int32_t>());
            }
            else if (input_element_type == element::u8 && filter_element_type == element::u8 &&
                     output_element_type == element::i32)
            {
                reference::convolution<uint8_t, uint8_t, int32_t, int32_t>(
                    args[0]->get_data_ptr<const uint8_t>(),
                    args[1]->get_data_ptr<const uint8_t>(),
                    out[0]->get_data_ptr<int32_t>(),
                    args[0]->get_shape(),
                    args[1]->get_shape(),
                    node.get_output_shape(0),
                    qc->get_window_movement_strides(),
                    qc->get_window_dilation_strides(),
                    qc->get_padding_below(),
                    qc->get_padding_above(),
                    qc->get_data_dilation_strides(),
                    args[2]->get_data_ptr<const float>(),
                    args[3]->get_data_ptr<const uint8_t>(),
                    args[4]->get_data_ptr<const float>(),
                    args[5]->get_data_ptr<const uint8_t>(),
                    args[6]->get_data_ptr<const float>(),
                    args[7]->get_data_ptr<const int32_t>());
            }
            else
            {
                std::stringstream ss;
                ss << "unsupported element type";
                throw std::runtime_error(ss.str());
            }

            break;
        }

        case OP_TYPEID::QuantizedConvolutionBias_v0:
        case OP_TYPEID::QuantizedConvolutionBiasAdd_v0:
        case OP_TYPEID::QuantizedConvolutionBiasSignedAdd_v0:
        case OP_TYPEID::QuantizedConvolutionRelu_v0:
        case OP_TYPEID::QuantizedDotBias_v0:
        case OP_TYPEID::QuantizedDot_v0:
        {
            const op::v0::QuantizedDot* qd = static_cast<const op::v0::QuantizedDot*>(&node);

            auto input0_element_type = qd->get_input_element_type(0);
            auto input1_element_type = qd->get_input_element_type(1);
            auto output_element_type = qd->get_output_element_type(0);

            if (input0_element_type == element::u8 && input1_element_type == element::i8 &&
                output_element_type == element::i8)
            {
                reference::dot<uint8_t, int8_t, int8_t, int32_t>(
                    args[0]->get_data_ptr<const uint8_t>(),
                    args[1]->get_data_ptr<const int8_t>(),
                    out[0]->get_data_ptr<int8_t>(),
                    args[0]->get_shape(),
                    args[1]->get_shape(),
                    node.get_output_shape(0),
                    1,
                    args[2]->get_data_ptr<const float>(),
                    args[3]->get_data_ptr<const uint8_t>(),
                    args[4]->get_data_ptr<const float>(),
                    args[5]->get_data_ptr<const int8_t>(),
                    args[6]->get_data_ptr<const float>(),
                    args[7]->get_data_ptr<const int8_t>());
            }
            else if (input0_element_type == element::u8 && input1_element_type == element::u8 &&
                     output_element_type == element::u8)
            {
                reference::dot<uint8_t, uint8_t, uint8_t, int32_t>(
                    args[0]->get_data_ptr<const uint8_t>(),
                    args[1]->get_data_ptr<const uint8_t>(),
                    out[0]->get_data_ptr<uint8_t>(),
                    args[0]->get_shape(),
                    args[1]->get_shape(),
                    node.get_output_shape(0),
                    1,
                    args[2]->get_data_ptr<const float>(),
                    args[3]->get_data_ptr<const uint8_t>(),
                    args[4]->get_data_ptr<const float>(),
                    args[5]->get_data_ptr<const uint8_t>(),
                    args[6]->get_data_ptr<const float>(),
                    args[7]->get_data_ptr<const uint8_t>());
            }
            else if (input0_element_type == element::u8 && input1_element_type == element::u8 &&
                     output_element_type == element::i32)
            {
                reference::dot<uint8_t, uint8_t, int32_t, int32_t>(
                    args[0]->get_data_ptr<const uint8_t>(),
                    args[1]->get_data_ptr<const uint8_t>(),
                    out[0]->get_data_ptr<int32_t>(),
                    args[0]->get_shape(),
                    args[1]->get_shape(),
                    node.get_output_shape(0),
                    1,
                    args[2]->get_data_ptr<const float>(),
                    args[3]->get_data_ptr<const uint8_t>(),
                    args[4]->get_data_ptr<const float>(),
                    args[5]->get_data_ptr<const uint8_t>(),
                    args[6]->get_data_ptr<const float>(),
                    args[7]->get_data_ptr<const int32_t>());
            }
            else if (input0_element_type == element::u8 && input1_element_type == element::i8 &&
                     output_element_type == element::i32)
            {
                reference::dot<uint8_t, int8_t, int32_t, int32_t>(
                    args[0]->get_data_ptr<const uint8_t>(),
                    args[1]->get_data_ptr<const int8_t>(),
                    out[0]->get_data_ptr<int32_t>(),
                    args[0]->get_shape(),
                    args[1]->get_shape(),
                    node.get_output_shape(0),
                    1,
                    args[2]->get_data_ptr<const float>(),
                    args[3]->get_data_ptr<const uint8_t>(),
                    args[4]->get_data_ptr<const float>(),
                    args[5]->get_data_ptr<const int8_t>(),
                    args[6]->get_data_ptr<const float>(),
                    args[7]->get_data_ptr<const int32_t>());
            }
            else
            {
                std::stringstream ss;
                ss << "unsupported element type";
                throw std::runtime_error(ss.str());
            }

            break;
        }
        case OP_TYPEID::RandomUniform_v0:
        {
            const op::v0::RandomUniform* ru = static_cast<const op::v0::RandomUniform*>(&node);

            T min_val = args[0]->get_data_ptr<const T>()[0];
            T max_val = args[1]->get_data_ptr<const T>()[0];
            // In INTERPRETER we can ignore arg 2 (output_shape) for now because we only work on
            // static output shapes anyway.
            bool use_fixed_seed = static_cast<bool>(args[3]->get_data_ptr<const char>()[0]);

            if (m_states.count(&node) == 0)
            {
                m_states[&node] = std::unique_ptr<UniformRNGState>(new UniformRNGState());
            }

            auto state = static_cast<UniformRNGState*>(m_states.at(&node).get());
            size_t element_count = shape_size(node.get_output_shape(0));
            if (!use_fixed_seed)
            {
                reference::random_uniform<T>(
                    out[0]->get_data_ptr<T>(), min_val, max_val, element_count, state);
            }
            else
            {
                reference::random_uniform_with_fixed_seed<T>(out[0]->get_data_ptr<T>(),
                                                             min_val,
                                                             max_val,
                                                             element_count,
                                                             ru->get_fixed_seed());
            }
            break;
        }
        case OP_TYPEID::Recv_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            size_t memSize = element_count * sizeof(T);
            const auto* op = static_cast<const ngraph::op::v0::Recv*>(&node);
            int src_id = op->get_src_id();

            reference::recv<T>(
                args[0]->get_data_ptr<T>(), node.get_input_element_type(0), element_count, src_id);

            memcpy(out[0]->get_data_ptr<T>(), args[0]->get_data_ptr<T>(), memSize);
            break;
        }
        case OP_TYPEID::Range_v0:
        {
            const op::v0::Range* op = static_cast<const op::v0::Range*>(&node);
            reference::range<T>(args[0]->get_data_ptr<const T>(),
                                args[1]->get_data_ptr<const T>(),
                                out[0]->get_shape(),
                                out[0]->get_data_ptr<T>());
            break;
        }
        case OP_TYPEID::ReduceLogicalAnd_v1:
        {
            throw runtime_error("INTERPRETER ReduceLogicalAnd_v1 Not implemented");
        }
        case OP_TYPEID::ReduceLogicalOr_v1:
        {
            throw runtime_error("INTERPRETER ReduceLogicalOr_v1 Not implemented");
        }
        case OP_TYPEID::ReduceMax_v1:
        {
            const op::v1::ReduceMax* op = static_cast<const op::v1::ReduceMax*>(&node);
            AxisSet reduction_axes = as_axis_set(args[1].get());
            Shape input_shape = args[0]->get_shape();
            Shape output_shape = op->compute_output_shape(input_shape, reduction_axes);
            out[0]->set_shape(output_shape);
            reference::max<T>(args[0]->get_data_ptr<const T>(),
                              out[0]->get_data_ptr<T>(),
                              input_shape,
                              reduction_axes);
            break;
        }
        case OP_TYPEID::ReduceMean_v1:
        {
            const op::v1::ReduceMean* op = static_cast<const op::v1::ReduceMean*>(&node);
            AxisSet reduction_axes = as_axis_set(args[1].get());
            Shape input_shape = args[0]->get_shape();
            Shape output_shape = op->compute_output_shape(input_shape, reduction_axes);
            out[0]->set_shape(output_shape);
            reference::mean<T>(args[0]->get_data_ptr<const T>(),
                               out[0]->get_data_ptr<T>(),
                               input_shape,
                               reduction_axes);
            break;
        }
        case OP_TYPEID::ReduceMin_v1:
        {
            const op::v1::ReduceMin* op = static_cast<const op::v1::ReduceMin*>(&node);
            AxisSet reduction_axes = as_axis_set(args[1].get());
            Shape input_shape = args[0]->get_shape();
            Shape output_shape = op->compute_output_shape(input_shape, reduction_axes);
            out[0]->set_shape(output_shape);
            reference::min<T>(args[0]->get_data_ptr<const T>(),
                              out[0]->get_data_ptr<T>(),
                              input_shape,
                              reduction_axes);
            break;
        }
        case OP_TYPEID::ReduceProd_v1:
        {
            const op::v1::ReduceProd* op = static_cast<const op::v1::ReduceProd*>(&node);
            AxisSet reduction_axes = as_axis_set(args[1].get());
            Shape input_shape = args[0]->get_shape();
            Shape output_shape = op->compute_output_shape(input_shape, reduction_axes);
            out[0]->set_shape(output_shape);
            reference::product<T>(args[0]->get_data_ptr<const T>(),
                                  out[0]->get_data_ptr<T>(),
                                  input_shape,
                                  reduction_axes);
            break;
        }
        case OP_TYPEID::ReduceSum_v1:
        {
            const op::v1::ReduceSum* op = static_cast<const op::v1::ReduceSum*>(&node);
            AxisSet reduction_axes = as_axis_set(args[1].get());
            Shape input_shape = args[0]->get_shape();
            Shape output_shape = op->compute_output_shape(input_shape, reduction_axes);
            out[0]->set_shape(output_shape);
            reference::sum<T>(args[0]->get_data_ptr<const T>(),
                              out[0]->get_data_ptr<T>(),
                              input_shape,
                              reduction_axes);
            break;
        }
        case OP_TYPEID::Relu_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::relu<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::ReluBackprop_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::relu_backprop<T>(args[0]->get_data_ptr<const T>(),
                                        args[1]->get_data_ptr<const T>(),
                                        out[0]->get_data_ptr<T>(),
                                        element_count);
            break;
        }
        case OP_TYPEID::ReplaceSlice_v0:
        {
            const op::v0::ReplaceSlice* slice = static_cast<const op::v0::ReplaceSlice*>(&node);
            reference::replace_slice<T>(args[0]->get_data_ptr<const T>(),
                                        args[1]->get_data_ptr<const T>(),
                                        out[0]->get_data_ptr<T>(),
                                        args[1]->get_shape(),
                                        slice->get_lower_bounds(),
                                        slice->get_upper_bounds(),
                                        slice->get_strides(),
                                        node.get_output_shape(0));
            break;
        }
        case OP_TYPEID::Reshape_v0:
        {
            const op::v0::Reshape* reshape = static_cast<const op::v0::Reshape*>(&node);
            reference::reshape(args[0]->get_data_ptr<const T>(),
                               out[0]->get_data_ptr<T>(),
                               args[0]->get_shape(),
                               reshape->get_input_order(),
                               node.get_output_shape(0));
            break;
        }
        case OP_TYPEID::Reshape_v1:
        {
            const op::v1::Reshape* reshape = static_cast<const op::v1::Reshape*>(&node);
            auto input_shape = args[0]->get_shape();
            vector<int64_t> pattern = as_vector<int64_t>(args[1].get());
            bool special_zero = reshape->get_special_zero();

            for (size_t i = 0; i < pattern.size(); i++)
            {
                if (pattern[i] < 0)
                {
                    // Infer this dim from shape size of input
                    size_t pattern_size = 1;
                    for (auto v : pattern)
                    {
                        pattern_size *= (v < 0 ? 1 : v);
                    }
                    pattern[i] = shape_size(input_shape) / pattern_size;
                }
                else if (special_zero && pattern[i] == 0)
                {
                    pattern[i] = input_shape[i];
                }
            }
            Shape output_shape(pattern.begin(), pattern.end());
            AxisVector input_order = get_default_order(input_shape.size());
            reference::reshape(args[0]->get_data_ptr<const T>(),
                               out[0]->get_data_ptr<T>(),
                               input_shape,
                               input_order,
                               output_shape);
            break;
        }
        case OP_TYPEID::Result_v0:
        {
            const op::v0::Result* res = static_cast<const op::v0::Result*>(&node);
            out[0]->set_shape(args[0]->get_shape());
            reference::result(args[0]->get_data_ptr<const T>(),
                              out[0]->get_data_ptr<T>(),
                              shape_size(out[0]->get_shape()));
            break;
        }
        case OP_TYPEID::Reverse_v0:
        {
            const op::v0::Reverse* reverse = static_cast<const op::v0::Reverse*>(&node);
            reference::reverse(args[0]->get_data_ptr<const T>(),
                               out[0]->get_data_ptr<T>(),
                               args[0]->get_shape(),
                               node.get_output_shape(0),
                               reverse->get_reversed_axes());
            break;
        }
        case OP_TYPEID::ReverseSequence_v0:
        {
            const op::v0::ReverseSequence* reverse =
                static_cast<const op::v0::ReverseSequence*>(&node);

            if (node.get_input_element_type(1) == element::i32)
            {
                reference::reverse_sequence<T, int32_t>(args[0]->get_data_ptr<const T>(),
                                                        out[0]->get_data_ptr<T>(),
                                                        args[0]->get_shape(),
                                                        reverse->get_batch_axis(),
                                                        reverse->get_sequence_axis(),
                                                        args[1]->get_data_ptr<const int32_t>());
            }
            else
            {
                throw ngraph_error("only int32 indices are supported");
            }
            break;
        }
        case OP_TYPEID::Round_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::round<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::ScatterAdd_v0:
        {
            if (node.get_input_element_type(1) == element::i64)
            {
                reference::scatter_add<T, int64_t>(args[0]->get_data_ptr<T>(),
                                                   args[1]->get_data_ptr<int64_t>(),
                                                   args[2]->get_data_ptr<T>(),
                                                   out[0]->get_data_ptr<T>(),
                                                   args[0]->get_shape(),
                                                   args[1]->get_shape(),
                                                   node.get_input_shape(2),
                                                   node.get_output_shape(0));
            }
            else if (node.get_input_element_type(1) == element::i32)
            {
                reference::scatter_add<T, int32_t>(args[0]->get_data_ptr<T>(),
                                                   args[1]->get_data_ptr<int32_t>(),
                                                   args[2]->get_data_ptr<T>(),
                                                   out[0]->get_data_ptr<T>(),
                                                   args[0]->get_shape(),
                                                   args[1]->get_shape(),
                                                   node.get_input_shape(2),
                                                   node.get_output_shape(0));
            }
            else
            {
                throw ngraph_error("Unexpected type");
            }
            break;
        }
        case OP_TYPEID::ScatterNDAdd_v0:
        {
            if (node.get_input_element_type(1) == element::i64)
            {
                reference::scatter_nd_add<T, int64_t>(args[0]->get_data_ptr<T>(),
                                                      args[1]->get_data_ptr<int64_t>(),
                                                      args[2]->get_data_ptr<T>(),
                                                      out[0]->get_data_ptr<T>(),
                                                      args[0]->get_shape(),
                                                      args[1]->get_shape(),
                                                      node.get_input_shape(2),
                                                      node.get_output_shape(0));
            }
            else if (node.get_input_element_type(1) == element::i32)
            {
                reference::scatter_nd_add<T, int32_t>(args[0]->get_data_ptr<T>(),
                                                      args[1]->get_data_ptr<int32_t>(),
                                                      args[2]->get_data_ptr<T>(),
                                                      out[0]->get_data_ptr<T>(),
                                                      args[0]->get_shape(),
                                                      args[1]->get_shape(),
                                                      node.get_input_shape(2),
                                                      node.get_output_shape(0));
            }
            else
            {
                throw ngraph_error("Unexpected type");
            }
            break;
        }
        case OP_TYPEID::Select_v0:
        {
            size_t element_count = shape_size(node.get_output_shape(0));
            reference::select<T>(args[0]->get_data_ptr<const char>(),
                                 args[1]->get_data_ptr<const T>(),
                                 args[2]->get_data_ptr<const T>(),
                                 out[0]->get_data_ptr<T>(),
                                 element_count);
            break;
        }
        case OP_TYPEID::Send_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            size_t memSize = element_count * sizeof(T);
            const auto* op = static_cast<const ngraph::op::v0::Send*>(&node);
            int dest_id = op->get_dest_id();

            reference::send<T>(args[0]->get_data_ptr<const T>(),
                               node.get_input_element_type(0),
                               element_count,
                               dest_id);

            memcpy(out[0]->get_data_ptr<T>(), args[0]->get_data_ptr<T>(), memSize);
            break;
        }
        case OP_TYPEID::ShapeOf_v0:
        {
            reference::shape_of(args[0]->get_shape(), out[0]->get_data_ptr<uint64_t>());
            break;
        }
        case OP_TYPEID::ShapeOf_v3:
        {
            reference::shape_of(args[0]->get_shape(), out[0]->get_data_ptr<uint64_t>());
            break;
        }
        case OP_TYPEID::Sigmoid_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::sigmoid<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::SigmoidBackprop_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::sigmoid_backprop<T>(args[0]->get_data_ptr<const T>(),
                                           args[1]->get_data_ptr<const T>(),
                                           out[0]->get_data_ptr<T>(),
                                           element_count);
            break;
        }
        case OP_TYPEID::Sign_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::sign<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Sin_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::sin<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Sinh_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::sinh<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Slice_v0:
        {
            const op::v0::Slice* slice = static_cast<const op::v0::Slice*>(&node);
            reference::slice<T>(args[0]->get_data_ptr<const T>(),
                                out[0]->get_data_ptr<T>(),
                                args[0]->get_shape(),
                                slice->get_lower_bounds(),
                                slice->get_upper_bounds(),
                                slice->get_strides(),
                                node.get_output_shape(0));
            break;
        }
        case OP_TYPEID::Softmax_v0:
        {
            const op::v0::Softmax* softmax = static_cast<const op::v0::Softmax*>(&node);
            reference::softmax<T>(args[0]->get_data_ptr<const T>(),
                                  out[0]->get_data_ptr<T>(),
                                  node.get_output_shape(0),
                                  softmax->get_axes());
            break;
        }
        case OP_TYPEID::Sqrt_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::sqrt<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::StopGradient_v0: { throw unsupported_op("Unsupported op 'StopGradient_v0'");
        }
        case OP_TYPEID::StridedSlice_v1:
        {
            const op::v1::StridedSlice* slice = static_cast<const op::v1::StridedSlice*>(&node);
            Shape input_shape = args[0]->get_shape();
            vector<int64_t> t_begin = as_vector<int64_t>(args[1].get());
            vector<int64_t> t_end = as_vector<int64_t>(args[2].get());
            vector<int64_t> t_strides = as_vector<int64_t>(args[3].get());
            NGRAPH_INFO << args.size();
            SlicePlan slice_plan =
                make_slice_plan(input_shape,
                                t_begin,
                                t_end,
                                t_strides,
                                slice->convert_mask_to_axis_set(slice->get_begin_mask()),
                                slice->convert_mask_to_axis_set(slice->get_end_mask()),
                                slice->convert_mask_to_axis_set(slice->get_new_axis_mask()),
                                slice->convert_mask_to_axis_set(slice->get_shrink_axis_mask()),
                                slice->convert_mask_to_axis_set(slice->get_ellipsis_mask()));
            Shape output_shape = slice_plan.reshape_out_shape;
            NGRAPH_INFO << output_shape;

            NGRAPH_INFO << out[0]->get_partial_shape();
            out[0]->set_shape(output_shape);
            reference::strided_slice<T>(args[0]->get_data_ptr<const T>(),
                                        out[0]->get_data_ptr<T>(),
                                        input_shape,
                                        slice_plan);
            break;
        }
        case OP_TYPEID::Subtract_v1:
        {
            auto subtract = static_cast<const op::v1::Subtract*>(&node);
            Shape output_shape =
                subtract->compute_output_shape(args[0]->get_shape(), args[1]->get_shape());
            out[0]->set_shape(output_shape);
            reference::subtract<T>(args[0]->get_data_ptr<const T>(),
                                   args[1]->get_data_ptr<const T>(),
                                   out[0]->get_data_ptr<T>(),
                                   args[0]->get_shape(),
                                   args[1]->get_shape(),
                                   subtract->get_autob());
            break;
        }
        case OP_TYPEID::Sum_v0:
        {
            const op::v0::Sum* sum = static_cast<const op::v0::Sum*>(&node);
            AxisSet reduction_axes = as_axis_set(args[1].get());
            out[0]->set_shape(reduce(args[0]->get_shape(), reduction_axes));
            reference::sum<T>(args[0]->get_data_ptr<const T>(),
                              out[0]->get_data_ptr<T>(),
                              args[0]->get_shape(),
                              reduction_axes);
            break;
        }
        case OP_TYPEID::Tan_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::tan<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::Tanh_v0:
        {
            Shape output_shape = args[0]->get_shape();
            size_t element_count = shape_size(output_shape);
            out[0]->set_shape(output_shape);
            reference::tanh<T>(
                args[0]->get_data_ptr<const T>(), out[0]->get_data_ptr<T>(), element_count);
            break;
        }
        case OP_TYPEID::TopK_v0:
        {
            const op::v0::TopK* topk = static_cast<const op::v0::TopK*>(&node);
            if (node.get_output_element_type(0) == element::i64)
            {
                reference::topk<T, int64_t>(args[0]->get_data_ptr<const T>(),
                                            out[0]->get_data_ptr<int64_t>(),
                                            out[1]->get_data_ptr<T>(),
                                            args[0]->get_shape(),
                                            node.get_output_shape(0),
                                            topk->get_top_k_axis(),
                                            topk->get_k(),
                                            topk->get_compute_max(),
                                            topk->get_sort());
            }
            else if (node.get_output_element_type(0) == element::i32)
            {
                reference::topk<T, int32_t>(args[0]->get_data_ptr<const T>(),
                                            out[0]->get_data_ptr<int32_t>(),
                                            out[1]->get_data_ptr<T>(),
                                            args[0]->get_shape(),
                                            node.get_output_shape(0),
                                            topk->get_top_k_axis(),
                                            topk->get_k(),
                                            topk->get_compute_max(),
                                            topk->get_sort());
            }
            else
            {
                throw ngraph_error("Unexpected type");
            }
            break;
        }
        case OP_TYPEID::Transpose_v1:
        {
            const op::v1::Transpose* op = static_cast<const op::v1::Transpose*>(&node);
            Shape input_shape = args[0]->get_shape();
            AxisVector input_order = as_axis_vector(args[1].get());
            Shape output_shape;
            for (size_t axis : input_order)
            {
                output_shape.push_back(input_shape[axis]);
            }
            reference::reshape(args[0]->get_data_ptr<const T>(),
                               out[0]->get_data_ptr<T>(),
                               input_shape,
                               input_order,
                               output_shape);
            break;
        }

        case OP_TYPEID::AvgPool_v1:
        case OP_TYPEID::BatchMatMulTranspose_v0:
        case OP_TYPEID::BatchToSpace_v1:
        case OP_TYPEID::BinaryConvolution_v1:
        case OP_TYPEID::Broadcast_v1:
        case OP_TYPEID::Broadcast_v3:
        case OP_TYPEID::Bucketize_v3:
        case OP_TYPEID::ConvertLike_v1:
        case OP_TYPEID::Convolution_v1:
        case OP_TYPEID::ConvolutionBackpropData_v1:
        case OP_TYPEID::ConvolutionBias_v0:
        case OP_TYPEID::ConvolutionBiasAdd_v0:
        case OP_TYPEID::ConvolutionBiasBackpropFiltersBias_v0:
        case OP_TYPEID::CrossEntropy_v0:
        case OP_TYPEID::CrossEntropyBackprop_v0:
        case OP_TYPEID::CTCGreedyDecoder_v0:
        case OP_TYPEID::DeformableConvolution_v1:
        case OP_TYPEID::DeformablePSROIPooling_v1:
        case OP_TYPEID::DepthToSpace_v0:
        case OP_TYPEID::DetectionOutput_v0:
        case OP_TYPEID::DynBroadcast_v0:
        case OP_TYPEID::DynPad_v0:
        case OP_TYPEID::DynReplaceSlice_v0:
        case OP_TYPEID::Elu_v0:
        case OP_TYPEID::EmbeddingBagOffsetsSum_v3:
        case OP_TYPEID::EmbeddingBagPackedSum_v3:
        case OP_TYPEID::EmbeddingSegmentsSum_v3:
        case OP_TYPEID::ExtractImagePatches_v3:
        case OP_TYPEID::FakeQuantize_v0:
        case OP_TYPEID::FloorMod_v1:
        case OP_TYPEID::Gather_v1:
        case OP_TYPEID::GatherTree_v1:
        case OP_TYPEID::Gelu_v0:
        case OP_TYPEID::GeluBackpropFactor_v0:
        case OP_TYPEID::Gemm_v0:
        case OP_TYPEID::GRN_v0:
        case OP_TYPEID::GroupConvolution_v0:
        case OP_TYPEID::GroupConvolution_v1:
        case OP_TYPEID::GroupConvolutionBackpropData_v0:
        case OP_TYPEID::GroupConvolutionBackpropData_v1:
        case OP_TYPEID::GroupConvolutionBackpropFilters_v0:
        case OP_TYPEID::GRUCell_v3:
        case OP_TYPEID::HardSigmoid_v0:
        case OP_TYPEID::Interpolate_v0:
        case OP_TYPEID::Interpolate_v3:
        case OP_TYPEID::LayerNorm_v0:
        case OP_TYPEID::LayerNormBackprop_v0:
        case OP_TYPEID::LSTMCell_v0:
        case OP_TYPEID::LSTMSequence_v0:
        case OP_TYPEID::MaxPool_v1:
        case OP_TYPEID::Mod_v1:
        case OP_TYPEID::MVN_v0:
        case OP_TYPEID::NonMaxSuppression_v1:
        case OP_TYPEID::NonMaxSuppression_v3:
        case OP_TYPEID::NonZero_v3:
        case OP_TYPEID::NormalizeL2_v0:
        case OP_TYPEID::OneHot_v1:
        case OP_TYPEID::Pad_v1:
        case OP_TYPEID::PartialSlice_v0:
        case OP_TYPEID::PartialSliceBackprop_v0:
        case OP_TYPEID::PRelu_v0:
        case OP_TYPEID::PriorBox_v0:
        case OP_TYPEID::PriorBoxClustered_v0:
        case OP_TYPEID::Proposal_v0:
        case OP_TYPEID::PSROIPooling_v0:
        case OP_TYPEID::RegionYolo_v0:
        case OP_TYPEID::ReorgYolo_v0:
        case OP_TYPEID::Reverse_v1:
        case OP_TYPEID::RNNCell_v0:
        case OP_TYPEID::ROIAlign_v3:
        case OP_TYPEID::ROIPooling_v0:
        case OP_TYPEID::ScalarConstantLike_v0:
        case OP_TYPEID::ScaleShift_v0:
        case OP_TYPEID::ScatterElementsUpdate_v3:
        case OP_TYPEID::ScatterND_v0:
        case OP_TYPEID::ScatterUpdate_v3:
        case OP_TYPEID::Select_v1:
        case OP_TYPEID::Selu_v0:
        case OP_TYPEID::ShuffleChannels_v0:
        case OP_TYPEID::Softmax_v1:
        case OP_TYPEID::SoftmaxCrossEntropy_v0:
        case OP_TYPEID::SoftmaxCrossEntropyBackprop_v0:
        case OP_TYPEID::SpaceToBatch_v1:
        case OP_TYPEID::SpaceToDepth_v0:
        case OP_TYPEID::Split_v0:
        case OP_TYPEID::Split_v1:
        case OP_TYPEID::SquaredDifference_v0:
        case OP_TYPEID::Squeeze_v0:
        case OP_TYPEID::Stack_v0:
        case OP_TYPEID::TensorIterator_v0:
        case OP_TYPEID::Tile_v0:
        case OP_TYPEID::TopK_v1:
        case OP_TYPEID::TopK_v3:
        case OP_TYPEID::Unsqueeze_v0:
        case OP_TYPEID::VariadicSplit_v1:
        case OP_TYPEID::UnknownOp:
        {
            stringstream ss;
            ss << "v" << node.get_type_info().version << "::" << node.get_type_info().name;
            throw unsupported_op("Unsupported op '" + ss.str() + "'");
        }
#if defined(__GNUC__) && !(__GNUC__ == 4 && __GNUC_MINOR__ == 8)
#pragma GCC diagnostic pop
#endif
        }
    }
};
