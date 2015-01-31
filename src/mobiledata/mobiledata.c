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
#include <appsvc.h>
#include <syspopup_caller.h>
#include "device-options.h"
#include "show-ui.h"

#define MOBILEDATA_ID   1800
#define MOBILEDATA_NAME "mobiledata"
#define MOBILEDATA_ICON "micro-mobiledata-off.png"
#define MOBILEDATA_TEXT "IDS_ST_MBODY_MOBILE_DATA_ABB"

#define VCONFKEY_SAP_TYPE    "memory/private/sap/conn_type"
#define SAP_MOBILE           0x10
#define SAP_BT               0x01

#define SYSTEM_SYSPOPUP    "system-syspopup"
#define POPUP_KEY_TYPE     "_SYSPOPUP_CONTENT_"
#define MOBILEDATA_ENABLE  "mobiledata_enable"
#define MOBILEDATA_DISABLE "mobiledata_disable"

#define VCONFKEY_MOBILEDATA_ON_CHECK  VCONFKEY_SETAPPL_MOBILE_DATA_ON_REMINDER
#define VCONFKEY_MOBILEDATA_OFF_CHECK VCONFKEY_SETAPPL_MOBILE_DATA_OFF_REMINDER

enum current_state {
	MOBILEDATA_OFF,
	MOBILEDATA_ON,
	MOBILEDATA_BT,
	MOBILEDATA_NOSIM,
	MOBILEDATA_NOTSUP,
};

static const char *item_icon[] = {
	"micro-mobiledata-off.png",
	"micro-mobiledata-on.png",
	"micro-mobiledata-disabled.png",
};

static int get_current_state(void)
{
	int state, sim;

	if (vconf_get_bool(VCONFKEY_3G_ENABLE, &state) != 0)
		return MOBILEDATA_NOTSUP;

	if (vconf_get_int(VCONFKEY_TELEPHONY_SIM_SLOT, &sim) != 0)
		return MOBILEDATA_NOTSUP;

	if (sim != VCONFKEY_TELEPHONY_SIM_INSERTED)
		return MOBILEDATA_NOSIM;

	if (state == 1)
		return MOBILEDATA_ON;

	return MOBILEDATA_OFF;
}

static void mobiledata_changed(keynode_t *in_key, void *data)
{
	struct device_option *opt = data;
	update_item(opt);
}

static int launch_mobiledata_error(struct appdata *ad, int state)
{
	char *text;

	if (!ad)
		return -EINVAL;

	switch (state) {
	case MOBILEDATA_NOTSUP:
		text = "IDS_ST_POP_NOT_SUPPORTED";
		break;
	case MOBILEDATA_NOSIM:
		text = "IDS_ST_POP_INSERT_SIM_CARD_TO_ACCESS_NETWORK_SERVICES";
		break;
	case MOBILEDATA_BT:
		text = "IDS_ST_TPOP_UNABLE_TO_TURN_ON_MOBILE_DATA_WHILE_CONNECTED_VIA_BLUETOOTH";
		break;
	default:
		return -EINVAL;
	}

	return load_popup_toast(ad, _(text));
}

static bool get_check_status(int current)
{
	char *vconf_name;
	int val;

	switch (current) {
	case MOBILEDATA_OFF:
		vconf_name = VCONFKEY_MOBILEDATA_ON_CHECK;
		break;
	case MOBILEDATA_ON:
		vconf_name = VCONFKEY_MOBILEDATA_OFF_CHECK;
		break;
	default:
		return true;
	}

	if (vconf_get_bool(vconf_name, &val) == 0
			&& val == 1)
		return false;

	return true;
}

static int update_mobiledata(struct appdata *ad, int current)
{
	int state, ret;

	switch (current) {
	case MOBILEDATA_OFF:
		state = MOBILEDATA_ON;
		break;
	case MOBILEDATA_ON:
		state = MOBILEDATA_OFF;
		break;
	default:
		_E("Current mobile state is (%d)", current);
		ret = launch_mobiledata_error(ad, current);
		if (ret < 0)
			_E("Failed to launch error popup (%d)", ret);
		return 0;
	}

	return vconf_set_bool(VCONFKEY_3G_ENABLE, state);
}

