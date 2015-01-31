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

#include <string.h>
#include <stdlib.h>
#include "wifi.h"

#define WIFI_TEXT_CN "IDS_ST_BODY_WLAN_CHN"
#define COUNTRY_CN   "CN"

static __attribute__ ((constructor)) void register_wifi_cn(void)
{
	char *country;

	country = vconf_get_str(VCONFKEY_CSC_TEXT_STRING_COUNTRY);
	if (!country)
		return;

	if (!strncmp(country, COUNTRY_CN, strlen(country)))
		wifi_register_text(WIFI_TEXT_CN);

	free(country);
}
