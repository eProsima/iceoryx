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

#include "iceoryx_dds/dds/fastdds_data_writer.hpp"
#include "iceoryx_dds/dds/fastdds_context.hpp"
#include "iceoryx_dds/internal/log/logging.hpp"

#include <MempoolPubSubTypes.h>
#include <string>

iox::dds::FastDDSDataWriter::FastDDSDataWriter(IdString serviceId, IdString instanceId, IdString eventId)
    : m_serviceId(serviceId)
    , m_instanceId(instanceId)
    , m_eventId(eventId)
    , type_(new MempoolPubSubType())
{
    LogDebug() << "[FastDDSDataWriter] Created FastDDSDataWriter.";
}

iox::dds::FastDDSDataWriter::~FastDDSDataWriter()
{
    auto participant = FastDDSContext::getParticipant();
    if (fastdds_data_writer_ != nullptr)
    {
        subscriber_->delete_datawriter(fastdds_data_writer_);
    }
    if (topic_ != nullptr)
    {
        participant->delete_topic(topic_);
    }
    if (fastdds_data_publisher_ != nullptr)
    {
        participant->delete_publisher(fastdds_data_publisher_);
    }
    LogDebug() << "[FastDDSDataWriter] Destroyed FastDDSDataWriter.";
}

void iox::dds::FastDDSDataWriter::connect() noexcept
{
    if (!m_isConnected.load(std::memory_order_relaxed))
    {
        // Get the Fast DDS DomainParticipant
        auto participant = FastDDSContext::getParticipant();

        // Register the type
        type_.register_type(participant);

        // Create topic
        auto topic_name =
            "/" + std::string(m_serviceId) + "/" + std::string(m_instanceId) + "/" + std::string(m_eventId);
        topic_ = participant->create_topic(topic_name, "Mempool::Chunk", TOPIC_QOS_DEFAULT);

        // Create publisher
        fastdds_data_publisher_ =
            participant->create_publisher(eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT, nullptr);

        // Create datawriter
        fastdds_data_writer_ =
            fastdds_data_publisher_->create_datawriter(topic_, eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT);

        // Save state
        m_isConnected.store(true, std::memory_order_relaxed);
        LogDebug() << "[FastDDSDataWriter] Connected to topic: " << topic_name;
    }
    LogDebug() << "[FastDDSDataWriter] Connected to topic: " << topic;
}

void iox::dds::FastDDSDataWriter::write(const uint8_t* const bytes, const uint64_t size) noexcept
{
    try
    {
        Mempool::Chunk data {size, bytes};
        fastdds_data_writer_->write(data);
    }
    catch(const std::exception& e)
    {
        LogError() << "[FastDDSDataWriter] write exception: " << e.what();
    }
}

iox::dds::IdString iox::dds::FastDDSDataWriter::getServiceId() const noexcept
{
    return m_serviceId;
}

iox::dds::IdString iox::dds::FastDDSDataWriter::getInstanceId() const noexcept
{
    return m_instanceId;
}

iox::dds::IdString iox::dds::FastDDSDataWriter::getEventId() const noexcept
{
    return m_eventId;
}
