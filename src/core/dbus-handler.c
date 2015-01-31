/*
 * device options
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/


#include <stdio.h>
#include "device-options.h"
#include "dbus-handler.h"

#define RETRY_MAX 10
#define DBUS_REPLY_TIMEOUT  (-1)

static E_DBus_Connection *edbus_conn = NULL;

int set_dbus_connection(void)
{
	int retry;

	if (edbus_conn)
		return 0;

	retry = 0;
	while (e_dbus_init() == 0) {
		if (retry++ >= RETRY_MAX)
			return -ENOMEM;
	}

	edbus_conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
	if (!edbus_conn) {
		_E("Failed to get dbus bus");
		e_dbus_shutdown();
		return -ENOMEM;
	}

	return 0;
}

E_DBus_Connection *get_dbus_connection(void)
{
	return edbus_conn;
}

void unset_dbus_connection(void)
{
	if (edbus_conn) {
		e_dbus_connection_close(edbus_conn);
		e_dbus_shutdown();
		edbus_conn = NULL;
	}
}
