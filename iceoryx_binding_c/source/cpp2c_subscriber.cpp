// Copyright (c) 2020 by Robert Bosch GmbH, Apex.AI Inc. All rights reserved.
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

#include "iceoryx_binding_c/internal/cpp2c_subscriber.hpp"
#include "iceoryx_binding_c/enums.h"
#include "iceoryx_binding_c/internal/cpp2c_enum_translation.hpp"
#include "iceoryx_posh/internal/popo/ports/subscriber_port_user.hpp"

cpp2c_Subscriber::~cpp2c_Subscriber()
{
    if (m_portData)
    {
        iox::popo::SubscriberPortUser(m_portData).destroy();
    }
}

iox_WaitSetResult cpp2c_Subscriber::attachTo(iox::popo::WaitSet& waitset,
                                             const iox_SubscriberEvent subscriberEvent,
                                             const uint64_t triggerId,
                                             const iox::popo::Trigger::Callback<cpp2c_Subscriber> callback) noexcept
{
    static_cast<void>(subscriberEvent);

    auto result = std::move(
        waitset
            .acquireTrigger(this,
                            {*this, &cpp2c_Subscriber::hasNewSamples},
                            {*this, &cpp2c_Subscriber::invalidateTrigger},
                            triggerId,
                            callback)
            .and_then([this](iox::popo::TriggerHandle& trigger) {
                m_trigger = std::move(trigger);
                iox::popo::SubscriberPortUser(m_portData).setConditionVariable(m_trigger.getConditionVariableData());
            }));

    return (result.has_error()) ? cpp2c::WaitSetResult(result.get_error()) : iox_WaitSetResult::WaitSetResult_SUCCESS;
}

void cpp2c_Subscriber::detachEvent(const iox_SubscriberEvent subscriberEvent) noexcept
{
    static_cast<void>(subscriberEvent);

    m_trigger.reset();
}

void cpp2c_Subscriber::invalidateTrigger(const uint64_t uniqueTriggerId) noexcept
{
    if (m_trigger.getUniqueId() == uniqueTriggerId)
    {
        iox::popo::SubscriberPortUser(m_portData).unsetConditionVariable();
        m_trigger.reset();
    }
}

bool cpp2c_Subscriber::hasNewSamples() const noexcept
{
    return iox::popo::SubscriberPortUser(m_portData).hasNewChunks();
}

