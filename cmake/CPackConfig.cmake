# CPack configuration file
# Keep packaging configuration separate to respect Single Responsibility (SRP)
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

# DATOS REALES DE CONTACTO
set(CPACK_PACKAGE_CONTACT "Juan Carlos Ponce <jcpd4@alu.ua.es>")

# Comprobamos la plataforma para definir el tipo de paquete
if(WIN32)
  # Windows: ZIP portable
  set(CPACK_GENERATOR "ZIP")
else()
  # Linux: DEB + TGZ
  set(CPACK_GENERATOR "DEB;TGZ")
endif()

# Debian metadata
# DATOS REALES DE MANTENEDOR (Requerido para paquetes .deb válidos)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Juan Carlos Ponce <jcpd4@alu.ua.es>")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# Optional: Add dependencies for the Debian package (common ones)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.29)")

# Additional meta info
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fire & Water - DCA Project built with Raylib")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")

# For ZIP outputs on Windows we prefer to not have many extra files - the install rules will put the files in the right folders

include(CPack)