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

#ifndef IOX_DDS_DDS_FASTDDS_DATA_READER_HPP
#define IOX_DDS_DDS_FASTDDS_DATA_READER_HPP

#include "iceoryx_dds/dds/data_reader.hpp"

#include <atomic>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>

namespace iox
{
namespace dds
{
///
/// @brief Implementation of the DataReader abstraction using the Fast DDS implementation.
///
class FastDDSDataReader : public DataReader
{
  public:
    FastDDSDataReader() = delete;
    FastDDSDataReader(IdString serviceId, IdString instanceId, IdString eventId) noexcept;
    virtual ~FastDDSDataReader();

    FastDDSDataReader(const FastDDSDataReader&) = delete;
    FastDDSDataReader& operator=(const FastDDSDataReader&) = delete;
    FastDDSDataReader(FastDDSDataReader&&) = delete;
    FastDDSDataReader& operator=(FastDDSDataReader&&) = delete;

    void connect() noexcept override;

    iox::cxx::optional<uint64_t> peekNextSize() override;

    iox::cxx::expected<DataReaderError> takeNext(uint8_t* const buffer, const uint64_t& bufferSize) override;

    iox::cxx::expected<uint64_t, DataReaderError>
    take(uint8_t* const buffer, const uint64_t& bufferSize, const iox::cxx::optional<uint64_t>& maxSamples) override;

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
    eprosima::fastdds::dds::Subscriber* fastdds_data_subscriber_ = nullptr;
    eprosima::fastdds::dds::DataReader* fastdds_data_reader_ = nullptr;
};

} // namespace dds
} // namespace iox

#endif // IOX_DDS_DDS_FASTDDS_DATA_READER_HPP
