
########### install files ###############

FILE(GLOB layoutfiles "${CMAKE_CURRENT_SOURCE_DIR}/*.layout")
FILE(GLOB desktopfiles "${CMAKE_CURRENT_SOURCE_DIR}/*.desktop")
INSTALL(FILES ${layoutfiles} DESTINATION  ${KDE_INSTALL_DATADIR}/kmahjongg/layouts)
INSTALL(FILES ${desktopfiles} DESTINATION ${KDE_INSTALL_DATADIR}/kmahjongg/layouts)

