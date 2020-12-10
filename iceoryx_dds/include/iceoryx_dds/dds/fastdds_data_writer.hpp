// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
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

#ifndef IOX_DDS_DDS_FASTDDS_DATA_WRITER_HPP
#define IOX_DDS_DDS_FASTDDS_DATA_WRITER_HPP

#include "iceoryx_dds/dds/data_writer.hpp"

#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>

namespace iox
{
namespace dds
{
///
/// @brief Implementation of the DataWriter abstraction using the Fast DDS implementation.
///
class FastDDSDataWriter : public iox::dds::DataWriter
{
  public:
    FastDDSDataWriter() = delete;
    FastDDSDataWriter(const IdString serviceId, const IdString instanceId, const IdString eventId);
    virtual ~FastDDSDataWriter();
    FastDDSDataWriter(const FastDDSDataWriter&) = delete;
    FastDDSDataWriter& operator=(const FastDDSDataWriter&) = delete;
    // Required for vector
    FastDDSDataWriter(FastDDSDataWriter&&) = default;
    FastDDSDataWriter& operator=(FastDDSDataWriter&&) = default;

    void connect() noexcept override;
    void write(const uint8_t* const bytes, const uint64_t size) noexcept override;
    IdString getServiceId() const noexcept override;
    IdString getInstanceId() const noexcept override;
    IdString getEventId() const noexcept override;

  private:
    IdString m_serviceId{""};
    IdString m_instanceId{""};
    IdString m_eventId{""};
    std::atomic_bool m_isConnected{false};

    eprosima::fastdds::dds::TypeSupport type_;
    eprosima::fastdds::dds::Topic* topic_;
    eprosima::fastdds::dds::Publisher* fastdds_data_publisher_ = nullptr;
    eprosima::fastdds::dds::DataWriter* fastdds_data_writer_ = nullptr;
};

} // namespace dds
} // namespace iox

#endif // IOX_DDS_DDS_FASTDDS_DATA_WRITER_HPP