static int launch_mobiledata_popup(int current)
{
	int ret;
	bundle *b;

	b = bundle_create();
	if (!b) {
		_E("Failed to make bundle");
		return -ENOMEM;
	}

	appsvc_set_operation(b, APPSVC_OPERATION_VIEW);
	appsvc_add_data(b, "launch", "popup");
	appsvc_set_pkgname(b, "org.tizen.clocksetting.network-mobile-data");

	ret = appsvc_run_service(b, 0, NULL, NULL);
	if (ret < 0)
		_E("Failed to launch mobiledata app(%d)", ret);
	bundle_free(b);

	return ret;
}

static bool mobiledata_terminate(void)
{
	return false;
}

static bool mobiledata_enabled(void)
{
	int state;
	if (vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &state) == 0
			&& state == 1)
		return false;
	return true;
}

static int mobiledata_item_type(void)
{
	return HALF_ITEM;
}

static int mobiledata_id(void)
{
	return MOBILEDATA_ID;
}

static int mobiledata_icon(char *icon, unsigned int len)
{
	int state;

	if (!icon)
		return -EINVAL;

	state = get_current_state();
	if (state != MOBILEDATA_ON && state != MOBILEDATA_OFF)
		state = MOBILEDATA_BT;

	snprintf(icon, len, "%s", item_icon[state]);
	return 0;
}

static int mobiledata_text(char *text, unsigned int len)
{
	if (!text)
		return -EINVAL;
	snprintf(text, len, "%s", MOBILEDATA_TEXT);
	return 0;
}

static int mobiledata_clicked(void *data)
{
	int state, ret;
	struct device_option *opt = data;

	if (!opt)
		return -EINVAL;

	state = get_current_state();
	_I("Mobile data clicked. Current mobiledata state (%d)", state);

	switch (state) {
	case MOBILEDATA_NOTSUP:
	case MOBILEDATA_NOSIM:
	case MOBILEDATA_BT:
		ret = launch_mobiledata_error(opt->ad, state);
		break;

	case MOBILEDATA_ON:
	case MOBILEDATA_OFF:
		if (get_check_status(state))
			ret = update_mobiledata(opt->ad, state);
		else {
			ret = launch_mobiledata_popup(state);
			popup_terminate();
		}
		break;

	default:
		_E("Unknown mobiledata state (%d)", state);
		ret = -EINVAL;
		break;
	}

	if (ret < 0) {
		_E("Failed to launch mobiledata popup (%d)", ret);
		popup_terminate();
	}

	return 0;
}

static int mobiledata_reg_handlers(void *data)
{
	return vconf_notify_key_changed(VCONFKEY_3G_ENABLE, mobiledata_changed, data);
}

static int mobiledata_unreg_handlers(void *data)
{
	vconf_ignore_key_changed(VCONFKEY_3G_ENABLE, mobiledata_changed);
	return 0;
}

static struct device_option mobiledata_ops = {
	.name                = MOBILEDATA_NAME,
	.terminate           = mobiledata_terminate,
	.is_enabled          = mobiledata_enabled,
	.get_item_type       = mobiledata_item_type,
	.get_id              = mobiledata_id,
	.get_icon            = mobiledata_icon,
	.get_text            = mobiledata_text,
	.response_clicked    = mobiledata_clicked,
	.clicked_data        = NULL,
	.register_handlers   = mobiledata_reg_handlers,
	.unregister_handlers = mobiledata_unreg_handlers,
};

static __attribute__ ((constructor)) void register_mobiledata_item(void)
{
	if (enhanced_power_saving_mode())
		return;
	register_device_options_item(&mobiledata_ops);
}
