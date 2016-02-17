/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __CoAP_h
#define __CoAP_h

#include "Client.h"
#include "RequestHandler.h"
#include "RequestHandlerDispatcher.h"
#include "IMessaging.h"

namespace CoAP {

/// Creates a new messaging object
std::unique_ptr<IMessaging> newMessaging(uint16_t port = 5683);

}  // namespace CoAP

#endif // __CoAP_h