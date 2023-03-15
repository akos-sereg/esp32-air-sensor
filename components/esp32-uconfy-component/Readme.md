# Basic Usage

```
// this function is called once, when configs are fetched from remote server at the
// first time. periodic updates are still running, and you can call uconfig_get_int_param
// or uconfig_get_string_param later in your code to use the most recent values
void configs_first_time_fetched() {
    printf("Config values have been fetched from UConfig:\n");
    printf("PIN Code: %d\n", uconfig_get_int_param("pin_code", 1234));
}

void app_main()
{
    // load previously fetched configs from nvs store. once this function completes, it is
    // safe to call uconfig_get_string_param or uconfig_get_int_param functions.
    uconfig_load_from_nvs();
    
    // when calling this for the first time, your hardcoded wifi settings will be used when
    // connecting to wifi. once configs are read from remote server, wifi settings will 
    // be stored in NVS storage (with keys 'primary_wifi_ssid' and 'primary_wifi_password'),
    // and these parameters will be used the next time. you need to make sure that 
    // primary_wifi_ssid and primary_wifi_password are defined on the portal, otherwise the
    // code will alwaysa fall back to the hardcoded credentials.
    uconfig_initialize_wifi(
        uconfig_get_string_param("primary_wifi_ssid", "<your-wifi-ssid>"), 
        uconfig_get_string_param("primary_wifi_password", "<your-wifi-password>")
    );
    
    // start polling for configs
    uconfig_start("<device_id>", "<api_key>", &configs_first_time_fetched, 60);
}
```

# Library Documentation

## Connect to managed wifi
```
uconfig_initialize_wifi(char *wifi_ssid, char *wifi_password);
```

If your wifi connection is managed by uconfig, you can override WIFI settings from UConfig 
portal, eg: you can define primary and secondary WIFI configs (ssid+password pairs), those 
configs will be stored on your device's NVS storage, and the hardcoded `wifi_ssid` + `wifi_password` 
pair will no longer be used. This is useful if you want to move your device to another location 
(different wifi zone), and you dont want to re-deploy your code to your device.

## Start polling for configs
```
uconfig_start(char *device_id, char *api_key, void (*callback)(), int poll_seconds);
```

Above function will initialize NVS storage for the fetched config values, and starts a background thread that keeps
polling configs at `poll_seconds` interval.

| Argument  | Description |
| ------------- | ------------- |
| `device_id`  | This parameter is coming from UConfig portal  |
| `api_key`  | This parameter is coming from UConfig portal  |
| `callback`  | Function that is called once, after the configs are fetched from the API  |
| `poll_seconds`  | Config fetching interval  |

## Get config

```
uconfig_get_string_param(char *key, char *fallback); 
```

Retrieves a string value of a given `key`, with the following strategy:
- Lookup NVS storage, use value if it exists. When value is found, it means that the device already
  fetched it from remote server.
- Use fallback argument. The hardcoded value you defined as `fallback` can be considered as initial value.

```
uconfig_get_int_param(char *key, int fallback); 
```

Retrieves an integer value of a given `key`, with the same strategy as mentioned above.

## Diag data

https://esp32.com/viewtopic.php?t=5759
https://www.londatiga.net/it/iot/how-to-get-hardware-info-of-esp32/

## TODOs

- Remove deprecated functions from network.c: tcpip_adapter_init and esp_event_loop_init