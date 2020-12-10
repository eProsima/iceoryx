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

#include "iceoryx_dds/dds/fastdds_context.hpp"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

eprosima::fastdds::dds::DomainParticipant* iox::dds::FastDDSContext::participant_ = nullptr;

eprosima::fastdds::dds::DomainParticipant* iox::dds::FastDDSContext::getParticipant() noexcept
{
    if (nullptr == participant_)
    {
        eprosima::fastdds::dds::DomainParticipantQos pqos;
        participant_ = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(0, pqos);
    }
    return participant_;
}
