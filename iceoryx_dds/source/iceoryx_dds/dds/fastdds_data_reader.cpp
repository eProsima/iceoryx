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

#include "iceoryx_dds/dds/fastdds_data_reader.hpp"
#include "iceoryx_dds/dds/fastdds_context.hpp"
#include "iceoryx_dds/internal/log/logging.hpp"
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <MempoolPubSubTypes.h>


iox::dds::FastDDSDataReader::FastDDSDataReader(IdString serviceId, IdString instanceId, IdString eventId) noexcept
    : m_serviceId(serviceId)
    , m_instanceId(instanceId)
    , m_eventId(eventId)
    , type_(new MempoolPubSubType())
{
    LogDebug() << "[FastDDSDataReader] Created FastDDSDataReader.";
}

iox::dds::FastDDSDataReader::~FastDDSDataReader()
{
    auto participant = FastDDSContext::getParticipant();
    if (fastdds_data_reader_ != nullptr)
    {
        subscriber_->delete_datareader(fastdds_data_reader_);
    }
    if (topic_ != nullptr)
    {
        participant->delete_topic(topic_);
    }
    if (fastdds_data_subscriber_ != nullptr)
    {
        participant->delete_subscriber(fastdds_data_subscriber_);
    }
    LogDebug() << "[FastDDSDataReader] Destroyed FastDDSDataReader.";
}

void iox::dds::FastDDSDataReader::connect() noexcept
{
    if (!m_isConnected.load(std::memory_order_relaxed))
    {
        // Get the Fast DDS DomainParticipant
        auto participant = FastDDSContext::getParticipant();

        // Register the type
        type_.register_type(participant);

        // Create topic
        auto topicString =
            "/" + std::string(m_serviceId) + "/" + std::string(m_instanceId) + "/" + std::string(m_eventId);
        topic_ = participant->create_topic(topicString, "Mempool::Chunk", TOPIC_QOS_DEFAULT);

        // Create subscriber
        fastdds_data_subscriber_ =
            participant->create_subscriber(eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT, nullptr);

        // Create datareader
        eprosima::fastdds::dds::DataReaderQos dr_qos = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;
        dr_qos.history().kind = eprosima::fastdds::dds::KEEP_ALL_HISTORY_QOS;
        fastdds_data_reader_ = fastdds_data_subscriber_->create_datareader(topic_, dr_qos);

        // Save state
        m_isConnected.store(true, std::memory_order_relaxed);
        LogDebug() << "[FastDDSDataReader] Connected to topic: " << topicString;
    }
}

iox::cxx::optional<uint64_t> iox::dds::FastDDSDataReader::peekNextSize()
{
    // Get information about the next unread sample
    Mempool::Chunk data {0, nullptr};
    eprosima::fastdds::dds::SampleInfo next_info;
    if (eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK ==
        fastdds_data_reader_->read_next_sample(&data ,&next_info))
    {
       if (data.payload_size() != 0)
       {
           return iox::cxx::optional<uint64_t>(static_cast<uint64_t>(data.payload_size()));
       }
    }

    // no valid samples available
    return iox::cxx::nullopt_t();
}

iox::cxx::expected<iox::dds::DataReaderError> iox::dds::FastDDSDataReader::takeNext(uint8_t* const buffer,
                                                                                    const uint64_t& bufferSize)
{
    // validation checks
    if (!m_isConnected.load())
    {
        return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::NOT_CONNECTED);
    }
    if (buffer == nullptr)
    {
        return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::INVALID_RECV_BUFFER);
    }

    // take next sample and copy into buffer
    Mempool::Chunk data {0, nullptr};
    eprosima::fastdds::dds::SampleInfo next_info;
    eprosima::fastrtps::types::ReturnCode_t ret = fastdds_data_reader_->take_next_sample(&data, &next_info);
    if (ret == eprosima::fastrtps::types::ReturnCode_t::RETCODE_NO_DATA)
    {
        // no samples available
        return iox::cxx::success<>();
    }
    else if (ret != eprosima::fastrtps::types::ReturnCode_t::RETCODE_OK)
    {
        LogError() << "[FastDDSDataReader] take_next_sample returned and error";
        return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::INVALID_DATA);
    }

    // valid size
    auto sample_size = data.payload_size();
    if (sample_size == 0)
    {
        return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::INVALID_DATA);
    }
    if (bufferSize < sample_size)
    {
        // provided buffer is too small.
        return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::RECV_BUFFER_TOO_SMALL);
    }

    // copy data into the provided buffer
    auto bytes = *data.payload();
    std::copy(bytes, bytes + sample_size, buffer);

    return iox::cxx::success<>();
}

iox::cxx::expected<uint64_t, iox::dds::DataReaderError> iox::dds::FastDDSDataReader::take(
    uint8_t* const buffer, const uint64_t& bufferSize, const iox::cxx::optional<uint64_t>& maxSamples)
{
    if (!m_isConnected.load())
    {
        return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::NOT_CONNECTED);
    }
    if (buffer == nullptr)
    {
        return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::INVALID_RECV_BUFFER);
    }

    // get size of the sample
    auto peekResult = peekNextSize();
    if (peekResult.has_value())
    {
        uint64_t sampleSize = peekResult.value();
        if (sampleSize == 0)
        {
            return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::INVALID_DATA);
        }
        if (bufferSize < sampleSize)
        {
            // Provided buffer is too small.
            return iox::cxx::error<iox::dds::DataReaderError>(iox::dds::DataReaderError::RECV_BUFFER_TOO_SMALL);
        }

        // take maximum amount possible up to the cap: maxSamples
        auto bufferCapacity = bufferSize / sampleSize;

        auto numToTake = bufferCapacity;
        if (maxSamples.has_value())
        {
            if (bufferCapacity > maxSamples.value())
            {
                numToTake = maxSamples.value();
            }
        }

        uint64_t cursor = 0;
        for (auto i = 0; i < numToTake; i++)
        {
            if (!takeNext(&buffer[cursor], sampleSize))
            {
                break;
            }
            cursor += sampleSize;
        }

        uint64_t numSamplesBuffered = cursor / sampleSize;
        return iox::cxx::success<uint64_t>(numSamplesBuffered);
    }
    else
    {
        return iox::cxx::success<uint64_t>(0u);
    }
}

iox::dds::IdString iox::dds::FastDDSDataReader::getServiceId() const noexcept
{
    return m_serviceId;
}

iox::dds::IdString iox::dds::FastDDSDataReader::getInstanceId() const noexcept
{
    return m_instanceId;
}

iox::dds::IdString iox::dds::FastDDSDataReader::getEventId() const noexcept
{
    return m_eventId;
}
