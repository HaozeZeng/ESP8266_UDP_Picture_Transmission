
idf_component_register(SRCS 
                    "main.c"
                    "station.c" 
                    "PictureTranmission.c" "PictureProcess.c"
                    INCLUDE_DIRS "." REQUIRES mqtt pthread json nvs_flash)
