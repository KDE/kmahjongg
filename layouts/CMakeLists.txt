
########### install files ###############

FILE(GLOB layoutfiles "${CMAKE_CURRENT_SOURCE_DIR}/*.layout" )
FILE(GLOB desktopfiles "${CMAKE_CURRENT_SOURCE_DIR}/*.desktop" )
INSTALL(FILES ${layoutfiles} DESTINATION  ${DATA_INSTALL_DIR}/kmahjongg/layouts )
INSTALL(FILES ${desktopfiles} DESTINATION ${DATA_INSTALL_DIR}/kmahjongg/layouts )

