idf_component_register(
  SRCS "main.c"
  REQUIRES picoruby-esp32 uart-file-transfer
  PRIV_REQUIRES spi_flash fatfs wear_levelling vfs
  INCLUDE_DIRS ""
)

set(image ../storage)
fatfs_create_rawflash_image(storage ${image} FLASH_IN_PROJECT PRESERVE_TIME)
