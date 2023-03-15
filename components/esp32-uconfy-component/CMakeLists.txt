idf_component_register(SRCS "config_repo.c" "uconfy.c" "fetch_configs.c" "flush_logs.c" "nvs.c" "uconfy_networking.c" "logging.c" "base64.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash wpa_supplicant)