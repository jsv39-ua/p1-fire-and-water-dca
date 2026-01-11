include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT "Javier Sánchez Villaescusa <jsv39@alu.ua.es>")

# Comprobación de la versión del sistema que empaqueta
if(WIN32)

  set(CPACK_GENERATOR "ZIP")
else()

  set(CPACK_GENERATOR "DEB;TGZ")
endif()

# Metadatos del paquete
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Javier Sánchez Villaescusa <jsv39@alu.ua.es>")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.29)")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fire & Water - DCA Project built with Raylib")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")


include(CPack)